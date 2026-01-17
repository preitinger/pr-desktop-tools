#ifndef __TYPE_H__
#define __TYPE_H__

#include "utils.H"
#include "Indent.H"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <stack>
#include <set>

using LocalNames = std::set<std::string>;

class Identifier
{
public:
    std::string name;

    Identifier() : name() {}
    Identifier(CSIt& it, const CSIt& end);
    Identifier(Identifier&& other);
    Identifier(const Identifier& other);

    bool operator<(const Identifier&) const;

    Identifier& operator=(Identifier&& other);

    static bool first(char c);
    static bool first(const CSIt& it, const CSIt& end) { return it != end && first(*it); }

    friend std::ostream& operator<<(std::ostream&, const Identifier&);
};

class TemplateArg;

class TemplateArgVec
{
public:
    std::vector<TemplateArg> vec;
    TemplateArgVec() : vec() {}
    TemplateArgVec(CSIt& it, const CSIt& end);

    void genList(std::ostream& o) const;
    bool operator<(const TemplateArgVec&) const;
};

class TemplateArg
{
public:
    Identifier id;
    TemplateArgVec templateArgs;

    TemplateArg(CSIt& it, const CSIt& end);
    // TemplateArg(const TemplateArg&);

    void genWriteArg(std::ostream& o) const;
    void genReadArg(std::ostream& o) const;
    void gen(std::ostream& o) const;
    bool operator<(const TemplateArg&) const;

    static bool first(char c) { return Identifier::first(c); }
    static bool first(const CSIt& it, const CSIt& end) { return it != end && first(*it); }
};

class TemplateArgPLess
{
public:
    inline bool operator()(const TemplateArg* a, const TemplateArg* b) const
    {
        return *a < *b;
    }
};

using TemplateArgPtr = std::shared_ptr<TemplateArg>;

class Literal
{
public:
    char quote;
    std::string value;
    Literal(CSIt& it, const CSIt& end);
    static bool first(char c);
    static bool first(const CSIt& it, const CSIt& end);
    void gen(std::ostream& out) const;
};

using LiteralPtr = std::shared_ptr<Literal>;

// nur zum Testen:
class A
{
public:
    A();
    A(const A&);
    A(A&&);
    A& operator=(const A&);
    A& operator=(A&&);

    int x;
};

class Literals
{
public:
    std::vector<Literal> vec;

    Literals() : vec() {}
    Literals(CSIt& it, const CSIt& end);
    static bool first(char c) { return Literal::first(c); }
    static bool first(CSIt& it, const CSIt& end) { return Literal::first(it, end); }
    void genWrite(std::ostream& out, Indent& indent) const;
    void genRead(std::ostream& out, Indent& indent) const;
};

class Imports;

// Aus Variants ausgelagert von KI:
class FuncGenerator {
public:
    std::map<const TemplateArg*, size_t, TemplateArgPLess> type2idx;
    std::vector<const TemplateArg*> typeOrder;
    const std::vector<const TemplateArg*>* variants;

    FuncGenerator() : type2idx(TemplateArgPLess()), typeOrder(), variants(nullptr) {}

    /**
     * Verarbeitet einen vorbereiteten Stack von Template-Argumenten.
     * Analysiert rekursiv Abhängigkeiten und baut die typeOrder auf.
     * Optional auch die Template-Argumente von Varianten erfasst
     * und die Varianten selbst vorgemerkt für die spätere Generation der Funktionen
     * writeBla_Variant bzw. readBla_Variant aus genWriteBla_Variant bzw. genReadBla_Variant.
     */
    void analyzeTemplates(std::stack<const TemplateArg*>& dfs, const std::vector<const TemplateArg*>* variants);

    // Aus Variants verschobene Methoden
    void genWriteFunctions(std::ostream& out, Indent& indent, Imports& imports) const;

    /**
     * @param prefix - "genWrite" oder "genRead"
     */
    void genCalledFunction(std::ostream& out, const TemplateArg& templateArg, Imports& imports, const Str& prefix, const Str& argPrefix, bool variant) const;

    void genReadFunctions(std::ostream& out, Indent& indent, Imports& imports) const;

    void genFunc(std::ostream& out, const TemplateArg& type, const std::string& prefix, Imports& imports) const;
};

class AttributeType
{
public:
    TemplateArgPtr baseType;
    // or
    LiteralPtr literal;

    bool array;

    AttributeType(CSIt& it, const CSIt& end);
    AttributeType(AttributeType&&);

    AttributeType& operator=(AttributeType&&);

    void genBaseWrite(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, const std::string& varName, Imports& imports) const;
    void genBaseRead(std::ostream& out, const FuncGenerator& funcGen, Imports& imports) const;

    void genWrite(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, const std::string& varName, Imports& imports) const;
    void genRead(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, Imports& imports) const;

    static bool first(char c) { return Literal::first(c) || TemplateArg::first(c); }
    static bool first(const CSIt& it, const CSIt& end) { return Literal::first(it, end) || TemplateArg::first(it, end); }
};

using AttributeTypePtr = std::shared_ptr<AttributeType>;

class Attribute
{
public:
    Identifier id;
    AttributeType type;

    Attribute(CSIt& it, const CSIt& end);
    Attribute(Attribute&&);

    void genWrite(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, Imports& imports) const;
    void genRead(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, Imports& imports) const;
    bool isTypeAttribute() const;

private:
    void genBaseWrite(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, const std::string& varName, Imports& imports) const;
    void genBaseRead(std::ostream& out, const FuncGenerator& funcGen, Imports& imports) const;
};

class Attributes
{
public:
    std::vector<Attribute> vec;
    Attributes(CSIt& it, const CSIt& end);
    const Attribute* getTypeAttribute() const;
};

class Variant;
using VariantPtr = std::shared_ptr<Variant>;

class Variant {
public:
    virtual ~Variant() {}
    virtual bool isObjectVariant() const = 0;
    virtual bool isSynonym() const = 0;
    virtual Str toString() const = 0;

    static VariantPtr parse(CSIt& it, const CSIt& end);
};

class ObjectVariant;
using ObjectVariantPtr = std::shared_ptr<ObjectVariant>;

class ObjectVariant : public Variant
{
public:
    Attributes attributes;
    ObjectVariant(CSIt& it, const CSIt& end);

    inline bool isObjectVariant() const { return true; }
    inline bool isSynonym() const { return false; }

    bool hasTypeAttribute() const;
    const Attribute* getTypeAttribute() const;
    void genWrite(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, Imports& imports) const;
    void genRead(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, Imports& imports) const;

    Str toString() const;
};

class SynonymVariant;
using SynonymVariantPtr = std::shared_ptr<SynonymVariant>;

class SynonymVariant : public Variant
{
public:
    AttributeType synonym;
    SynonymVariant(CSIt& it, const CSIt& end);
    inline bool isObjectVariant() const { return false; }
    inline bool isSynonym() const { return true; }

    Str toString() const { return synonym.literal ? synonym.literal->value : synonym.baseType->id.name; }
};

// alt:
// class Variants
// {
// public:
//     std::vector<Variant> vec;

//     Variants() : vec(), type2idx(), typeOrder() {}
//     Variants(CSIt& it, const CSIt& end);

//     void genWrite(std::ostream& out, Indent& indent) const;
//     void genRead(std::ostream& out, Indent& indent) const;

//     // BEGIN to refactor
//     std::map<const TemplateArg*, size_t, TemplateArgPLess> type2idx;
//     std::vector<const TemplateArg*> typeOrder;
//     void genWriteFunctions(std::ostream& out, Indent& indent) const;
//     void genWriteFunction(std::ostream& out, const TemplateArg& templateArg) const;
//     void genReadFunctions(std::ostream& out, Indent& indent) const;
//     void genFuncSuffix(std::ostream& out, const TemplateArg& type) const;
//     // END to refactor
// };

// Refaktorisiert von KI:
class Variants {
public:
    std::vector<Variant> vec;
    FuncGenerator funcGen; // Neue Instanz

    Variants() : vec(), funcGen() {}
    Variants(CSIt& it, const CSIt& end);

    void genWrite(std::ostream& out, Indent& indent, Imports& imports) const;
    void genRead(std::ostream& out, Indent& indent, Imports& imports) const;

    // Die markierten Methoden wurden entfernt, da sie nun in FuncGenerator liegen
};

using Forced = LocalNames;

class ImportsItem
{
public:
    std::set<std::string> funcs;
    std::set<std::string> types;

    void genImportList(std::ostream& out, const std::string_view& importPath, const LocalNames& localNames, const Forced& forced) const;
};

class Imports
{
public:
    ImportsItem base;
    ImportsItem other;
    LocalNames localNames;
    Forced forced;


    Imports();
    void eventuallyAdd(const std::string& prefix, const std::string& name);
    void force(const std::string& typeName);
};

class Type;
using AllTypes = std::map<std::string, Type>;

class AllTypeCollections;

class TypeOrigins {
public:
    TypeOrigins(const Type& type, AllTypeCollections& allTypes);
    std::pair<const Variant*, int> find(const Str& typeName) const;

private:
    std::map<Str, std::pair<const Variant*, std::uint_fast8_t> > origins;
};

class Type
{
public:
    Identifier id;
    TemplateArgVec templateArgs;

    std::vector<VariantPtr> variants;

    /**
     * reads a typescript type for which a binary writer and reader can be generated.
     * @throws NoTypeFound() if input does not contain a valid type
     */
    Type(CSIt& it, const CSIt& end, Imports& imports);

    void checkTypeAttributes(const AllTypes& allTypes) const;
    // void collect(const AllTypes& allTypes, StrVec& literals, StrVec& typeValues, Str& error) const;
    void genWriteVariant(std::ostream& out, Imports& imports, AllTypeCollections& allTypeCollections, const FuncGenerator& funcGen, const TypeOrigins& typeOrigins) const;
    void genWrite(std::ostream& out, Imports& imports, AllTypeCollections& allTypeCollections, const FuncGenerator& funcGen, const TypeOrigins& typeOrigins) const;
    void genReadVariant(std::ostream& out, Imports& imports, AllTypeCollections& allTypeCollections, const FuncGenerator& funcGen, const TypeOrigins& typeOrigins) const;
    void genRead(std::ostream& out, Imports& imports, AllTypeCollections& allTypeCollections, const FuncGenerator& funcGen, const TypeOrigins& typeOrigins) const;
    void genFullType(std::ostream& out) const;

private:
    void genArgDecls(std::ostream& out, bool write) const;
};

class TypeNotFoundException : public std::exception {
public:
    TypeNotFoundException();
    std::stacktrace st;
};

class TypeCollection {
public:
    const Type& type;
    StrVec literals;
    StrVec typeValues;
    Str error;

    TypeCollection(const Type& type, AllTypeCollections& allTypeCollections);
};

class AllTypeCollections {
public:
    AllTypeCollections(const AllTypes& allTypes);
    
    /**
     * @throws TypeNotFoundException
     */
    const TypeCollection& get(const Str& typeName);

private:
    const AllTypes& allTypes;
    std::map<Str, TypeCollection> m;
};


#endif // __TYPE_H__
