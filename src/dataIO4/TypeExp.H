#pragma once

#include "maps.H"
#include "utils/utils.H"

#include <memory>
#include <compare>

class Output;
class DeclCol;
class SRange;
class Substitution;

/**
 * Basisklasse für alle Typausdrücke in Typescript
 */
class TypeExp : public std::enable_shared_from_this<TypeExp>
{
private:

protected:
    virtual std::strong_ordering vcmp(const TypeExp& other) const = 0;

public:
    TypeExp() = default;
    virtual ~TypeExp() = default;

    virtual int rank() const = 0;

    virtual void genType(Output& output) const = 0;
    virtual int maxUnderscores() const = 0;
    virtual Sp<const TypeExp> subst(const DeclCol& decls) const = 0;
    virtual Sp<const TypeExp> abstr(DeclCol& decls) const = 0;
    virtual Sp<const TypeExp> substitute(const Substitution& s) const = 0;


    std::strong_ordering operator<=>(const TypeExp&) const;

    // static Sp<const TypeExp> parse(SRange& range);
};