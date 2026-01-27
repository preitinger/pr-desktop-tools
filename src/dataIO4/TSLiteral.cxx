#include "TSLiteral.H"
#include "utils/Output.H"

#include <compare>

#include <cassert>

std::strong_ordering TSLiteral::vcmp(const TypeExp& other) const {
    const TSLiteral& other1 = static_cast<const TSLiteral&>(other);

    if (this == &other1) return std::strong_ordering::equal;
    // quote bewusst hier ignoriert!
    return this->val <=> other1.val;
}

void TSLiteral::genType(Output& out) const {
    out << quote;
    for (char c : val) {
        switch (c) {
        case '\\':
            out << "\\\\";
            break;
        default:
            if (c == quote) {
                out << '\\' << c;
            } else {
                out << c;
            }
        }
    }
    out << quote;
}

int TSLiteral::maxUnderscores() const {
    return 0;
}

Sp<const TypeExp> TSLiteral::subst(const DeclCol& decls) const {
    return shared_from_this();
}

Sp<const TypeExp> TSLiteral::abstr(DeclCol& names) const {
    return shared_from_this();
}

Sp<const TypeExp> TSLiteral::substitute(const Substitution& s) const {
    return self();
}
