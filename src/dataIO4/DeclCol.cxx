#include "DeclCol.H"
#include "utils/utils.H"
#include "Decl.H"

#include <cassert>


DeclCol::DeclCol(/* args */)
{
}

DeclCol::~DeclCol()
{
}

AddResult DeclCol::add(Sp<const Decl> decl) {
    auto foundName = name2decl.find(decl->name);
    if (foundName != name2decl.end()) {
        return AddResult::NAME_NOT_UNIQUE;
    }
    
    auto foundType = type2decl.find(decl->exp);
    if (foundType != type2decl.end()) {
        return AddResult::TYPE_NOT_UNIQUE;
    }

    auto resName = name2decl.emplace(decl->name, decl);
    auto resType = type2decl.emplace(decl->exp, decl);
    assert(resName.second);
    assert(resType.second);
    return AddResult::ADDED;
    
}

Sp<const Decl> DeclCol::find(const Sp<const Str>& name) const {
    auto found = name2decl.find(name);
    if (found == name2decl.end()) return Sp<const Decl>();
    return found->second;
}
    
Sp<const Decl> DeclCol::find(const Sp<const TypeExp>& exp) const {
    auto found = type2decl.find(exp);
    if (found == type2decl.end()) return Sp<const Decl>();
    return found->second;

}
