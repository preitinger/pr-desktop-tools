#include "Exp.H"
#include "utils/utils.H"


namespace logics {

std::strong_ordering CExp::operator<=>(const CExp& o) const {
    if (this == &o) return std::strong_ordering::equal;
    if (auto cmp = rank() <=> o.rank(); cmp != 0) {
        return cmp;
    }
    return vcmp(o);
}

} // namespace logics