#pragma once

#include "TypeExp.H"
#include "utils/SRange.H"

class Substitution;

class TSArray : public TypeExp
{
private:

protected:
    virtual std::strong_ordering vcmp(const TypeExp& other) const override;

public:
    Sp<const TypeExp> baseType;
    TSArray(Sp<const TypeExp> baseType) : baseType(std::move(baseType)) {}
    virtual ~TSArray() = default;

    virtual int rank() const override { return 4; };

    virtual void genType(Output& output) const override;
    virtual int maxUnderscores() const override;
    virtual Sp<const TypeExp> subst(const DeclCol& decls) const override;
    virtual Sp<const TypeExp> abstr(DeclCol& decls) const override;
    virtual Sp<const TypeExp> substitute(const Substitution& s) const override;

};
