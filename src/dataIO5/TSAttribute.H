#pragma once

#include "utils/utils.H"
#include "utils/ordering.H"

class SRange;
class TypeExp;
class Output;

class TSAttribute {
public:
    Str name;
    Sp<const TypeExp> type;
    TSAttribute(Str name, Sp<const TypeExp> type) : name(std::move(name)), type(std::move(type)) {}

    std::strong_ordering operator<=>(const TSAttribute& other) const;

    void gen(Output&) const;
};

