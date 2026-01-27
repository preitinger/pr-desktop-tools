#include "TypeExp.H"
#include "utils/SRange.H"
#include "TSLiteral.H"
#include "TSAttributeList.H"
#include "TSReference.H"
#include "TSArray.H"
#include "parse.H"

#include <cassert>

std::strong_ordering TypeExp::operator<=>(const TypeExp& other) const {
    if (this == &other) return std::strong_ordering::equal; // Optimierung, da wir bei Substitutionen ohne Änderung wegen immutable-Prinzip dasselbe Objekt behalten.

    if (auto cmp = rank() <=> other.rank(); cmp != 0) {
        return cmp;
    }
    return vcmp(other);
}

// // static
// Sp<const TypeExp> TypeExp::parse(SRange& r) {
//     r.skipWs();

//     Sp<const TypeExp> baseType;

//     if (r.inAnyRange({ {'{'} })) {
//         baseType = ms<TSAttributeList>(r);
//     }
//     else if (firstOfName(r)) {
//         baseType = ms<TSReference>(r);
//     }
//     r.skipWs();
//     if (r.inAnyRange({ {'['} })) {
//         r.skipKeyword("[]");
//         return ms<TSArray>(baseType);
//     }
//     else if (r.inAnyRange({ {'|'} })) {
//         Vec<Sp<const TypeExp>> alternatives;

//         do {
//             r.skipKeyword("|");
//             alternatives.push_back(std::move(baseType));
//             baseType = parse(r);
//         } while (r.inAnyRange({ {'|'} }));

//     }

// }
