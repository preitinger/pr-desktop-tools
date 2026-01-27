#pragma once

#include "macros.H"


#include <memory>
#include <utility>

template <class Derived>
class ObjectBase {
protected:
    class Secret {
        explicit Secret() = default;
        friend class ObjectBase<Derived>;
    };
    ~ObjectBase() = default;

public:
    DELEGATE_TO_DERIVED(increment);
    DELEGATE_TO_DERIVED_CONST(dump);

    template<typename... Args>
    static std::shared_ptr<Derived> createSP(Args&&... args) {
        // return std::shared_ptr<Derived>(new Derived(std::forward<Args>(args)...));
        return std::make_shared<Derived>(Secret{}, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static Derived create(Args&&... args) {
        // return std::shared_ptr<Derived>(new Derived(std::forward<Args>(args)...));
        return Derived(Secret{}, std::forward<Args>(args)...);
    }
};

