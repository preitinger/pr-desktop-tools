#pragma once

#include "utils/utils.H"
#include "utils/StackTraceException.H"

class SRange;
class TypeExp;
class Decl;

class ParseException : public StackTraceException {
public:
};

// Sp<const TypeExp> parseType(SRange& r);

// Str parseName(SRange& r, char first);
// bool firstOfName(SRange& r);


/**
 * Wenn r nur noch max. Whitespace enthält, wird ein leerer Smartpointer zurückgegeben.
 * Wenn ein korrekter Typ geparst wurde, wird natürlich dieser zurückgegeben.
 * Wenn mehr als Whitespace in r enthalten ist, aber dann kein korrekter Typ geparst werden
 * konnte, wird ParseException oder SRangeException geworfen.
 */
Sp<const Decl> parseTypeDecl(SRange& r);

// Sp<const TypeExp> parseArray(SRange& r, Sp<const TypeExp> atomBefore);
// Sp<const TypeExp> parseUnion(SRange& r, Vec<Sp<const TypeExp>> alternativesBefore);
// Sp<const Decl> parseDecl(SRange& r, char first);