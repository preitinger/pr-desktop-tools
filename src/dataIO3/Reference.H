#pragma once

#include "Name.H"
#include "TSTypeBase.H"
// #include "AnyTSType.H"
#include "utils.H"
#include "Parser.H"

CDECLVECS(Reference);
CDECLVECS(AnyTSType);

class Reference : public TSTypeBase<Reference> {
    public:

    NameCSP name;
    AnyTSTypeSPVec templateArgs;

    Reference(Secret, NameCSP&& name, AnyTSTypeSPVec&& templateArgs);
    Reference(Secret, NameCSP&& name);

    Str toStringImpl() const;
    void genTypeImpl(Output& out) const;
};

class ReferenceParser: public Parser<ReferenceSP, ReferenceParser, CSIt> {
public:
    ReferenceSP parseImpl(CSIt& it, const CSIt& end) const;
    bool firstCharImpl(char c) const;
};


// ///////////////////////////////////////////////////////////////////////////////////////
// // TEMPLATE IMPLEMENTATIONS
// ///////////////////////////////////////////////////////////////////////////////////////
// template <class Stream>
// void Reference::_genType(Stream& s) const {
//     s << *name;
//     if (!templateArgs.empty()) {
//         s << "<";
//         bool first = true;
//         for (const auto& arg : templateArgs) {
//             arg.genType(s);
//         }
//         s << ">";
//     }
//     s << "</*TODO*/>";
// }
