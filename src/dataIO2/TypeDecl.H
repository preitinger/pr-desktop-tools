#ifndef __TYPE_DECL_H__
#define __TYPE_DECL_H__


#include "../dataIO-codegen/utils.H"

#include <memory>
#include <vector>
#include <map>

CDECLVECS(Type);
CDECLVECS(Literal);
CDECLVECS(AttributeList);
CDECLVECS(EnableSharedBase);

CDECLVECS(Array);
CDECLVECS(Union);
CDECLVECS(ValidUnion);

CDECLVECS(TypeDecl);
CMAPS(TypeDecl, Str, TypeDecl);

CDECL(GenFuncBody);
CDECL(GenWriteFuncBody);

CDECLVECS(GenFuncArgDecl);

class NotValidException : public std::exception {
public:
    NotValidException();
};

class CanGen {
public:
    CanGen() {}
    virtual ~CanGen() {}
    virtual void gen(Output& out) const = 0;
};

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

    friend Output& operator<<(Output&, const Identifier&);
};

using TemplateBinding = std::map<Str, TypeCSP>;


class TypeDecl {
public:
    Str name;
    StrVec templateArgs;
    TypeSP type;

    TypeDecl(Str&& name, StrVec&& templateArgs, TypeSP&& type);

    Str toString() const;

    TypeCSP bindTemplateArgs(const TypeCSPVec& binding) const;

    static TypeDecl parse(CSIt& it, const CSIt& end);
    static TypeDecl parse(const Str& s) { CSIt it = s.begin(); return parse(it, s.end()); }
    static TypeDeclCMap parseMap(CSIt&, const CSIt& end);
    static TypeDeclCMap parseMap(const Str& s) { CSIt it = s.begin(); return parseMap(it, s.end()); }
};

class ValidType;
using ValidTypeSP = std::shared_ptr<ValidType>;
using ValidTypeCSP = std::shared_ptr<const ValidType>;

class EnableSharedBase : public std::enable_shared_from_this<EnableSharedBase> {
protected:
    EnableSharedBase() {}
public:
    virtual ~EnableSharedBase() {}

    EnableSharedBaseSP asEnableSharedBaseSP() { return shared_from_this(); }
    EnableSharedBaseCSP asEnableSharedBaseCSP() const { return shared_from_this(); }
};

class Type : virtual public EnableSharedBase, public CanGen {
private:
protected:
    Type() {}

public:
    virtual ~Type();

    TypeSP asTypeSP()
    {
        return std::dynamic_pointer_cast<Type>(shared_from_this());
    }

    TypeCSP asTypeCSP() const {
        return std::dynamic_pointer_cast<const Type>(shared_from_this());
    }

    virtual Str toString() const = 0;

    virtual TypeCSP bind(const TemplateBinding& binding) const;

    virtual ValidTypeCSP validate(const TypeDeclCMap& decls) const = 0;

    virtual void replaceReferences(const TypeDeclCMap& decls, LiteralCSPVec& literals, AttributeListCSPVec& attributeLists) const = 0;

    static TypeSP parse(CSIt& it, const CSIt& end);
    virtual void gen(Output& out) const = 0;
};

class ValidType : virtual public EnableSharedBase {
protected:
    ValidType() {}

public:
    virtual ~ValidType() {}


    ValidTypeSP asValidTypeSP()
    {
        return std::dynamic_pointer_cast<ValidType>(shared_from_this());
    }

    ValidTypeCSP asValidTypeCSP() const {
        return std::dynamic_pointer_cast<const ValidType>(shared_from_this());
    }

    virtual Str toString() const = 0;
};

class SimpleValidType : public Type, public ValidType {
protected: SimpleValidType() : Type(), ValidType() {}
public:
    virtual ~SimpleValidType() {}
    ValidTypeCSP validate(const TypeDeclCMap& decls) const;
};

class Literal : public SimpleValidType {
private:
    Str val;
    struct Private { explicit Private() = default; };

public:
    ~Literal();

    Str toString() const override;
    void replaceReferences(const TypeDeclCMap& decls, LiteralCSPVec& literals, AttributeListCSPVec& attributeLists) const override;
    // virtual void replaceReferences(const TypeDeclCMap& decls, LiteralCSPVec& literals, AttributeListCSPVec& attributeLists) const = 0;
    LiteralSP asLiteralSP() { return std::dynamic_pointer_cast<Literal>(Type::shared_from_this()); }
    LiteralCSP asLiteralCSP() const { return std::dynamic_pointer_cast<const Literal>(Type::shared_from_this()); }

    static LiteralSP create(CSIt& it, const CSIt& end);
    static bool first(const CSIt& it, const CSIt& end);
    void gen(Output& out) const override;

private:
    Literal(CSIt& it, const CSIt& end);
};


class Attribute : public CanGen {
public:
    Str name;
    TypeCSP type;

    Attribute(CSIt& it, const CSIt& end);
    Attribute(Str&& name, TypeCSP&& type);

    Str toString() const;
    void gen(Output& out) const;
};

using AttributeVec = std::vector<Attribute>;

class AttributeList : public SimpleValidType {
private:
    AttributeList(CSIt& it, const CSIt& end);
    AttributeList(AttributeVec&& attributes);
public:
    AttributeVec attributes;

    static AttributeListSP create(CSIt& it, const CSIt& end);
    static bool first(const CSIt& it, const CSIt& end);


    Str toString() const override;
    void replaceReferences(const TypeDeclCMap& decls, LiteralCSPVec& literals, AttributeListCSPVec& attributeLists) const override;
    AttributeListCSP asAttributeListCSP() const { return std::dynamic_pointer_cast<const AttributeList>(Type::shared_from_this()); }
    TypeCSP bind(const TemplateBinding& binding) const override;
    static AttributeListSP create(AttributeVec&& attributes);
    void gen(Output& out) const override;
};

/*
Bsp:

type A<T> = B<Utf, T>

type B<T, S> = {
t: T;
s: S;
};

Dann waere gebunden: A<I32> = B<Utf, I32> = {
t: Utf;
s: I32;
}

*/

class Reference;
using ReferenceSP = std::shared_ptr<Reference>;

class Reference : public SimpleValidType {
private:
    Reference(Str&& name, TypeCSPVec&& templateArgs);
public:
    Str name;
    TypeCSPVec templateArgs;

    static ReferenceSP create(Str&& name, TypeCSPVec&& templateArgs);
    Str toString() const override;
    TypeCSP bind(const TemplateBinding& binding) const override;
    void replaceReferences(const TypeDeclCMap& decls, LiteralCSPVec& literals, AttributeListCSPVec& attributeLists) const override;
    void gen(Output& out) const override;

};

class Array : public SimpleValidType {
private:
    Array(TypeCSP&& baseType);
public:
    TypeCSP baseType;

    static ArraySP create(TypeCSP&& baseType);
    Str toString() const override;
    TypeCSP bind(const TemplateBinding& binding) const override;
    void replaceReferences(const TypeDeclCMap& decls, LiteralCSPVec& literals, AttributeListCSPVec& attributeLists) const override;
    void gen(Output& out) const override;
};

class Union : public Type {
private:
    Union(TypeCSPVec&& alternatives);
public:
    TypeCSPVec alternatives;

    static UnionSP create(TypeCSPVec&& alternatives);
    Str toString() const override;
    TypeCSP bind(const TemplateBinding& binding) const override;
    ValidTypeCSP validate(const TypeDeclCMap& decls) const override;
    void replaceReferences(const TypeDeclCMap& decls, LiteralCSPVec& literals, AttributeListCSPVec& attributeLists) const override;
    void gen(Output& out) const override;

};

class ValidUnion : public ValidType {
private:
    ValidUnion(LiteralCSPVec&& literals, AttributeListCSPVec&& attributeLists);
public:
    LiteralCSPVec literals;
    AttributeListCSPVec attributeLists;

    static ValidUnionSP create(LiteralCSPVec&& literals, AttributeListCSPVec&& attributeLists);

    Str toString() const;
};

struct GenWriteFuncRemainder {
    ValidTypeCSP validType;
    void gen(Output& out) const;
};

struct GenWriteFunc {
    const Str& name;
    GenWriteFuncRemainder remainder;
    void gen(Output& out) const;
};

struct GenReadFuncRemainder
{
    ValidTypeCSP validType;
    void gen(Output& out) const;
};






struct GenReadFunc {
    const Str& name;
    GenReadFuncRemainder remainder;
    void gen(Output& out) const;
};

struct GenFuncArgDecl {
    const Str name;
    TypeCSP type;
    void gen(Output& out) const;
};


struct GenFuncArgsDecl {
    GenFuncArgDeclVec args;
    void gen(Output& out) const;
};

struct GenFuncBody {
    virtual ~GenFuncBody() {}
    virtual void gen(Output& out) const = 0;
};

struct 
GenWriteFuncBody : public GenFuncBody {
    void gen(Output& out) const override;
};

struct GenExportFunc {
    Str name;
    GenFuncArgsDecl args;
    GenFuncBodyCSP body;

    GenExportFunc(Str&& name, GenFuncArgsDecl&& args, GenFuncBodyCSP&& body) : name(std::move(name)), args(std::move(args)), body(std::move(body)) {}
    void gen(Output& out) const;
};

struct GenExportFuncs
{
    const TypeDecl& decl;
    ValidTypeCSP validType;
    GenExportFuncs(const TypeDecl& decl, const TypeDeclCMap& decls);
    void gen(Output& out) const;
};


#endif // __TYPE_DECL_H__
