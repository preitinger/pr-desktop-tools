#pragma once

#include "utils/ordering.H"
#include "utils/utils.H"

#include <map>
#include <string>

// // nur fuer test:
// #include <compare>
// #include <functional>
// #include <ranges>

class TypeExp;
class Decl;

// template <class T, class Cmp>
// std::strong_ordering liftCompare(const Sp<const T>& a, const Sp<const T>& b, Cmp cmp) {
//     if (!a) {
//         return b ? std::strong_ordering::less : std::strong_ordering::equal;
//     }
//     else {
//         return !b ? std::strong_ordering::greater : cmp(*a, *b);
//     }
// }

// template <class T>
// std::strong_ordering liftStrongOrdering(const Sp<const T>& a, const Sp<const T>& b) {
//     if (!a) {
//         return b ? std::strong_ordering::less : std::strong_ordering::equal;
//     }
//     else {
//         return !b ? std::strong_ordering::greater : *a <=> *b;
//     }
// }

// template <class T>
// std::strong_ordering liftStrongOrdering(const Vec<Sp<const T>>& a, const Vec<Sp<const T>>& b) {
//     auto n = a.size();
//     if (auto cmp = n <=> b.size(); cmp != 0) {
//         return cmp;
//     }

//     for (auto [xa, xb] : std::views::zip(a, b)) {
//         if (auto cmp = liftStrongOrdering(xa, xb); cmp != 0) {
//             return cmp;
//         }
//     }

//     return std::strong_ordering::equal;
// }
// template <typename... Ts>
// std::strong_ordering compareTuples(const std::tuple<Ts...>& t1, const std::tuple<Ts...>& t2) {
//     return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
//         std::strong_ordering result = std::strong_ordering::equal;
        
//         // Fold expression: Vergleiche bis result != 0
//         ((result = liftStrongOrdering(std::get<Is>(t1), std::get<Is>(t2)), result != 0) || ...);
        
//         return result;
//     }(std::index_sequence_for<Ts...>{});
// }

struct CompareType {
    bool operator()(const Sp<const TypeExp>& a, const Sp<const TypeExp>& b) const;
};

using Name2Decl = SpStrMap<Sp<const Decl>>; // std::map<Sp<const Str>, Sp<const Decl>, CompareStr>;
using Type2Decl = std::map<Sp<const TypeExp>, Sp<const Decl>, CompareType>;
