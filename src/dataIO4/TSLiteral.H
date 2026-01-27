#pragma once

#include "TypeExp.H"

class Substitution;

class TSLiteral : public TypeExp {

protected:
    virtual std::strong_ordering vcmp(const TypeExp& other) const override;
public:
    char quote;
    Str val;

    TSLiteral(char quote, Str val) : quote(quote), val(std::move(val)) {}

    int rank() const override { return 0; }
    void genType(Output& output) const override;
    int maxUnderscores() const override;
    Sp<const TypeExp> subst(const DeclCol& decls) const override;
    Sp<const TypeExp> abstr(DeclCol& names) const override;
    Sp<const TypeExp> substitute(const Substitution& s) const override;

    Sp<TSLiteral> self() { return std::static_pointer_cast<TSLiteral>(shared_from_this()); }
    Sp<const TSLiteral> self() const { return std::static_pointer_cast<const TSLiteral>(shared_from_this()); }
};