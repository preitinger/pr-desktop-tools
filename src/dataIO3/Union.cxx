#include "Union.H"
#include "utils.H"

#include "AnyTSType_Impl.H"

#include <sstream>

Str Union::toStringImpl() const {
    std::stringstream s;
    Output out(s);
    genTypeImpl(out);

    return s.str();
}

void Union::genTypeImpl(Output& out) const {
    bool first = true;

    for (const auto& t : this->types) {
        if (first) {
            first = false;
        } else {
            out << " | ";
        }

        t->genType(out);
    }
}