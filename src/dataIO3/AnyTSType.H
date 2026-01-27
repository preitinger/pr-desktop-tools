#pragma once

#include "TSTypeVariants.H"
#include "macros.H"
#include "utils.H"

#include <utility>
#include <vector>
#include <string>

CDECLVECS(AnyTSType);

#define MANUAL_IMPLEMENTATION

class AnyTSType : public TSTypeSPVariants {
public:
    // Importiert automatisch alle Konstruktoren von std::variant
    // Dadurch kannst du AnyObject mit jedem shared_ptr initialisieren, der im Variant gelistet ist.
    using TSTypeSPVariants::variant;

    AnyTSType* operator->() { return this; }
    const AnyTSType* operator->() const { return this; }

#ifndef MANUAL_IMPLEMENTATION

    DISPATCH_CSP_METHOD_DECL(toString);
    DISPATCH_CSP_METHOD_DECL(genType);

    // template <class... Args>
    // auto genType(Args&&... args) const;
#else
    Str toString() const;
    void genType(Output& output) const;
#endif

};
