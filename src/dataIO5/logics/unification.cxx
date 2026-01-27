#include "unification.H"
#include "FuncExp.H"
#include "VarExp.H"
#include "utils/utils.H"

#include <sstream>
// #include <iostream> // only for debugging

#include <cassert>

// static auto& out = std::cout;

namespace logics
{

Exp Substitution::apply(VarExp var) const {
    auto found = var2exp.find(var->getName());
    if (found == var2exp.end()) {
        // not found
        return var;
    }
    else {
        // found
        return found->second;
    }
}

void Substitution::addSequentially(Sp<const Str> var, Exp t) {
    assert(var2exp.find(var) == var2exp.end()); // Es darf noch keinen Eintrag mit Key `var` geben.
    Substitution tmp;
    tmp.var2exp[var] = t;
    for (auto& [key, value] : var2exp) {
#ifdef WITH_OPTIMIZATION
        value = value->substitute(tmp);
#else
        var2exp[key] = value->substitute(tmp);
#endif
    }
    var2exp[var] = t;
    // out << "nach addSequentially: " << toString() << "\n";
}

Str Substitution::toString() const {
    std::stringstream s;
    s << "{ ";
    bool first = true;
    for (const auto& entry : this->var2exp) {
        if (first) {
            first = false;
        }
        else {
            s << ", ";
        }
        s << *entry.first << "->" << entry.second->toString();
    }
    s << " }";
    return s.str();

}

Unification::Unification(const Exp& a, const Exp& b) : s() {
    // unification algorithm for terms
    // The substitution is built on-the-fly.
    std::unique_ptr<Substitution> s1;
    if (a->unifyWith(b, s1)) {
        if (s1) {
            s = std::move(s1);
        }
        else {
            static const auto emptyInstance = std::make_shared<const Substitution>();
            s = emptyInstance;
        }
    }
}

} // namespace logics
