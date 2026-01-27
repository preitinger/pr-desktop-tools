#pragma once

#include "macros.H"


#include <memory>
#include <utility>

template <class Derived>
class GenericBase {
protected:
    class Secret {
        explicit Secret() = default;
        friend class GenericBase<Derived>;
    };
    ~GenericBase() = default;

public:
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


template <class Derived>
class TSTypeBase : public GenericBase<Derived> {

public:

    // TODO add delegates for typescript types

    DELEGATE_TO_DERIVED_CONST(toString);
    DELEGATE_TO_DERIVED_CONST(genType);

    // DELEGATE_TO_DERIVED(increment);
    // DELEGATE_TO_DERIVED_CONST(dump);

};

