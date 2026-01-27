#include "test.H"
#include "unification.H"
#include "Func.H"
#include "FuncExp.H"
#include "VarExp.H"
#include "utils/utils.H"

#include <iostream>
#include <cassert>

namespace logics {

static auto& out = std::cout;

static int count = 0;
static bool silent = true;

void testUnification(const Exp& a, const Exp& b, bool expectedSuccess) {
    Unification u(a, b);

    if (!silent) out << a->toString() << " und " << b->toString() << " sind ";
    if (u.empty()) {
        if (!silent) out << "nicht unifizierbar!\n\n";
    }
    else {
        assert(u.substitution());
        if (!silent) out << "unifizierbar durch: " << u.substitution()->toString() << "\n";
        if (!silent) out << "Nach Substitution: " << a->substitute(*u.substitution())->toString() << " bzw. hoffentlich gleich " << b->substitute(*u.substitution())->toString() << "\n\n";
        assert(*a->substitute(*u.substitution()) <=> *b->substitute(*u.substitution()) == 0);
    }

    assert(u.empty() != expectedSuccess);
    if (u.empty() == expectedSuccess) {
        out << "FEHLER: " << a->toString() << " & " << b->toString() << "\n";
    }
    ++count;
}

void test() {
    silent = false;
    {    // VarExp x1 = varExp(TestForwarding("X"));
        SV nameX("X");
        VarExp x1 = varExp(std::move(nameX));
        // assert(*x1 == *x1);
        // VarExp x2 = varExp(TestForwarding("X"));
        VarExp x2 = varExp(("X"));
        const IVarExp& vx1 = *x1;
        const IVarExp& vx2 = *x2;
        assert((vx1 <=> vx2) == 0);
        // TestForwarding tfy("Y");
        out << "vor y =\n";
        auto y = varExp("Y");
        out << "nach y =\n";
        auto y2 = varExp("Y");
        out << "nach y= 2\n";
        assert(*x1 < *y);
        assert((*y <=> *y2) == 0);

        FuncExp f = funcExp("f", { varExp("x"), varExp("y") });
        out << "argNum von f: " << f->getFunc()->argNum << "\n";
        out << "f->func->toString() " << f->getFunc()->toString() << "\n";
        assert(*x1 > *f);
        FuncExp f2 = funcExp(("f"), { varExp("x"), varExp("x") });
        assert(*f > *f2);
        FuncExp f3 = funcExp(("f"), { varExp("x"), varExp("z") });
        assert(*f < *f3);
        assert(*f3 > *f);
    }

    {
        Substitution subst; // leere Substitution
        assert(*subst.apply(varExp("X")) <=> *varExp("X") == 0);
    }
    {
        Substitution subst;
        subst.var2exp.emplace(ms<Str>("X"), funcExp("c", {})); // X <- c
        assert((*subst.apply(varExp("X")) <=> *varExp("X")) != 0);
        assert((*subst.apply(varExp("X")) <=> *funcExp("c", {})) == 0);
        auto f_X = funcExp("f", { varExp("X") });
        auto f_Y = funcExp("f", { varExp("Y") });
        assert(*f_X->substitute(subst) <=> *funcExp("f", { funcExp("c", {}) }) == 0);
        assert(*f_Y->substitute(subst) <=> *funcExp("f", { funcExp("c", {}) }) != 0);
        assert(*f_Y->substitute(subst) <=> *f_Y == 0);
        assert(*f_X->substitute(subst) <=> *f_X != 0);
    }

    {
        auto f_X = funcExp("f", { varExp("X") });
        auto f_Y = funcExp("f", { varExp("Y") });
        Unification unification(f_X, f_Y);
        out << "unification.empty() " << unification.empty() << "\n";
        if (unification.substitution()) {
            out << "Unifizierende Substitution: " << unification.substitution()->toString() << "\n";
        }
    }

    for (int i = 0; i < 1; ++i) {

        {
            auto a = funcExp("f", { varExp("X"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
            auto b = funcExp("f", { varExp("Y"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
            testUnification(a, b, true);
            testUnification(b, a, true);
        }

        {
            auto a = funcExp("f", { varExp("X"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
            auto b = funcExp("g", { varExp("Y"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
            testUnification(a, b, false);
            testUnification(b, a, false);
        }
        {
            auto a = funcExp("f", { varExp("X"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
            auto b = funcExp("f", { varExp("X"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
            testUnification(a, b, true);
            testUnification(b, a, true);
        }
        {
            auto a = funcExp("f", { varExp("X"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
            auto b = funcExp("f", { funcExp("f", {varExp("X"), varExp("X")}), funcExp("f", {varExp("X"), funcExp("0", {})}) });
            testUnification(a, b, false);
            testUnification(b, a, false);
        }
        {
            auto a = funcExp("f", { funcExp("f", {varExp("X"), funcExp("0", {})}), varExp("X") });
            auto b = funcExp("f", { funcExp("f", {varExp("X"), funcExp("0", {})}), funcExp("f", {varExp("X"), varExp("X")}) });
            testUnification(a, b, false);
            testUnification(b, a, false);
        }

        {
            // Terme: f(X, Y) und f(Y, g(X))
            // 1. Schritt unifiziert X mit Y -> Substitution: {X -> Y}
            // 2. Schritt versucht Y mit g(X) zu unifizieren.
            // Ein fehlerhafter Occurs-Check sieht in g(X) nur X und denkt "X != Y, alles ok".
            // Der korrekte Check "walked" X zu Y und erkennt: Y kommt in g(Y) vor!
            auto a = funcExp("f", { varExp("X"), varExp("Y") });
            auto b = funcExp("f", { varExp("Y"), funcExp("g", {varExp("X")}) });

            if (!silent) out << "--- Test Indirekter Zyklus (X->Y, Y->g(X)) ---\n";
            testUnification(a, b, false);
            testUnification(b, a, false);
        }
        {
            // Test: Zirkuläre Abhängigkeit ohne direkte Identität
            // Term 1: f(X, X)
            // Term 2: f(Y, g(Y))
            // 1. X wird an Y gebunden {X -> Y}
            // 2. Das zweite X (das jetzt Y ist) soll mit g(Y) unifiziert werden.
            // Hier MUSS der Occurs-Check zuschlagen.
            auto a = funcExp("f", { varExp("X"), varExp("X") });
            auto b = funcExp("f", { varExp("Y"), funcExp("g", {varExp("Y")}) });
            testUnification(a, b, false);
            testUnification(b, a, false);
        }
        {
            if (!silent) out << "\nHintereinanderausfuehrung:\n";
            auto a = funcExp("f", { varExp("X"), varExp("Y") });
            auto b = funcExp("f", { varExp("Y"), funcExp("0", {}) });
            testUnification(a, b, true);
            testUnification(b, a, true);
        }
        {
            if (!silent) out << "\nEtwas komplexere Terme a):\n";
            auto a = funcExp("f", { varExp("X"), funcExp("g", {funcExp("h", {funcExp("i", {})})}) });
            auto b = funcExp("f", { funcExp("g", {funcExp("h", {funcExp("i", {})})}), varExp("X") });
            testUnification(a, b, true);
            testUnification(b, a, true);
        }
        {
            if (!silent) out << "\nEtwas komplexere Terme b):\n";
            auto a = funcExp("f", { varExp("X"), funcExp("g", {funcExp("h", {funcExp("i", {})})}) });
            auto b = funcExp("f", { funcExp("h", {funcExp("h", {funcExp("j", {})})}), varExp("Y") });
            testUnification(a, b, true);
            testUnification(b, a, true);
        }
        {
            if (!silent) out << "\nEtwas komplexere Terme c):\n";
            auto a = funcExp("f", { varExp("X"),                                      funcExp("g", {funcExp("h", {funcExp("i", {})})}), varExp("Y") });
            auto b = funcExp("f", { funcExp("h", {funcExp("h", {funcExp("j", {})})}), varExp("Y"),                                      varExp("Y") });
            testUnification(a, b, true);
            testUnification(b, a, true);
        }
        {
            if (!silent) out << "\nEtwas komplexere Terme d):\n";
            auto a = funcExp("f", { varExp("X"), funcExp("g", {funcExp("h", {funcExp("i", {})})}), varExp("Y") });
            auto b = funcExp("f", { funcExp("h", {funcExp("h", {funcExp("j", {})})}), varExp("Y"), funcExp("g", {funcExp("h", {varExp("Z")})}) });
            testUnification(a, b, true);
            testUnification(b, a, true);
        }
        {
            if (!silent) out << "\nEtwas komplexere Terme e):\n";
            auto a = funcExp("f", { varExp("X"), funcExp("g", {funcExp("h", {funcExp("i", {})})}), varExp("Y") });
            auto b = funcExp("f", { funcExp("h", {funcExp("h", {funcExp("j", {})})}), varExp("Y"), funcExp("g", {funcExp("h", {varExp("Y")})}) });
            testUnification(a, b, false);
            testUnification(b, a, false);
        }
    }

    out << "\n\nTestfaelle insgesamt: " << count << "\n\n";


    auto& out = std::cout;
    out << "\nlogics::test() done.\n\n";
}

void test2() {
    std::vector<Exp> cases;
    cases.reserve(26);
    std::vector<bool> expected;
    expected.reserve(26);

    {
        auto a = funcExp("f", { varExp("X"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
        auto b = funcExp("f", { varExp("Y"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(true);
    }

    {
        auto a = funcExp("f", { varExp("X"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
        auto b = funcExp("g", { varExp("Y"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(false);
    }
    {
        auto a = funcExp("f", { varExp("X"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
        auto b = funcExp("f", { varExp("X"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(true);
    }
    {
        auto a = funcExp("f", { varExp("X"), funcExp("f", {varExp("X"), funcExp("0", {})}) });
        auto b = funcExp("f", { funcExp("f", {varExp("X"), varExp("X")}), funcExp("f", {varExp("X"), funcExp("0", {})}) });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(false);
    }
    {
        auto a = funcExp("f", { funcExp("f", {varExp("X"), funcExp("0", {})}), varExp("X") });
        auto b = funcExp("f", { funcExp("f", {varExp("X"), funcExp("0", {})}), funcExp("f", {varExp("X"), varExp("X")}) });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(false);
    }

    {
        // Terme: f(X, Y) und f(Y, g(X))
        // 1. Schritt unifiziert X mit Y -> Substitution: {X -> Y}
        // 2. Schritt versucht Y mit g(X) zu unifizieren.
        // Ein fehlerhafter Occurs-Check sieht in g(X) nur X und denkt "X != Y, alles ok".
        // Der korrekte Check "walked" X zu Y und erkennt: Y kommt in g(Y) vor!
        auto a = funcExp("f", { varExp("X"), varExp("Y") });
        auto b = funcExp("f", { varExp("Y"), funcExp("g", {varExp("X")}) });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(false);
    }
    {
        // Test: Zirkuläre Abhängigkeit ohne direkte Identität
        // Term 1: f(X, X)
        // Term 2: f(Y, g(Y))
        // 1. X wird an Y gebunden {X -> Y}
        // 2. Das zweite X (das jetzt Y ist) soll mit g(Y) unifiziert werden.
        // Hier MUSS der Occurs-Check zuschlagen.
        auto a = funcExp("f", { varExp("X"), varExp("X") });
        auto b = funcExp("f", { varExp("Y"), funcExp("g", {varExp("Y")}) });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(false);
    }
    {
        if (!silent) out << "\nHintereinanderausfuehrung:\n";
        auto a = funcExp("f", { varExp("X"), varExp("Y") });
        auto b = funcExp("f", { varExp("Y"), funcExp("0", {}) });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(true);
    }
    {
        if (!silent) out << "\nEtwas komplexere Terme a):\n";
        auto a = funcExp("f", { varExp("X"), funcExp("g", {funcExp("h", {funcExp("i", {})})}) });
        auto b = funcExp("f", { funcExp("g", {funcExp("h", {funcExp("i", {})})}), varExp("X") });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(true);
    }
    {
        if (!silent) out << "\nEtwas komplexere Terme b):\n";
        auto a = funcExp("f", { varExp("X"), funcExp("g", {funcExp("h", {funcExp("i", {})})}) });
        auto b = funcExp("f", { funcExp("h", {funcExp("h", {funcExp("j", {})})}), varExp("Y") });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(true);
    }
    {
        if (!silent) out << "\nEtwas komplexere Terme c):\n";
        auto a = funcExp("f", { varExp("X"),                                      funcExp("g", {funcExp("h", {funcExp("i", {})})}), varExp("Y") });
        auto b = funcExp("f", { funcExp("h", {funcExp("h", {funcExp("j", {})})}), varExp("Y"),                                      varExp("Y") });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(true);
    }
    {
        if (!silent) out << "\nEtwas komplexere Terme d):\n";
        auto a = funcExp("f", { varExp("X"), funcExp("g", {funcExp("h", {funcExp("i", {})})}), varExp("Y") });
        auto b = funcExp("f", { funcExp("h", {funcExp("h", {funcExp("j", {})})}), varExp("Y"), funcExp("g", {funcExp("h", {varExp("Z")})}) });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(true);
    }
    {
        if (!silent) out << "\nEtwas komplexere Terme e):\n";
        auto a = funcExp("f", { varExp("X"), funcExp("g", {funcExp("h", {funcExp("i", {})})}), varExp("Y") });
        auto b = funcExp("f", { funcExp("h", {funcExp("h", {funcExp("j", {})})}), varExp("Y"), funcExp("g", {funcExp("h", {varExp("Y")})}) });
        cases.push_back(std::move(a));
        cases.push_back(std::move(b));
        expected.push_back(false);
    }

    auto expectedBegin = expected.begin();
    auto expectedEnd = expected.end();
    auto casesBegin = cases.begin();
    auto casesEnd = cases.end();
    assert(cases.size() == expected.size() * 2);
    out << "expected.size() " << expected.size() << "\n";
    std::vector<bool>::const_iterator expectedIt;
    int n = 100000;
    // silent = false;
    // n = 1;
    for (int i = 0; i < n; ++i) {
        // if (i % 1000 == 0) out << i << "\n";
        expectedIt = expectedBegin;
        for (auto it = casesBegin; it != casesEnd; ++expectedIt, ++it) {
            const auto& a = *it;
            if (++it == casesEnd) break;
            const auto& b = *it;
            assert(expectedIt != expectedEnd);
            // out << ""
            testUnification(a, b, *expectedIt);
            testUnification(b, a, *expectedIt);
        }
    }

    out << "\n\nTestfaelle insgesamt: " << count << "\n\n";

}

} // namespace logics