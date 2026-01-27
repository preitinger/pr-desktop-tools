#pragma once

#include "Exp.H"
#include "utils/utils.H"

namespace logics {

// class TestForwarding {
// public:
//     SV name;
//     // TestForwarding(SV&& name);
//     // TestForwarding(const SV& name);
//     TestForwarding(SV name);
//     TestForwarding(const TestForwarding&);
//     TestForwarding(TestForwarding&&);

//     auto operator<=>(const TestForwarding&) const = default;
// };

class IVarExp : public CExp {
public:
    IVarExp() = default;
    virtual ~IVarExp() = default;
    virtual const Sp<const Str>& getName() const = 0;

    VarExp self() const { return static_pointer_cast<const IVarExp>(CExp::self()); }

    int rank() const override { return 1; }
    std::strong_ordering vcmp(const CExp&) const override;
    virtual Exp substitute(const Substitution&) const override;
    bool unifyWith(const Exp& other, std::unique_ptr<Substitution>& s) const override;
#ifdef WITH_OPTIMIZATION
    bool isSameVar(const IVarExp& other) const override { return vcmp(other) == 0; };
#else
    bool unifyWithVar(const IVarExp& other, std::unique_ptr<Substitution>& s) const override;
#endif
    bool unifyWithFunc(const IFuncExp& other, std::unique_ptr<Substitution>& s) const override;
    bool occursIn(const IVarExp& var) const override;
    Str toString() const override;

    void dump() const;
};

class CVarExp : public IVarExp {
public:
    // TestForwarding name;
    int huge[10000];
    Sp<const Str> name;
    // CVarExp(TestForwarding name) : name(std::move(name)) {}
    CVarExp(Sp<const Str> name) : name(std::move(name)) {}
    const Sp<const Str>& getName() const { return name; }
};

using VarExp = Sp<const IVarExp>;

// VarExp varExp(SV name);

inline VarExp varExp(SV name) {
    return ms<CVarExp>(ms<const Str>(std::move(name)));
}

// template<class... Args>
// VarExp varExp(Args... args) {
//     return ms<CVarExp>(std::forward<Args>(args)...);
// }

} // namespace logics

