#include "unification.H"
#include "VarExp.H"
#include "FuncExp.H"
#include "utils/utils.H"
#include "utils/ordering.H"

#include <iostream>

namespace logics
{

auto& out = std::cout;

// TestForwarding::TestForwarding(SV&& name) : name(std::move(name)) {
//     out << "TestForwarding(&&) [" << name << "]\n";
// }

// TestForwarding::TestForwarding(const SV& name) : name(name) {
//     out << "TestForwarding(const&)\n";
// }
// TestForwarding::TestForwarding(SV name) : name(std::move(name)) {
//     out << "TestForwarding(rval)\n";
// }

// TestForwarding::TestForwarding(const TestForwarding& other) : name(other.name) {
//     out << "TestForwarding(const TestForwarding&)\n";
// }
    
// TestForwarding::TestForwarding(TestForwarding&& other) : name(std::move(other.name)) {
//     out << "TestForwarding(TestForwarding&&)\n";
// }

// VarExp varExp(SV name) {
//     return ms<CVarExp>(std::move(name));
// }

std::strong_ordering IVarExp::vcmp(const CExp& other) const {
    const IVarExp& o = static_cast<const IVarExp&>(other);
    if (this == &o) return std::strong_ordering::equal;
    return liftStrongOrdering(getName(), o.getName());
}

// virtual
Exp IVarExp::substitute(const Substitution& s) const {
    return s.apply(self());
}

bool IVarExp::unifyWith(const Exp& other, std::unique_ptr<Substitution>& s) const {
    return other->unifyWithVar(*this, s);
}

bool IVarExp::unifyWithVar(const IVarExp& other, std::unique_ptr<Substitution>& s) const {
    if (liftStrongOrdering(getName(), other.getName()) == 0) return true;
    if (!s) {
        s = std::make_unique<Substitution>();
    }

    s->addSequentially(this->getName(), other.self());
    // s->var2exp[this->getName()] = other.self();
    return true;
}

bool IVarExp::unifyWithFunc(const IFuncExp& other, std::unique_ptr<Substitution>& s) const {
    if (other.occursIn(*this)) {
        // out << "[Occurs Check - Abbruch!]\n";
        return false;
    }

    if (!s) {
        s = std::make_unique<Substitution>();
    }

    // Alle bestehenden Variablensubstitutionen muessen mit der neuen verknüpft
    s->addSequentially(this->getName(), other.self());
    // s->var2exp[this->getName()] = other.self();
    return true;
}

bool IVarExp::occursIn(const IVarExp& var) const {
    return *this <=> var == 0;
}

Str IVarExp::toString() const {
    return *this->getName();
}

void IVarExp::dump() const {
    out << "[CVarExp '" << *this->getName() << "']\n";
}

} // namespace logics

