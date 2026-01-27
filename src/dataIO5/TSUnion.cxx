#include "TSUnion.H"
#include "utils/ordering.H"
#include "utils/Output.H"
#include "format.H"

#include <cassert>

std::strong_ordering TSUnion::vcmp(const TypeExp& other) const {
    const TSUnion& o = static_cast<const TSUnion&>(other);
    if (this == &o) return std::strong_ordering::equal;
    return liftStrongOrdering(alternatives, o.alternatives);

}

void TSUnion::genType(Output& out) const {
    format(out, alternatives, " | ", [](Output& out, const Sp<const TypeExp>& alternative)
        {
            alternative->genType(out);
        }
    );
}

int TSUnion::maxUnderscores() const {
    assert(false);
}

Sp<const TypeExp> TSUnion::subst(const DeclCol& decls) const {
    assert(false);
}

Sp<const TypeExp> TSUnion::abstr(DeclCol& decls) const {
    assert(false);
}

// virtual
Sp<const TypeExp> TSUnion::substitute(const Substitution& s) const {
    // Prinzip Copy on write:

    auto begin = this->alternatives.begin();
    auto end = this->alternatives.end();

    for (auto i = begin; i != end; ++i) {
        const Sp<const TypeExp>& alternative = *i;
        auto newAlternative = alternative->substitute(s);
        if (newAlternative.get() != alternative.get()) {
            Vec<Sp<const TypeExp>> newAlternatives;
            newAlternatives.reserve(end - begin);
            newAlternatives.insert(newAlternatives.end(), begin, i);
            newAlternatives.push_back(std::move(newAlternative));
            for (++i; i != end; ++i) {
                const Sp<const TypeExp>& alternative = *i;
                auto newAlternative2 = alternative->substitute(s);
                if (newAlternative2.get() != alternative.get()) {
                    newAlternatives.push_back(std::move(newAlternative2));
                }
                else {
                    newAlternatives.push_back(alternative);
                }
            }

            return ms<TSUnion>(newAlternatives);
        }
    }

    // Keine einzige Änderung, der gesamte Union bleibt unverändert, es gibt überhaupt keinen Allokationsaufwand!
    return shared_from_this();
}
