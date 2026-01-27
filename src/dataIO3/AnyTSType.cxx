#include "AnyTSType.H"
// #include "AnyTSType_Impl.H"
#include "Literal.H"
#include "Reference.H"
#include "AttributeList.H"
#include "Union.H"
#include "Array.H"
#include "TSTypeVariants.H"
#include "utils.H"

#include <utility>


#ifdef MANUAL_IMPLEMENTATION

std::string AnyTSType::toString() const {
    return std::visit([](const auto& obj) { return obj->toString(); }, *this);
}

void AnyTSType::genType(Output& output) const {
    std::visit([&](const auto& obj)
        {
            obj->genType(output);
        },
        *this);
}

#endif