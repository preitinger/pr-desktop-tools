#include "parse.H"
#include "TSAttributeList.H"
#include "TSAttribute.H"
#include "utils/utils.H"
#include "utils/SRange.H"
#include "Decl.H"
#include "TSLiteral.H"
#include "TSReference.H"
#include "TSArray.H"
#include "TSUnion.H"

#include <cassert>

/*

TypeDecl ::= Interface | TypeEq
Interface ::= 'interface' <Name> <OptTemplateVarList> AttributeList
OptTemplateVarList ::= | '<' NameList '>'
NameList ::= <Name> | NameList ',' <Name>
TypeEq ::= 'type' <Name> <OptTemplateVarList> '=' TypeDef
TypeDef ::= TypeDef '|' OptArray | OptArray
OptArray ::= ArrayBase OptArrayOp // OK
OptArrayOp ::= | '[' ']' // OK
ArrayBase ::= '(' TypeDef ')' | <Literal> | Reference | AttributeList  // OK
// ArrayBase1 ::= TypeDef ')'
Reference ::= <Name> OptTemplateArgs
OptTemplateArgs ::= | '<' TemplateArgList '>'
TemplateArgList ::= TypeDef | TemplateArgList ',' TypeDef
AttributeList ::= '{' AttributeList1 '}'
AttributeList1 ::= | AttributeList1 Attribute OptSemicolonOrComma
Attribute ::= <Name> ':' TypeDef
OptSemicolonOrComma ::= | ';' | ','

First(TypeDecl) = {'interface', 'type'}
First(Interface) = {'interface'}
First(TypeEq) = {'type'}
First(TypeDef) = {'('), <Literal>, <Name>, '{'}
First(Union) = {'('), <Literal>, <Name>, '{'}
First(OptArray) = {'('), <Literal>, <Name>, '{'}
First(OptArrayOp) = {epsilon, '['}
First(ArrayBase) = {'('), <Literal>, <Name>, '{'}
First(Reference) = {<Name>}
First(OptTemplateArgs) = {epsilon, '<'}
First(TemplateArgList) = {'('), <Literal>, <Name>, '{'}
First(AttributeList) = {'{'}
First(AttributeList1) = {epsilon, <Name>}
First(Attribute) = {<Name>}
First(OptSemicolonOrComma) = {epsilon, ';', ','}

Follow(Attribute) = {';', ',', <Name>, '}'}
Follow(AttributeList1) = {'}'}
Follow(TypeDecl) = { $ }
Follow(TypeEq) =  { $ }
Follow(Union) = {$, ')', ';', ',', <Name>, '}', '>'}
Follow(TemplateArgList) = { '>' }
Follow(TypeDef) = {$, ')', ';', ',', <Name>, '}', '>'}

 */

 // BEGIN stur nach Grammatik

static SRange::CharRanges nameInnerRanges{ {'a', 'z'}, {'A', 'Z'}, {'0', '9'}, {'_'} };

Str parseName(SRange& r) {
    r.skipWs();

    Str s;
    char c = *r;
    if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_') {
        s += r.skipChar1();
        s += r.skipAll(nameInnerRanges);
        return s;
    }
    else {
        throw ParseException();
    }
}

// Vec<Sp<const Str>> continueNameList(SRange& r, Vec<Sp<const Str>>&& nameList) {
//     r.skipKeyword(",");
//     nameList.push_back(ms<const Str>(parseName(r)));
//     return nameList;
//     // or better? return std::move(nameList);
// }

std::vector<Sp<const Str>> continueNameList(SRange& r, std::vector<Sp<const Str>>&& nameList) {
    r.skipKeyword(",");
    nameList.push_back(ms<const Str>(parseName(r)));
    r.skipWs();
    if (*r == ',') {
        return continueNameList(r, std::move(nameList));
    }
    else {
        return nameList;
    }
    return nameList; // Hier waere std::move gleich gut, da nameList ein Parameter und keine lokale Variable ist!
}

Vec<Sp<const Str>> parseNameList(SRange& r) {
    Vec<Sp<const Str>> nameList;
    nameList.push_back(ms<const Str>(parseName(r)));
    r.skipWs();
    if (*r == ',') {
        return continueNameList(r, std::move(nameList));
    }
    else {
        return nameList;
    }
}

Vec<Sp<const Str>> parseOptTemplateVarList(SRange& r) {
    r.skipWs();
    char c = *r;
    if (c == '<') {
        r.skipKeyword("<");
        auto nameList = parseNameList(r);
        r.skipKeyword(">");
        return nameList;
    }

    return Vec<Sp<const Str>>();
}

Sp<const TypeExp> parseTypeDef(SRange& r);

Sp<const TSAttribute> parseAttribute(SRange& r) {
    Str name = parseName(r);
    r.skipKeyword(":");
    auto type = parseTypeDef(r);
    return ms<const TSAttribute>(std::move(name), std::move(type));
}

void parseOptSemicolonOrComma(SRange& r) {
    r.skipWs();
    switch (*r) {
    case ',':
        // fall through
    case ';':
        r.skipChar1();
        r.skipWs();
        break;
    }
}

Sp<const TSAttributeList> parseAttributeList1(SRange& r) {
    r.skipWs();
    Vec<Sp<const TSAttribute>> attributes;

    while (*r != '}') {
        if (*r == 0) {
            throw ParseException();
        }
        attributes.push_back(parseAttribute(r));
        parseOptSemicolonOrComma(r);
        r.skipWs();
    }
    return ms<TSAttributeList>(std::move(attributes));
}

Sp<const TSAttributeList> parseAttributeList(SRange& r) {
    r.skipKeyword("{");
    Sp<const TSAttributeList> attributeList = parseAttributeList1(r);
    r.skipKeyword("}");
    return attributeList;
}

Sp<const Decl> parseInterface(SRange& r) {
    r.skipKeyword("interface");
    auto name = ms<const Str>(parseName(r));
    auto templateArgs = parseOptTemplateVarList(r);
    auto attributeList = parseAttributeList(r);
    // Scheiße, die Werte für die Funktionsargumente können vom Compiler in beliebiger Reihenfolge
    // berechnet werden! Daher zwingend vorab in Variablen ablegen, die dann gemoved werden müssen!
    return ms<const Decl>(std::move(name), std::move(templateArgs), std::move(attributeList));
}

Sp<const TSLiteral> parseLiteral(SRange& r) {
    r.skipWs();
    char quote = *r;
    if (quote == 0) {
        throw ParseException();
    }
    r.skipChar(quote);
    Str val;
    while (*r && *r != quote) {
        switch (*r) {
        case '\\':
            r.skipChar1();
            if (*r == 0) {
                throw ParseException();
            }
            // val += '\\'; // NEIN, denn sonst waere val unterschiedlich falls einmal bei quote='"' was anderes escaped waere als bei quote='\''
            val += *r;
            r.skipChar1();
            break;
        default:
            val += *r;
            r.skipChar1();
            break;
        }
    }

    if (*r != quote) {
        throw ParseException();
    }
    r.skipChar(quote);
    return ms<TSLiteral>(quote, std::move(val));
}

Vec<Sp<const TypeExp>> parseTemplateArgList(SRange& r) {
    Vec<Sp<const TypeExp>> templateArgs;
    templateArgs.push_back(parseTypeDef(r));
    r.skipWs();
    while (*r == ',') {
        r.skipChar1();
        templateArgs.push_back(parseTypeDef(r));
        r.skipWs();
    }
    return templateArgs;
}

Vec<Sp<const TypeExp>> parseOptTemplateArgs(SRange& r) {
    r.skipWs();
    Vec<Sp<const TypeExp>> templateArgs;

    if (*r == '<') {
        r.skipChar1();
        templateArgs = parseTemplateArgList(r);
        r.skipKeyword(">");
    }

    return templateArgs;
}

Sp<const TSReference> parseReference(SRange& r) {
    Sp<const Str> name = ms<const Str>(parseName(r));
    // Scheiße, kein Verlass auf Reihenfolge der Auswertung von Funktionsargumenten! ;-)
    return ms<TSReference>(std::move(name), parseOptTemplateArgs(r));
}

Sp<const TypeExp> parseArrayBase(SRange& r) {
    r.skipWs();

    switch (*r) {
    case '(': {
        auto typeDef = parseTypeDef(r);
        r.skipKeyword(")");
        return typeDef;
        break;
    }
    case '\'':
        return parseLiteral(r);
    case '{':
        return parseAttributeList(r);

    default:
        return parseReference(r);
    }
}

bool parseOptArrayOp(SRange& r) {
    r.skipWs();
    if (*r == '[') {
        r.skipKeyword("]");
        return true;
    }

    return false;
}

Sp<const TypeExp> parseOptArray(SRange& r) {
    Sp<const TypeExp> baseType = parseArrayBase(r);
    bool isArray = parseOptArrayOp(r);
    if (!isArray) return baseType;
    return ms<TSArray>(std::move(baseType));
}

Sp<const TypeExp> parseTypeDef(SRange& r) {
    Sp<const TypeExp> typeDef = parseOptArray(r);
    r.skipWs();

    if (*r != '|') {
        return typeDef;
    }

    Vec<Sp<const TypeExp>> alternatives;
    alternatives.push_back(std::move(typeDef));

    while (*r == '|') {
        r.skipChar1();
        alternatives.push_back(parseOptArray(r));
        r.skipWs();
    }
    return ms<const TSUnion>(std::move(alternatives));
}

Sp<const Decl> parseTypeEq(SRange& r) {
    r.skipKeyword("type");
    auto name = ms<Str>(parseName(r));
    auto templateArgs = parseOptTemplateVarList(r);
    r.skipKeyword("=");
    // Kein Verlass auf Reihenfolge bei Auswertung von Funktionsargumenten, daher nur das letzte `inline`!
    return ms<Decl>(std::move(name), std::move(templateArgs), parseTypeDef(r));
}

Sp<const Decl> parseTypeDecl(SRange& r) {
    r.skipWs();
    while (*r == ';') {
        r.skipKeyword(";");
        r.skipWs();
    }
    if (*r == 0) return Sp<const Decl>();

    // Sonst versuchen wir einen korrekten Typ zu finden und werden eine Exception werfen, wenn dem nicht so ist:

    char c = *r;
    switch (c) {
    case 'i':
        return parseInterface(r);
    case 't':
        return parseTypeEq(r);
    default:
        throw ParseException();
    }
}

// END stur nach Grammatik

// Sp<const TSAttribute> parseAttribute(SRange& r, char first) {

// }

// Sp<const TSAttributeList> parseAttributeList(SRange& r /* first is here always*/) {
//     // Beginning '{' already skipped.

//     r.skipWs();
//     Vec<Sp<const TSAttribute>> attributes;
//     char c = r.skipChar1();

//     while (c && c != '}') {
//         attributes.push_back(parseAttribute(r, c));
//         r.skipOptionalKeyword(";");
//         r.skipOptionalKeyword(",");
//     }
// }

// Sp<const TypeExp> parseType(SRange& r, char c) {

//     switch (c) {
//     case '{': {
//         Sp<const TSAttributeList> attributeList = parseAttributeList(r);
//         break;
//     }
//     } // switch

//     if (r.inAnyRange({ {'{'} })) {
//         r.skipChar('{');
//         auto attributeList = parseAttributeList(r);
//     }
// }

// static SRange::CharRanges nameFirstRanges{ {'a', 'z'}, {'A', 'Z'}, {'_'} };
// static SRange::CharRanges nameInnerRanges{ {'a', 'z'}, {'A', 'Z'}, {'0', '9'}, {'_'} };

// Str parseName(SRange& r, char first) {
//     Str s;
//     s += first;
//     s += r.skipAll(nameInnerRanges);
//     return s;
// }

// bool firstOfName(SRange& r) {
//     return r.inAnyRange(nameFirstRanges);
// }

// Sp<const Decl> parseDecl(SRange& r, char c) {
//     switch (c) {
//     case 't':
//         r.skipKeyword("ype");
//         r.skipWs();
//         c = r.skipChar1();
//         if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_') {
//             Str name = parseName(r, c);
//             r.skipKeyword("=");
//             r.skipWs();
//             c = r.skipChar1();
//             Sp<const TypeExp> exp = parseType(r, c);
//         }
//         break;
//     case 'i':
//         r.skipKeyword("nterface");
//         break;
//     }
// }