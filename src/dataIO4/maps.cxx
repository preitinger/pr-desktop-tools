#include "maps.H"
#include "TypeExp.H"


bool CompareType::operator()(const Sp<const TypeExp>& a, const Sp<const TypeExp>& b) const {
    return liftStrongOrdering(a, b) < 0;
}
