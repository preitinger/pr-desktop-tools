#include "TSArray.H"
#include "utils/ordering.H"
#include "utils/Output.H"

#include <cassert>

std::strong_ordering TSArray::vcmp(const TypeExp& other) const {
    const TSArray& o = static_cast<const TSArray&>(other);
    if (this == &o) return std::strong_ordering::equal;
    return liftStrongOrdering(baseType, o.baseType);
}

void TSArray::genType(Output& out) const {
    baseType->genType(out);
    out << "[]";
}

int TSArray::maxUnderscores() const {
    assert(false);
}

Sp<const TypeExp> TSArray::subst(const DeclCol& decls) const {
    assert(false);
}

Sp<const TypeExp> TSArray::abstr(DeclCol& decls) const {
    assert(false);
}

// virtual
Sp<const TypeExp> TSArray::substitute(const Substitution& s) const {
    auto newBaseType = this->baseType->substitute(s);
    if (newBaseType.get() != baseType.get()) {
        return ms<TSArray>(std::move(newBaseType));
    }

    return shared_from_this();
}
