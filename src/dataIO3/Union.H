#pragma once

#include "TSTypeBase.H"
#include "utils.H"
#include "AnyTSType.H"

CDECL(Union);

class Union : public TSTypeBase<Union> {
public:
    AnyTSTypeSPVec types;
    Union(Secret, AnyTSTypeSPVec&& types) : types(std::move(types)) {}

    Str toStringImpl() const;
    void genTypeImpl(Output& out) const;
};