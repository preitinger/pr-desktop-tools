#pragma once

#include "utils.H"

#include <compare>
#include <ranges>
#include <map>

template <class T, class Cmp>
std::strong_ordering liftCompare(const Sp<const T>& a, const Sp<const T>& b, Cmp cmp) {
    if (!a) {
        return b ? std::strong_ordering::less : std::strong_ordering::equal;
    }
    else {
        return !b ? std::strong_ordering::greater : cmp(*a, *b);
    }
}

// Fall b
template <class T, class U>
std::strong_ordering liftStrongOrdering(const Sp<T>& a, const Sp<U>& b) {
    if (!a) {
        return b ? std::strong_ordering::less : std::strong_ordering::equal;
    }
    else {
        if (a.get() == b.get()) return std::strong_ordering::equal;
        return !b ? std::strong_ordering::greater : *a <=> *b;
    }
}

// Fall c
template <class T, class U>
std::strong_ordering liftStrongOrdering(const Vec<Sp<T>>& a, const Vec<Sp<U>>& b) {
    auto n = a.size();
    if (auto cmp = n <=> b.size(); cmp != 0) {
        return cmp;
    }

    for (const auto& [xa, xb] : std::views::zip(a, b)) {
        if (auto cmp = liftStrongOrdering(xa, xb); cmp != 0) {
            return cmp;
        }
    }

    return std::strong_ordering::equal;
}

// Fall a
// template <typename T>
// std::strong_ordering liftStrongOrdering(const T& a, const T& b) {
//     return a <=> b;
// }
template<typename T>
concept IsLifted = (requires {
    typename T::element_type; // Erkennt SmartPointer (Sp)
} || requires {
    typename T::value_type;
    typename T::iterator;
}) && !requires(T s) {
    s.substr(0); // std::string hat substr(), std::vector nicht
};

// template<typename T>
// concept IsLifted = requires {
//     typename T::element_type; // Erkennt Sp (shared_ptr)
// } || requires {
//     typename T::value_type;   // Erkennt Vec (std::vector)
//     typename T::iterator;
// };
// 2. Fall a: Nur für Typen, die NICHT in b oder c fallen
template <typename T>
    requires (!IsLifted<T>)
std::strong_ordering liftStrongOrdering(const T& a, const T& b) {
    return a <=> b;
}


// template <typename T>
//     requires (!requires(T t) { t->operator*(); }) // Verhindert Pointer-ähnliche Typen
// std::strong_ordering liftStrongOrdering(const T& a, const T& b) {
//     return a <=> b;
// }

template <typename... Ts>
std::strong_ordering compareTuples(const std::tuple<Ts...>& t1, const std::tuple<Ts...>& t2) {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        std::strong_ordering result = std::strong_ordering::equal;
        
        // Fold expression: Vergleiche bis result != 0
        ((result = liftStrongOrdering(std::get<Is>(t1), std::get<Is>(t2)), result != 0) || ...);
        
        return result;
    }(std::index_sequence_for<Ts...>{});
}


struct CompareStr {
    bool operator()(const Sp<const Str>& a, const Sp<const Str>& b) const {
        // return (liftCompare(a, b, [](const Str& a, const Str& b) {
        //     return a <=> b;
        // })) < 0;
        // // if (auto cmp = !!a <=> !!b; cmp != 0) {
        // //     return cmp < 0;
        // // }
        // // return *a < *b;

        // return compareTuples(std::tie(a), std::tie(b)) < 0;
        return liftStrongOrdering<const Str, const Str>(a, b) < 0;
    }
};

template <class T>
using SpStrMap = std::map<Sp<const Str>, T, CompareStr>;
