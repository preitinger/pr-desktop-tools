#pragma once

#include "TSTypeBase.H"
#include "utils.H"
#include "Parser.H"

CDECLVECS(Literal);


class Literal : public TSTypeBase<Literal> {
public:

    Str name;

    Literal(Secret, Str&& name);

    Str toStringImpl() const;
    void genTypeImpl(Output& out) const;

private:
    template <class Stream>
    void _genType(Stream& o) const;
};

class LiteralParser: public Parser<LiteralSP, LiteralParser, CSIt> {
public:
    LiteralSP parseImpl(CSIt& it, const CSIt& end) const;
    bool firstCharImpl(char c) const;
};

///////////////////////////////////////////////////////////////////////////////////////
// TEMPLATE IMPLEMENTATIONS
///////////////////////////////////////////////////////////////////////////////////////
template <class Stream>
void Literal::_genType(Stream& o) const {
    o << "'";
    for (char c : name) {
        switch (c) {
        case '\'':
            o << "\\'";
            break;
        case '\\':
            o << "\\\\";
            break;
        default:
            o << c;
        }
    }
    o << "'";
}
