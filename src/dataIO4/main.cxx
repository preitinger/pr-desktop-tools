#include "utils/SRange.H"
#include "maps.H"
#include "utils/utils.H"
#include "Decl.H"
#include "TSLiteral.H"
#include "DeclCol.H"
#include "parse.H"
#include "TSReference.H"
#include "utils/Output.H"
#include "Substitution.H"
#include "logics/test.H"

#include <iostream>
#include <compare>

#include <cassert>

auto& out = std::cout;

void testCompare() {
    CompareStr c;
    using S = Sp<const Str>;
    S a = ms<Str>("a");
    S b = ms<Str>("b");
    S a2 = ms<Str>("a");
    assert(c(a, b));
    assert(!c(b, a));
    assert(!c(a, a2));
    assert(!c(a2, a));
    assert(!c(a, a));
    assert(!c(a2, a2));

    out << "testCompare done\n";
}

class TestBase {
private:
    int typeNum;
public:
    TestBase(int typeNum) : typeNum(typeNum) {}
    virtual int typeRank() const = 0;
    virtual ~TestBase() {}

    auto operator<=>(const TestBase& other) const {
        if (auto cmp = typeRank() <=> other.typeRank(); cmp != 0) {
            return cmp;
        }

        return vcmp(other);
    }

protected:
    virtual std::strong_ordering vcmp(const TestBase& other) const = 0;
};

class TestSub2;

class TestSub1 : public TestBase {
    int x;
public:
    TestSub1(int x) : TestBase(1), x(x) {}
    virtual ~TestSub1() {}
    int typeRank() const override { return 1; }

    // protected:
    virtual std::strong_ordering vcmp(const TestBase& other) const override;
};

class TestSub2 : public TestBase {
    int x;
public:
    TestSub2(int x) : TestBase(2), x(x) {}
    virtual ~TestSub2() {}
    int typeRank() const override { return 2; }

    // protected:
    virtual std::strong_ordering vcmp(const TestBase& other) const override;
};

class TestSub2a : public TestSub2 {
public:
    virtual ~TestSub2a() { out << "~TestSub2a()\n"; }
};

std::strong_ordering TestSub1::vcmp(const TestBase& other) const {
    const TestSub1& other1 = static_cast<const TestSub1&>(other);

    return x <=> other1.x;
}

std::strong_ordering TestSub2::vcmp(const TestBase& other) const {
    const TestSub2& other2 = static_cast<const TestSub2&>(other);

    return x <=> other2.x;
}

// int TestSub1::cmp(const TestSub2& other) const { out << "TestSub1::cmp(TestSub2)\n"; return -1; }
// int TestSub1::cmp(const TestSub1& other) const { out << "TestSub1::cmp(TestSub1)\n"; return 0; }
// int TestSub2::cmp(const TestSub2& other) const { out << "TestSub2::cmp(TestSub2)\n"; return 0; }
// int TestSub2::cmp(const TestSub1& other) const { out << "TestSub2::cmp(TestSub1)\n"; return 1; }

void testSubs() {
    Sp<const TestBase> a = ms<TestSub1>(20);
    Sp<const TestBase> b = ms<TestSub2>(10);
    auto cmp = a <=> b;
    out << "Vergleich: ";
    if (cmp == 0) {
        out << "gleich";
    }
    else if (cmp < 0) {
        out << "kleiner";
    }
    else {
        assert(cmp > 0);
        out << "groesser";
    }
    out << "\n";

    assert(a < b);
    assert(!(b < a));
    assert(!(a == b));
}

void testMaps() {
    Name2Decl m;
    m.emplace(ms<Str>("key"), ms<Decl>(ms<Str>("Wert"), Vec<Sp<const Str>>(), Sp<const TypeExp>()));

    Type2Decl m2;
    Sp<const TypeExp> exampleLiteral = ms<TSLiteral>('\'', Str("x"));
    Sp<const TypeExp> exampleLiteral2 = ms<TSLiteral>('\'', Str("y"));
    Sp<const TypeExp> exampleLiteral3 = ms<TSLiteral>('\'', Str("a"));
    Sp<const TypeExp> exampleLiteral4 = ms<TSLiteral>('\'', Str("a"));

    Sp<Decl> exampleDecl = ms<Decl>(ms<Str>("Wert"), Vec<Sp<const Str>>(), exampleLiteral);
    m2.emplace(exampleLiteral, exampleDecl);
    m2.emplace(exampleLiteral2, exampleDecl);
    m2.emplace(exampleLiteral3, exampleDecl);
    m2.emplace(exampleLiteral4, exampleDecl);
    out << "rank: " << m2.begin()->second->exp->rank() << "\n";

    for (const auto& x : m2) {
        Sp<const TSLiteral> asLit = std::dynamic_pointer_cast<const TSLiteral>(x.first);
        out << asLit;
        if (asLit) {
            out << ": " << asLit->val;
        }

        out << "\n";
    }

    DeclCol col;
    auto found = col.find(exampleLiteral);
    if (found) {
        out << "found not empty\n";
    }
    else {
        out << "found empty\n";
    }
    found = col.find(exampleLiteral2);
    if (found) {
        out << "exampleLiteral2 found\n";
    }
    else {
        out << "exampleLiteral2 not found\n";
    }
    col.add(exampleDecl);
    out << "added exampleDecl, then:\n";
    found = col.find(exampleLiteral);
    if (found) {
        out << "exampleLiteral found\n";
    }
    else {
        out << "exampleLiteral not found\n";
    }
    found = col.find(exampleLiteral2);
    if (found) {
        out << "exampleLiteral2 found\n";
    }
    else {
        out << "exampleLiteral2 not found\n";
    }
    auto found2 = col.find(exampleDecl->exp);
    out << "found2 = " << found2 << "\n";
    auto foundX = col.find(ms<Str>("x"));
    out << "foundX = " << foundX << "\n";
    auto foundWert = col.find(ms<Str>("Wert"));
    out << "foundWert = " << foundWert << "\n";
}

void testParse() {
    Str input = readAllFromFile("testInput.txt");
    SRange r(input.begin(), input.end());
    Output output(out);

    try {
        // r.skipWs();
        do {
            auto decl = parseTypeDecl(r);
            if (!decl) {
                out << "Alles geparst.\n\n";
                break;
            }
            out << "after decl creation\n";
            decl->exp->genType(output);
            output << "\n\n";
        } while (true);
    }
    catch (const ParseException& e) {
        e.printStackTrace();
    }
    catch (const SRangeException& e) {
        e.printStackTrace();
    }
    catch (const std::exception& any) {
        std::cerr << any.what();
    }
}

Sp<const Str> name(const char* x) {
    return ms<Str>(x);
}

Sp<const TypeExp> templateArg(const char* x) {
    return ms<TSReference>(name(x), Vec<Sp<const TypeExp>>());
}

void testLifts() {
    TSReference a(name("A"), { templateArg("T"), templateArg("S") });
    TSReference b(name("B"), { templateArg("T"), templateArg("S") });
    TSReference a2(name("A"), { templateArg("Z") });
    TSReference a3(name("A"), { templateArg("Z"), templateArg("A") });
    TSReference a4(name("A"), { templateArg("Z"), templateArg("A") });
    TSReference a5(name("A"), { templateArg("Z"), templateArg("B") });
    assert(a.vcmp(b) < 0);
    assert(b.vcmp(a) > 0);
    assert(a2.vcmp(a) < 0);
    assert(a.vcmp(a2) > 0);
    assert(a.vcmp(a3) < 0);
    assert(a3.vcmp(a) > 0);
    assert(a3.vcmp(a3) == 0);
    assert(a3.vcmp(a4) == 0);
    assert(a4.vcmp(a3) == 0);
    assert(a4.vcmp(a5) < 0);
    assert(a5.vcmp(a4) > 0);
    out << "testLifts erfolgreich!\n";
}

int testTailRc(int x) {
    if (x <= 1) return x;
    return x * testTailRc(x - 1);
}

int main() {
    // Str s("Bla");
    // SRange range(s.begin(), s.end());

    // Sp<int> x = ms<int>(1);

    // testCompare();

    // testSubs();
    // testMaps();
    // testParse();
    // testLifts();

    // out << "100! = " << testTailRc(100);

    logics::test();

    out << "\n\n";
    return 0;
}