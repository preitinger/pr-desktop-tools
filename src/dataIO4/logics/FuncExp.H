#pragma once

#include "Exp.H"
#include "utils/utils.H"
#include "forwards.H"
#include "Func.H" // nur wegen inline implementierung von funcExp(SV name, ...)

namespace logics {

FORWARDS(Func, C);
FORWARDS(FuncExp, I);

class IFuncExp : public CExp {
public:
    IFuncExp() = default;
    virtual ~IFuncExp() = default;
    virtual const Func& getFunc() const = 0;
    virtual const Vec<Exp>& getArgs() const = 0;
    virtual FuncExp createClone(Vec<Exp> newArgs) const = 0;


    int rank() const override { return 0; }
    std::strong_ordering vcmp(const CExp&) const override;
    Exp substitute(const Substitution&) const override;
    bool unifyWith(const Exp& other, std::unique_ptr<Substitution>& s) const override;
    bool unifyWithVar(const IVarExp& other, std::unique_ptr<Substitution>& s) const override;
    bool unifyWithFunc(const IFuncExp& other, std::unique_ptr<Substitution>& s) const override;
    bool occursIn(const IVarExp& var) const override;
    Str toString() const override;
};

class CFuncExp : public IFuncExp {
public:
    Func func;
    Vec<Exp> args;
    CFuncExp(Func func, Vec<Exp> args);

    virtual const Func& getFunc() const override { return func; }
    virtual const Vec<Exp>& getArgs() const override { return args; }
    virtual FuncExp createClone(Vec<Exp> newArgs) const override { return ms<CFuncExp>(func, std::move(newArgs)); }

    // int rank() const override { return 0; }
    // std::strong_ordering vcmp(const CExp&) const override;
};

// template<class... Args>
// FuncExp funcExp(Args... args) {
//     return ms<const CFuncExp>(std::forward<Args>(args)...);
// }

inline FuncExp funcExp(SV name, std::initializer_list<Exp>&& args) {
    return ms<const CFuncExp>(func(std::move(name), args.size()), Vec<Exp>(std::move(args)));
}


} // namespace logics
