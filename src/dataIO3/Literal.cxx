#include "Literal.H"
#include <sstream>

Literal::Literal(Secret, Str&& name) : name(std::move(name)) {}

Str Literal::toStringImpl() const {
    std::stringstream s;
    _genType(s);
    return s.str();
}

void Literal::genTypeImpl(Output& o) const {
    _genType(o);
}

LiteralSP LiteralParser::parseImpl(CSIt& it, const CSIt& end) const {
    skipWs(it, end);
    assertOrNoTypeFound(this->first(it, end));
    Str::value_type quote(*it);
    ++it;
    assertOrNoTypeFound(it != end);
    Str val;

    while (*it != quote)
    {
        if (*it == '\\')
        {
            ++it;
            assertOrNoTypeFound(it != end);
            val += *it;
            ++it;
            assertOrNoTypeFound(it != end);
        }
        else
        {
            val += *it;
            ++it;
            assertOrNoTypeFound(it != end);
        }
    }
    ++it; // skip final quote

    return Literal::createSP(std::move(val));

}

bool LiteralParser::firstCharImpl(char c) const {
    return (c == '\'' || c == '"');
}
