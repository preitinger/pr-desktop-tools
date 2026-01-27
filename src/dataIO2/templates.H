#ifndef __TEMPLATES_H__
#define __TEMPLATES_H__
//##################################################################################################################################

#include "Parser.H"
#include "../dataIO-codegen/utils.H"


#include <string>
#include <variant>
#include <map>
#include <memory>
#include <fstream>
#include <ranges>
#include <iostream>

using Str = std::string;
using CSIt = Str::const_iterator;
CPOINTERS(Str);
CVECS(Str);

// Helper für das Dispatching
template<class... Ts> struct overloads : Ts... { using Ts::operator()...; };


class ValidType;

CDECLVECS(TypeDecl);
using TypeDeclMap = std::map<Str, TypeDecl>;

class Identifier
{
public:
    Str name;

    // Identifier() : name() {}
    // Identifier(Identifier&& other);
    // Identifier(const Identifier& other);

    bool operator<(const Identifier&) const;

    Identifier& operator=(Identifier&& other);

    friend Output& operator<<(Output&, const Identifier&);
};

#define BLA(Arg) void genWriteFunction(Arg) const { \
        static_cast<const Derived*>(this)->genWriteFunctionImpl(out); \
    }

// Die CRTP-Basisklasse
template <typename Derived>
class TypeBase {
public:
    // Gemeinsame Schnittstelle, die statisch auf die Subklasse delegiert

    DELEGATE_TO_DERIVED_CONST(genType);
    DELEGATE_TO_DERIVED_CONST(validate);
};

CDECL(Literal);
CDECL(Reference);
CDECL(AttributeList);
CDECL(Union);

CDECLVECS(Type);

struct Literal : public TypeBase<Literal> {
    Str val;
    Literal(Str&& val) : val(std::move(val)) {}

    void genTypeImpl(Output& out) const;
    ValidType validateImpl(const TypeDeclMap& allTypeDecls) const;
};

struct Reference : public TypeBase<Reference> {
    Str name;
    TypeVec templateArgs;
    // Reference(Str&& name, TypeVec&& templateArgs) : name(std::move(name)), templateArgs(std::move(templateArgs)) {}
    // void genWriteFunctionImpl(Output& out) const;
    // void genReadFunctionImpl(Output& out) const;
    void genTypeImpl(Output& out) const;
    ValidType validateImpl(const TypeDeclMap& allTypeDecls) const;
};

CDECLVECS(Attribute);



struct AttributeList : public TypeBase<AttributeList> {
    AttributeVec attributes;
    // AttributeList(AttributeVec&& attributes) : attributes(std::move(attributes)) {}
    // void genWriteFunctionImpl(Output& out) const;
    // void genReadFunctionImpl(Output& out) const;
    void genTypeImpl(Output& out) const;
    ValidType validateImpl(const TypeDeclMap& allTypeDecls) const;
};


struct Array : public TypeBase<Array> {
    TypeCSP baseType;
    void genTypeImpl(Output& out) const;
    ValidType validateImpl(const TypeDeclMap& allTypeDecls) const;
};

struct Union : public TypeBase<Union> {
    TypeCSPVec alternatives;
    void genTypeImpl(Output& out) const;
    ValidType validateImpl(const TypeDeclMap& allTypeDecls) const;
};

using TypeVariant = std::variant<Literal, Reference, AttributeList, Array, Union>;

class Type : public TypeVariant {
public:
    template <class T>
    Type(T&& t) : TypeVariant(std::move(t)) {}

    void genType(Output& out) const;
    // ValidType validate(const TypeDeclMap& allTypeDecls) const;

    // void genWriteFunction(Output& out) const;
    // void genReadFunction(Output& out) const;
};

class Attribute {
public:
    Str name;
    Type type;


    Str toString() const;
};

template <class It>
class IdentifierParser : public Parser<Type, IdentifierParser<It>, It> {
    public:
    Identifier parseImpl(It& it, const It& end) const;
    bool firstCharImpl(char c) const;
};

template <class It>
class TypeParser : public Parser<TypeCSP, TypeParser<It>, It> {
public:
    TypeCSP parseImpl(It& it, const It& end) const;
    bool firstCharImpl(char c) const;
};

class TypeDecl {
public:
    Str name;
    StrVec templateArgs;
    Type type;

    TypeDecl(Str&& name, StrVec&& templateArgs, Type&& type)
        : name(std::move(name)), templateArgs(std::move(templateArgs)), type(std::move(type)) {
    }

    // Str toString() const;

    void genFuncs(Output& out, const TypeDeclMap& allDecls) const;


};

template <class It>
class AttributeParser : public Parser<Attribute, AttributeParser<It>, It> {
public:
    Attribute parseImpl(It& it, const It& end) const;
    bool firstCharImpl(char c) const;
};

template <class It>
class LiteralParser : public Parser<LiteralCSP, LiteralParser<It>, It> {
public:
    LiteralCSP parseImpl(It& it, const It& end) const;
    bool firstCharImpl(char c) const;
};

template <class It>
class AttributeListParser : public Parser<AttributeListCSP, AttributeListParser<It>, It> {
public:
    AttributeListCSP parseImpl(It& it, const It& end) const;
    bool firstCharImpl(char c) const;
};

template <class It>
class TypeDeclParser : public Parser<TypeDecl, TypeDeclParser<It>, It> {
public:
    TypeDecl parseImpl(It& it, const It& end) const;
    bool firstCharImpl(char c) const;
};

template <class It>
class TemplateArgParser : public Parser<StrVec, TemplateArgParser<It>, It> {
public:
    StrVec parseImpl(It& it, const It& end) const;
    bool firstCharImpl(char c) const;
};

template <class It>
class TypeDeclMapParser : public Parser<TypeDeclMap, TypeDeclMapParser<It>, It> {
public:
    TypeDeclMap parseImpl(It& it, const It& end) const;
    bool firstCharImpl(char c) const;
};





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEMPLATE IMPLEMENTATIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifier::Identifier(Identifier&& other)
//     : name(std::move(other.name))
// {
// }

// Identifier::Identifier(const Identifier& other)
//     : name(other.name)
// {
// }

template <class It>
Identifier IdentifierParser<It>::parseImpl(It& it, const It& end) const {
    Str name;
    skipWs(it, end);
    char c;
    if (it != end && firstCharImpl((c = *it)))
    {
        name += c;

        while (++it != end && ((c = *it) >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_' || c >= '0' && c <= '9'))
        {
            name += c;
        }
    }

    return Identifier{name};
}
    
template <class It>
bool IdentifierParser<It>::firstCharImpl(char c) const {
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
}

template <class ParserIt>
bool TypeParser<ParserIt>::firstCharImpl(char c) const {
    return LiteralParser<ParserIt>().firstChar(c) || c == '{' || c == '\'' || c == '"';
}

// using ParserIt = CSIt; // TODO remove me
template <class ParserIt> // TODO add me
TypeCSP TypeParser<ParserIt>::parseImpl(ParserIt& it, const ParserIt& end) const {
    TypeCSP type;
    TypeCSPVec alternatives;
    LiteralParser<ParserIt> literalParser;
    AttributeListParser<ParserIt> attributeListParser;
    IdentifierParser<ParserIt> identifierParser;

    do {
        skipWs(it, end);
        // std::cerr << "Type::parse: ";
        // if (it == end) {
        //     std::cerr << "it==end";
        // }
        // else {
        //     std::cerr << *it;
        // }
        // std::cerr << std::endl;

        if (literalParser.first(it, end)) {
            type.reset(new Type(literalParser.parse(it, end)));
        }
        else if (attributeListParser.first(it, end)) {
            type.reset(new Type(attributeListParser.parse(it, end)));
        }
        else if (identifierParser.first(it, end)) {
            Identifier id = identifierParser.parse(it, end);
            TypeVec templateArgs;
            skipWs(it, end);
            if (it != end && *it == '<') {
                ++it;
                do {
                    templateArgs.push_back(this->parse(it, end));
                    skipWs(it, end);

                    if (it != end && *it == '>') {
                        ++it;
                        break;
                    }

                    skipKeyword(it, end, ",");
                } while (true);
            }
            type.reset(new Type(Reference{ {}, std::move(id.name), std::move(templateArgs) }));
        }

        skipWs(it, end);
        if (it != end && *it == '[') {
            ++it;
            skipKeyword(it, end, "]");
            type.reset(new Type(Array{ {}, type }));
        }

        skipWs(it, end);

        if (it != end && *it == '|') {
            ++it;
            alternatives.push_back(type);
            continue;
        }

        if (alternatives.empty()) {
            return type;
        }
        else {
            alternatives.push_back(type);
            return TypeCSP(new Type(Union{ {}, std::move(alternatives) }));
        }
    } while (true);

}

template <class ParserIt>
StrVec TemplateArgParser<ParserIt>::parseImpl(ParserIt& it, const ParserIt& end) const {
    skipWs(it, end);
    StrVec templateArgs;

    IdentifierParser<ParserIt> identifierParser;

    if (it != end && *it == '<') {
        skipKeyword(it, end, "<");
        Identifier templateArg = identifierParser.parse(it, end);
        templateArgs.push_back(templateArg.name);
        skipWs(it, end);
        while (it != end && *it == ',') {
            skipKeyword(it, end, ",");
            Identifier templateArg2 = identifierParser.parse(it, end);
            templateArgs.push_back(templateArg2.name);
        }
        skipKeyword(it, end, ">");
    }
    return templateArgs;

}

template <class ParserIt>
TypeDecl TypeDeclParser<ParserIt>::parseImpl(ParserIt& it, const ParserIt& end) const {
    skipWs(it, end);

    IdentifierParser<ParserIt> identifierParser;
    TemplateArgParser<ParserIt> templateArgParser;
    TypeParser<ParserIt> typeParser;
    AttributeListParser<ParserIt> attributeListParser;

    if (it != end && *it == 't') {
        skipKeyword(it, end, "type");
        Identifier name = identifierParser.parse(it, end);
        StrVec templateArgs = templateArgParser.parse(it, end);

        skipKeyword(it, end, "=");
        Type type = typeParser.parse(it, end);
        skipOptionalKeyword(it, end, ";");
        return TypeDecl(std::move(name.name), std::move(templateArgs), std::move(type));
    }
    else {
        skipKeyword(it, end, "interface");
        Identifier name = identifierParser.parse(it, end);
        StrVec templateArgs = templateArgParser.parse(it, end);

        TypeDecl res(std::move(name.name), std::move(templateArgs), attributeListParser.parse(it, end));
        skipOptionalKeyword(it, end, ";");
        return res;
    }

}

template <class ParserIt>
TypeDeclMap TypeDeclMapParser<ParserIt>::parseImpl(ParserIt& it, const ParserIt& end) const {
    TypeDeclMap m;

    try {
        TypeDeclParser<ParserIt> declParser;
        do {
            TypeDecl typeDecl = declParser.parse(it, end);
            m.emplace(typeDecl.name, std::move(typeDecl));
        } while (true);
    }
    catch (const NoTypeFound& ex) {
        ex.printStackTrace();
    }
    return m;
}

template <class ParserIt>
Attribute AttributeParser<ParserIt>::parseImpl(ParserIt& it, const ParserIt& end) const {
    Identifier id = IdentifierParser<ParserIt>().parse(it, end);
    skipKeyword(it, end, ":");
    return Attribute{ std::move(id.name), TypeParser<ParserIt>().parse(it, end) };
}

template <class ParserIt>
AttributeList AttributeListParser<ParserIt>::parseImpl(ParserIt& it, const ParserIt& end) const {
    AttributeVec attributes;
    skipWs(it, end);
    skipKeyword(it, end, "{");
    skipWs(it, end);

    AttributeParser<ParserIt> attributeParser;

    while (it != end && *it != '}') {
        attributes.push_back(attributeParser.parse(it, end));
        skipWs(it, end);
        if (it != end && (*it == ',' || *it == ';')) {
            ++it;
        }
        skipWs(it, end);
    }

    skipKeyword(it, end, "}");

    return AttributeList{ {}, std::move(attributes) };
}

// static
template <class ParserIt>
bool LiteralParser<ParserIt>::firstCharImpl(char c) const {
    return (c == '\'' || c == '"');
}

template <class ParserIt>
LiteralCSP LiteralParser<ParserIt>::parseImpl(ParserIt& it, const ParserIt& end) const {
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

    return Literal{ std::move(val) };
}

template <class ParserIt>
bool AttributeListParser<ParserIt>::firstCharImpl(char c) const {
    return c == '{';
}

/* using It = std::vector<TypeDecl>::const_iterator; // */ template <class It>
int countUnderScores(const It& begin, const It& end) {
    int max = 0;

    for (It it = begin; it != end; ++it) {
        const TypeDecl& typeDecl = *it;
        auto nameBegin = typeDecl.name.begin();
        auto nameIt = nameBegin;
        auto nameEnd = typeDecl.name.end();
        while (nameIt != nameEnd && *nameIt == '_') {
            ++nameIt;
        }
        int count = nameIt - nameBegin;
        if (count > max) max = count;
    }

    return max;
}

//##################################################################################################################################
#endif // __TEMPLATES_H__
