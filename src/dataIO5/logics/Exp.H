#pragma once

#include "utils/utils.H"
#include "forwards.H"

#include <compare>
#include <memory>

#define WITH_OPTIMIZATION

namespace logics {

FORWARDS(Exp, C);
class Substitution;
FORWARDS(VarExp, I);
FORWARDS(FuncExp, I);

/**
 * Expression, i.e. a term
 */
class CExp : public std::enable_shared_from_this<CExp> {
public:
    CExp() = default;
    CExp(const CExp&) = delete;
    virtual ~CExp() = default;

    std::strong_ordering operator<=>(const CExp&) const;
    Exp self() const { return shared_from_this(); }

    virtual int rank() const = 0;
    virtual std::strong_ordering vcmp(const CExp&) const = 0;
    virtual Exp substitute(const Substitution&) const = 0;
    virtual bool unifyWith(const Exp& other, std::unique_ptr<Substitution>& s) const = 0;
#ifdef WITH_OPTIMIZATION
    virtual bool isSameVar(const IVarExp& other) const = 0;
#else
    virtual bool unifyWithVar(const IVarExp& other, std::unique_ptr<Substitution>& s) const = 0;
#endif
    virtual bool unifyWithFunc(const IFuncExp& other, std::unique_ptr<Substitution>& s) const = 0;
    virtual bool occursIn(const IVarExp& var) const = 0;
    virtual Str toString() const = 0;

};


} // namespace logics
