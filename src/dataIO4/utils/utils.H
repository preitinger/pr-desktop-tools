#pragma once

#include <string>
#include <memory>
#include <vector>
#include <compare>
#include <ranges>


using Str = std::string;
using CSIt = std::string::const_iterator;
using SV = std::string_view;

template <class T>
using Vec = std::vector<T>;

template <class T>
using Sp = std::shared_ptr<T>;

/**
 * Abk. für std::make_shared
 */
template <typename T, typename... Args>
inline auto ms(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

inline std::strong_ordering compareStrSp(const Sp<const Str>& a, const Sp<const Str>& b) {
    if (!a || !b) {
        return a <=> b;
    }
    return *a <=> *b;
}

std::string readAllFromFile(std::string_view name);

template <class T, class Func>
inline std::vector<T> mapVec(const std::vector<T>& v, Func f) {
    return v | std::views::transform(f) | std::ranges::to<std::vector>();
}