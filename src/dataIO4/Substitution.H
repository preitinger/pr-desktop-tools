#pragma once

#include "utils/utils.H"
#include "maps.H"

class TypeExp;
class TSReference;

class Substitution {
public:
    SpStrMap<Sp<const TypeExp>> var2exp;
    Sp<const TypeExp> apply(Sp<const TSReference> reference) const;
};

