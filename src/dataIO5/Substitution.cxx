#include "Substitution.H"
#include "TSReference.H"
#include "utils/utils.H"

#include <cassert>


Sp<const TypeExp> Substitution::apply(Sp<const TSReference> reference) const {
    auto found = var2exp.find(reference->name);
    if (found == var2exp.end()) {
        // not found
        return reference;
    } else {
        // found
        assert(reference->templateArgs.empty()); // Sonst Substitution undefiniert, da reference nicht auf eine freie Variable verweist, aber auf eine gleichnamige Template-Instanziierung!
        return found->second;
    }
}
