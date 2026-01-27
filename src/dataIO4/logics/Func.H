#pragma once

#include "utils/utils.H"

#include <memory>
#include <utility>

namespace logics {

class CFunc {
public:
    Sp<const Str> name;
    int argNum;
    CFunc(Sp<const Str> name, int argNum) : name(std::move(name)), argNum(argNum) {}

    std::strong_ordering operator<=>(const CFunc&) const;
    Str toString() const;
};

using Func = std::shared_ptr<const CFunc>;

inline Func func(SV name, int argNum) {
    return ms<CFunc>(ms<const Str>(std::move(name)), argNum);
}

// template<class... Args>
// Func func(Args... args) {
//     return ms<CFunc>(std::forward<Args>(args)...);
// }


} // namespace logics
