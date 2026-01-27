#pragma once

#define CPOINTERS(ClassName) \
    using ClassName##SP = std::shared_ptr<ClassName>; \
    using ClassName##CSP = std::shared_ptr<const ClassName>; \
    using ClassName##UP = std::unique_ptr<ClassName>; \
    using ClassName##CUP = std::unique_ptr<const ClassName>; \
    using ClassName##WP = std::weak_ptr<ClassName>; \
    using ClassName##CWP = std::weak_ptr<const ClassName>

#define CDECL(ClassName) \
    class ClassName; \
    CPOINTERS(ClassName)

#define CVEC(TypeName) \
    using TypeName##Vec = std::vector<TypeName>

#define CVECS(ClassName) \
    CVEC(ClassName); \
    CVEC(ClassName##SP); \
    CVEC(ClassName##CSP); \
    CVEC(ClassName##UP); \
    CVEC(ClassName##CUP); \
    CVEC(ClassName##WP); \
    CVEC(ClassName##CWP)

#define CDECLVECS(ClassName) \
    CDECL(ClassName); \
    CVECS(ClassName)

#define TDECLVECS(TemplateArgs, ClassName) \
template TemplateArgs \
class ClassName; \
template TemplateArgs using TypeName##Vec = std::vector<TypeName TemplateArgs >


#define CMAP(Prefix, Key, Value) \
    using Prefix##Map = std::map<Key, Value>

#define CMAPS(Prefix, Key, Value) \
    CMAP(Prefix, Key, Value); \
    CMAP(Prefix##C, Key, const Value); \
    CMAP(Prefix##SP, Key, Value##SP); \
    CMAP(Prefix##CSP, Key, Value##CSP); \
    CMAP(Prefix##UP, Key, Value##UP); \
    CMAP(Prefix##WP, Key, Value##WP); \
    CMAP(Prefix##CWP, Key, Value##CWP)

// erleichtert die Deklaration einer delegierenden Funktion in einer CRTP-Basisklasse:
#define DELEGATE_TO_DERIVED_CONST(func_name) \
    template<typename... Args> \
    auto func_name(Args&&... args) const { \
        return static_cast<const Derived*>(this)->func_name##Impl(std::forward<Args>(args)...); \
    }

#define DELEGATE_TO_DERIVED(func_name) \
    template<typename... Args> \
    auto func_name(Args&&... args) { \
        return static_cast<Derived*>(this)->func_name##Impl(std::forward<Args>(args)...); \
    }

// Helper für das Dispatching
template<class... Ts> struct overloads : Ts... { using Ts::operator()...; };


// erleichtert die Deklaration einer delegierenden Funktion in einer von std::variant abgeleiteten Klasse.
#define DISPATCH(variant_obj, func_name, ...) \
    return std::visit(overloads { \
        [](auto& node) { return node.func_name(__VA_ARGS__); } \
    }, variant_obj)


#define DISPATCH_CONST(variant_obj, func_name, ...) \
    return std::visit(overloads { \
        [](const auto& node) { return node.func_name(__VA_ARGS__); } \
    }, variant_obj)

#define DISPATCH_SP(variant_obj, func_name, ...) \
    return std::visit(overloads { \
        [](auto& node) { return node->func_name(__VA_ARGS__); } \
    }, variant_obj)

#define DISPATCH_CSP(variant_obj, func_name, ...) \
    return std::visit(overloads { \
        [](const auto& node) { return node->func_name(__VA_ARGS__); } \
    }, variant_obj)


#define DISPATCH_SP_METHOD(method) \
    template <class... Args> \
    auto method(Args&&... args) { \
        return std::visit(overloads { \
            [&](auto& node) { \
                return node->method(std::forward<Args>(args)...); \
            } \
        }, *this); \
    }

#define DISPATCH_CSP_METHOD(method) \
    template <class... Args> \
    auto method(Args&&... args) const { \
        return std::visit(overloads { \
            [&](const auto& node) { \
                return node->method(std::forward<Args>(args)...); \
            } \
        }, *this); \
    }
