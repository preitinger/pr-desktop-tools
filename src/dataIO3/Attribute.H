#pragma once

#include "TSTypeBase.H"
#include "macros.H"

CDECL(AnyTSType);
using Name = std::string;
CPOINTERS(Name);

CDECL(Attribute);

class Output;

struct Attribute : public GenericBase<Attribute> {
    NameCSP name;
    AnyTSTypeCSP attributeType;

    Attribute(Secret, NameCSP&& name, AnyTSTypeCSP&& attributeType) : name(std::move(name)), attributeType(std::move(attributeType)) {}

    void gen(Output& output) const;
};