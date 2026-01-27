#pragma once

#include "TypeExp.H"

class SRange;
class TSAttribute;
class Substitution;

class TSAttributeList : public TypeExp {
protected:
    virtual std::strong_ordering vcmp(const TypeExp& other) const override;

public:
    Vec<Sp<const TSAttribute>> attributes;
    TSAttributeList(Vec<Sp<const TSAttribute>> attributes) : attributes(std::move(attributes)) {}
    // TSAttributeList(SRange& r);

    virtual int rank() const override { return 2; };

    virtual void genType(Output& output) const override;
    virtual int maxUnderscores() const override;
    virtual Sp<const TypeExp> subst(const DeclCol& decls) const override;
    virtual Sp<const TypeExp> abstr(DeclCol& decls) const override;
    virtual Sp<const TypeExp> substitute(const Substitution& s) const override;
};