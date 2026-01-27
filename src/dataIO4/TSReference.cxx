#include "Substitution.H"
#include "TSReference.H"
#include "utils/utils.H"
#include "utils/ordering.H"
#include "utils/Output.H"
#include "format.H"

#include <memory>

#include <cassert>

std::strong_ordering TSReference::vcmp(const TypeExp& other) const {
    
    const TSReference& o = static_cast<const TSReference&>(other);
    if (this == &o) return std::strong_ordering::equal;
    // manuell
    // if (auto cmp = liftStrongOrdering(this->name, o.name); cmp != 0) {
    //     return cmp;
    // }
    // return liftStrongOrdering(this->templateArgs, o.templateArgs);
    // dank KI
    return compareTuples(std::tie(name, templateArgs), std::tie(o.name, o.templateArgs));
}

void TSReference::genType(Output& out) const {
    assert(name);
    out << *name;
    if (!templateArgs.empty()) {
        out << '<';
        format(out, templateArgs, ", ", [](Output& out, const Sp<const TypeExp>& arg)
            {
                arg->genType(out);
            });

        out << '>';
    }
}

int TSReference::maxUnderscores() const {
    assert(false);
}

Sp<const TypeExp> TSReference::subst(const DeclCol& decls) const {
    assert(false);
}

Sp<const TypeExp> TSReference::abstr(DeclCol& decls) const {
    assert(false);
}


// virtual
Sp<const TypeExp> TSReference::substitute(const Substitution& s) const {
    return s.apply(self());
}
