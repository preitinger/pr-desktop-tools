#include "Func.H"
#include "utils/utils.H"
#include "utils/ordering.H"

#include <sstream>

namespace logics
{


std::strong_ordering CFunc::operator<=>(const CFunc& o) const {
    if (this == &o) return std::strong_ordering::equal;
    return compareTuples(std::tie(this->name, this->argNum), std::tie(o.name, o.argNum));
}

Str CFunc::toString() const {
    std::stringstream ss;
    ss << *name; // << " [" << argNum << "]";
    return ss.str();
}

} // namespace logics

