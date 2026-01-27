#include "FloatObject.H"

#include <iostream>

static std::ostream& out = std::cout;

// static 
// FloatObjectSP FloatObject::create(float val) {
//     return FloatObjectSP(new FloatObject{{}, val});
// }

void FloatObject::incrementImpl() {
    val += 1.3;
}

void FloatObject::dumpImpl() const {
    out << "FloatObject: " << val << "\n";
}
