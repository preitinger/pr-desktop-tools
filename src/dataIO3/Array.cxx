#include "Array.H"

#include "utils.H"
#include "AnyTSType_Impl.H"

#include <sstream>

Str Array::toStringImpl() const {
    std::stringstream s;
    Output out(s);
    genTypeImpl(out);

    return s.str();
}

void Array::genTypeImpl(Output& out) const {
    baseType->genType(out);
    out << "[]";
}