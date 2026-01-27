#pragma once

#include "utils/utils.H"

class SRange;
class TypeExp;

class Output;

class Decl {
public:
    Sp<const Str> name;
    Vec<Sp<const Str>> templateArgs;
    Sp<const TypeExp> exp;

public:
    // Decl(SRange& r);
    Decl(Sp<const Str> name, Vec<Sp<const Str>> templateArgs, Sp<const TypeExp> exp)
        : name(std::move(name)), templateArgs(std::move((templateArgs))), exp(std::move(exp)) {
    }


    void genFuncs(Output& out) const;

private:

};
