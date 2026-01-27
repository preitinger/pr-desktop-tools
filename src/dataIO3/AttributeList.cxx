#include "AttributeList.H"
#include "utils.H"
#include "Attribute.H"
#include "AnyTSType_Impl.H"

#include <sstream>

AttributeList::AttributeList(Secret, AttributeSPVec&& attributes)
: attributes(std::move(attributes)) {}
    
Str AttributeList::toStringImpl() const {
    std::stringstream s;
    Output out(s);
    genTypeImpl(out);
    return s.str();
}

void AttributeList::genTypeImpl(Output& out) const {
    out << "{ ";

    for (const AttributeSP& attribute : attributes) {
        attribute->gen(out);
        out << "; ";
    }

    out << "}";
}
