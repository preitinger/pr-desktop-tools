#pragma once

#include "TypeExp.H"
#include "utils/utils.H"

class SRange;
class Substitution;


class TSReference : public TypeExp
{

// protected:
public:
    virtual std::strong_ordering vcmp(const TypeExp& other) const override;

    public:
    Sp<const Str> name;
    Vec<Sp<const TypeExp>> templateArgs;
    TSReference(Sp<const Str> name, Vec<Sp<const TypeExp>> templateArgs) : name(std::move(name)), templateArgs(std::move(templateArgs)) {}

    virtual int rank() const override { return 1; };

    virtual void genType(Output& output) const override;
    virtual int maxUnderscores() const override;
    virtual Sp<const TypeExp> subst(const DeclCol& decls) const override;
    virtual Sp<const TypeExp> abstr(DeclCol& decls) const override;
    virtual Sp<const TypeExp> substitute(const Substitution& s) const override;

    Sp<TSReference> self() { return std::static_pointer_cast<TSReference>(shared_from_this()); }
    Sp<const TSReference> self() const { return std::static_pointer_cast<const TSReference>(shared_from_this()); }
};