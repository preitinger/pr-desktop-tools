#include "TSAttributeList.H"
#include "utils/ordering.H"
#include "TSAttribute.H"
#include "utils/Output.H"
#include "format.H"

#include <cassert>

std::strong_ordering TSAttributeList::vcmp(const TypeExp& other) const {
    const TSAttributeList& o = static_cast<const TSAttributeList&>(other);
    if (this == &o) return std::strong_ordering::equal;
    return liftStrongOrdering(attributes, o.attributes);
}

void TSAttributeList::genType(Output& out) const {
    out << "{ ";
    format(out, attributes, "; ", [](Output& out, const Sp<const TSAttribute>& attribute)
        {
            attribute->gen(out);
        });
    out << " }";
}

int TSAttributeList::maxUnderscores() const {
    assert(false);
}

Sp<const TypeExp> TSAttributeList::subst(const DeclCol& decls) const {
    assert(false);
}

Sp<const TypeExp> TSAttributeList::abstr(DeclCol& decls) const {
    assert(false);
}

// virtual
// Sp<const TypeExp> TSAttributeList::substitute(const Substitution& s) const {
//     // std::vector<Sp<const TSAttribute>> newAttributes;
//     // newAttributes.reserve(attributes.size());

//     // for (const auto& attribute : attributes) {
//     //     newAttributes.push_back(ms<TSAttribute>(attribute->name, attribute->type->substitute(s)));
//     // }
//     // return ms<TSAttributeList>(std::move(newAttributes));

//     return ms<TSAttributeList>(mapVec(attributes, [&s](const auto& attribute)
//         {
//             auto newType = attribute->type->substitute(s);
//             if (newType.get() != attribute->type.get()) {
//                 return ms<const TSAttribute>(attribute->name, newType);
//             }
//             else {
//                 return attribute;
//             }
//         })
//     );
// }

Sp<const TypeExp> TSAttributeList::substitute(const Substitution& s) const {
    size_t n = attributes.size();

    for (size_t i = 0; i < n; ++i) {
        const auto& attr = attributes[i];
        auto newType = attr->type->substitute(s);

        if (newType.get() != attr->type.get()) {
            // Erst jetzt: Wir wissen, wir brauchen einen neuen Vektor!
            Vec<Sp<const TSAttribute>> newAttributes;
            newAttributes.reserve(n);
            // 1. Die bisherigen, unveränderten Elemente übernehmen
            newAttributes.insert(newAttributes.end(), attributes.begin(), attributes.begin() + i);

            // 2. Das aktuell geänderte Element einfügen
            newAttributes.push_back(ms<const TSAttribute>(attr->name, std::move(newType)));

            // 3. Den Rest der Liste transformieren
            for (size_t j = i + 1; j < n; ++j) {
                const auto& a = attributes[j];
                auto t = a->type->substitute(s);
                if (t.get() != a->type.get()) {
                    newAttributes.push_back(ms<const TSAttribute>(a->name, std::move(t)));
                }
                else {
                    newAttributes.push_back(a);
                }
            }

            return ms<TSAttributeList>(std::move(newAttributes));
        }
    }

    // Wenn wir hier ankommen, war KEINE Änderung nötig.
    // Wir sparen uns: 1 Vektor-Allokation, N Shared-Ptr-Kopien, 1 List-Allokation.
    return shared_from_this();
}
