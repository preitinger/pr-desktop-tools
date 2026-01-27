#include "IntObject.H"

#include <iostream>

static std::ostream& out = std::cout;

void IntObject::incrementImpl() {
    ++val;
}

void IntObject::dumpImpl() const {
    out << "IntObject: " << val << "\n";
}
