#pragma once

// Nur forward Deklarationen da sonst include zyklus:
// #include "Literal.H"
// #include "Reference.H"
// #include "AttributeList.H"
// #include "Union.H"
// #include "Array.H"


#include <variant>
#include <memory>

class Literal;
class Reference;
class AttributeList;
class Union;
class Array;

template<class T>
using Sp = std::shared_ptr<T>;

using TSTypeSPVariants = std::variant<
    Sp<Literal>
    , Sp<Reference>
    , Sp<AttributeList>
    , Sp<Union>
    , Sp<Array>
>;
