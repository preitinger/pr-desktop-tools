#pragma once

#include "forwards.H"
#include "utils/utils.H"
#include "utils/ordering.H"

namespace logics
{

FORWARDS(Exp, C);
FORWARDS(VarExp, I);
FORWARDS(FuncExp, I);

class Substitution {
public:
    Substitution() = default;
    Substitution(const Substitution&) = delete;

    SpStrMap<Exp> var2exp;
    // TODO besser const IVarExp& var und dann nicht var.self() zurückgeben, wenn sich nichts geändert hat, sondern einen leeren shared_ptr
    Exp apply(VarExp var) const;
    /**
     * In allen bestehenden Eintraegen von var2exp [var -> t] anwenden und anschließend Eintrag
     * [var -> t] hinzufügen.
     * Voraussetzung ist, dass bisher noch kein Eintrag mit Schlüssel var in var2exp existiert.
     */
    void addSequentially(Sp<const Str> var, Exp t);

    Str toString() const;
};

/**
 * Bei der Konstruktion wird versucht eine Unifikation der zwei übergebenen Ausdrücke (vom Typ Exp, d.h. shared pointer auf CExp) zu berechnen.
 * Dies ist im Erfolgsfall eine Substitution. Falls keine Unifikation gefunden werden kann, ist das Ergebnis in diesem Objekt `empty`.
 */
class Unification {
public:
    /**
     * Berechnet die Unifikation zweier Terme, falls möglich.
     */
    Unification(const Exp& a, const Exp& b);
    bool empty() const { return !s; }
    const Sp<const Substitution>& substitution() const& { return s; }
    Sp<const Substitution> substitution()&& { return std::move(s); }

private:
    Sp<const Substitution> s;
};


} // namespace logics


