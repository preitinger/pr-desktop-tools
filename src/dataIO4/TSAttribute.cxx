#include "TSAttribute.H"

#include "TypeExp.H"
#include "utils/ordering.H"
#include "utils/utils.H"
#include "utils/Output.H"


std::strong_ordering TSAttribute::operator<=>(const TSAttribute& other) const {
    std::tuple<const Str&/* , const Sp<const TypeExp>& */> test = std::tie(name/* , type */);
    std::tuple<const Str&/* , const Sp<const TypeExp >&*/> test2 = std::tie(other.name/* , other.type */);
    return compareTuples(test, test2);
    // return compareTuples<const Str&, Sp<const TypeExp>>(std::tie(name, type), std::tie(other.name, other.type));
}

void TSAttribute::gen(Output& out) const {
    out << name << ": ";
    type->genType(out);
}