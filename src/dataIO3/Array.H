#pragma once

#include "TSTypeBase.H"
#include "utils.H"
#include "AnyTSType.H"
#include "macros.H"

CDECL(Array);

class Array : public TSTypeBase<Array> {
    public:
    AnyTSTypeSP baseType;
    Array(Secret, AnyTSTypeSP&& baseType) : baseType(std::move(baseType)) {}

    Str toStringImpl() const;
    void genTypeImpl(Output& out) const;
};