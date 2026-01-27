#pragma once

#include "TypeExp.H"

class Substitution;

/**
 * Basisklasse für alle Typausdrücke in Typescript
 */
class TSUnion : public TypeExp {
private:

protected:
    virtual std::strong_ordering vcmp(const TypeExp& other) const override;
    
public:
    Vec<Sp<const TypeExp>> alternatives;
    TSUnion(Vec<Sp<const TypeExp>> alternatives) : alternatives(std::move(alternatives)) {}

    virtual int rank() const override { return 3; }

    virtual void genType(Output& output) const override;
    virtual int maxUnderscores() const override;
    virtual Sp<const TypeExp> subst(const DeclCol& decls) const override;
    virtual Sp<const TypeExp> abstr(DeclCol& decls) const override;
    virtual Sp<const TypeExp> substitute(const Substitution& s) const override;

};