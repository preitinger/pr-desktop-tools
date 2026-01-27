#pragma once

#include "maps.H"
#include "utils/utils.H"

#include <map>

class Decl;

enum class AddResult {
    ADDED,
    NAME_NOT_UNIQUE,
    TYPE_NOT_UNIQUE
};

class DeclCol
{
private:
    Name2Decl name2decl;
    Type2Decl type2decl;
public:
    DeclCol(/* args */);
    ~DeclCol();

    AddResult add(Sp<const Decl> decl);
    Sp<const Decl> find(const Sp<const Str>& name) const;
    Sp<const Decl> find(const Sp<const TypeExp>& exp) const;
};
