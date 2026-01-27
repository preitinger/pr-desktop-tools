#pragma once

#include "Attribute.H"
#include "TSTypeBase.H"
#include "utils.H"

#include <string>

CDECL(AttributeList);

using AttributeSPVec = std::vector<std::shared_ptr<Attribute>>;

class AttributeList : public TSTypeBase<AttributeList> {
public:
    AttributeSPVec attributes;
    AttributeList(Secret, AttributeSPVec&& attributes);

    Str toStringImpl() const;
    void genTypeImpl(Output& out) const;
};