#include "Attribute.H"
#include "utils.H"
#include "AnyTSType.H"
#include "AnyTSType_Impl.H"

void Attribute::gen(Output& out) const {
    out << *name << ": ";
    this->attributeType->genType(out);
}
