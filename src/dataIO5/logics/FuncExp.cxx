#include "FuncExp.H"
#include "VarExp.H"
#include "utils/utils.H"
#include "utils/ordering.H"

#include <sstream>

namespace logics {

CFuncExp::CFuncExp(Func func, Vec<Exp> args) : func(std::move(func)), args(std::move(args)) {}

std::strong_ordering IFuncExp::vcmp(const CExp& other) const {
    const IFuncExp& o = static_cast<const IFuncExp&>(other);
    if (this == &o) return std::strong_ordering::equal;
    return compareTuples(std::tie(this->getFunc(), this->getArgs()), std::tie(o.getFunc(), o.getArgs()));
}

// virtual
Exp IFuncExp::substitute(const Substitution& s) const {
    // TODO
    // Terme ersetzen und sobald Unterschied neue Funktion bilden;
    // sonst self() zurueckgeben.

    const auto& oldArgs = getArgs();
    auto begin = oldArgs.begin();
    auto end = oldArgs.end();

    for (auto i = begin; i != end; ++i) {
        const auto& arg = *i;
        auto newArg1 = arg->substitute(s);
        if (newArg1.get() != arg.get()) {
            Vec<Exp> newArgs;
            newArgs.reserve(end - begin);
            newArgs.insert(newArgs.end(), begin, i);
            newArgs.push_back(std::move(newArg1));
            for (++i; i != end; ++i) {
                const auto& arg = *i;
                auto newArg1 = arg->substitute(s);
                if (newArg1.get() != arg.get()) {
                    newArgs.push_back(std::move(newArg1));
                }
                else {
                    newArgs.push_back(arg);
                }
            }

            return createClone(newArgs);
        }
    }

    return self();
}

bool IFuncExp::unifyWith(const Exp& other, std::unique_ptr<Substitution>& s) const {
    return other->unifyWithFunc(*this, s);
}

#ifndef WITH_OPTIMIZATION
bool IFuncExp::unifyWithVar(const IVarExp& other, std::unique_ptr<Substitution>& s) const {
    return other.unifyWithFunc(*this, s);
    // if (liftStrongOrdering(getName(), other.getName()) == 0) return true;
    // if (!s) {
    //     s = std::make_unique<Substitution>();
    // }

    // s->var2exp[this->getName()] = other.self();
    // return true;
}
#endif

bool IFuncExp::unifyWithFunc(const IFuncExp& other, std::unique_ptr<Substitution>& s) const {
    if (*this->getFunc() <=> *other.getFunc() != 0) {
        return false;
    }

    for (const auto& [e1, e2] : std::views::zip(getArgs(), other.getArgs())) {
#ifdef WITH_OPTIMIZATION
        if (!e1->unifyWith(e2, s)) return false;
#else
        Exp subst1 = s ? e1->substitute(*s) : e1;
        Exp subst2 = s ? e2->substitute(*s) : e2;
        if (!subst1->unifyWith(subst2, s)) {
            return false;
        }
#endif
    }

    return true;
}

bool IFuncExp::occursIn(const IVarExp& var) const {
    for (const auto& arg : this->getArgs()) {
        if (arg->occursIn(var)) return true;
    }
    return false;
}

Str IFuncExp::toString() const {
    std::stringstream s;
    s << this->getFunc()->toString();
    s << "(";
    bool first = true;
    for (const auto& arg : this->getArgs()) {
        if (first) {
            first = false;
        }
        else {
            s << ", ";
        }
        s << arg->toString();
    }
    s << ")";
    return s.str();
}


} // namespace logics
