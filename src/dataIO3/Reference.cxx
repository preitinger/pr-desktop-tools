#include "Reference.H"
#include "utils.H"
#include "macros.H"
#include "AnyTSType.H"
#include "AnyTSType_Impl.H"

#include <sstream>

Reference::Reference(Secret, NameCSP&& name, AnyTSTypeSPVec&& templateArgs)
    : name(std::move(name)), templateArgs(std::move(templateArgs)) {

}

Reference::Reference(Secret, NameCSP&& name)
    : name(std::move(name)), templateArgs() {

}

void Reference::genTypeImpl(Output& out) const {
    out << *name;
    if (!templateArgs.empty()) {
        out << "<";
        bool first = true;
        for (const auto& arg : templateArgs) {
            if (first) {
                first = false;
            } else {
                out << ", ";
            }
            arg->genType(out);
        }
        out << ">";
    }
}



Str Reference::toStringImpl() const {
    std::stringstream s;
    Output out(s);
    genTypeImpl(out);
    return s.str();
}
