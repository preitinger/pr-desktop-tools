#pragma once

#include "ObjectVariants.H"

class AnyObjectSP : public ObjectVariantsSP {
public:
    // Importiert automatisch alle Konstruktoren von std::variant
    // Dadurch kannst du AnyObject mit jedem shared_ptr initialisieren, der im Variant gelistet ist.
    using ObjectVariantsSP::variant;


    AnyObjectSP* operator->() { return this; }
    const AnyObjectSP* operator->() const { return this; }
    
    // void increment() {
    //     std::visit([](auto& obj) { obj->increment(); }, *this);
    // }

    DISPATCH_SP_METHOD(increment);
    DISPATCH_CSP_METHOD(dump);

    // void dump() const {
    //     DISPATCH_CSP(*this, dump);
    //     // return std::visit([](const auto& obj) { return obj->dump(); }, *this);
    // }
};
