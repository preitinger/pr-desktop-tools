#include "TypeDecl.H"

#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>

#include <cassert>

static auto& out = std::cout;

Identifier::Identifier(CSIt& it, const CSIt& end)
    : name()
{
    skipWs(it, end);
    char c;
    if (it != end && Identifier::first((c = *it)))
    {
        name += c;

        while (++it != end && ((c = *it) >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_' || c >= '0' && c <= '9'))
        {
            name += c;
        }
    }
}

// static
bool Identifier::first(char c)
{
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
}

Identifier::Identifier(Identifier&& other)
    : name(std::move(other.name))
{
}

Identifier::Identifier(const Identifier& other)
    : name(other.name)
{
}

Identifier& Identifier::operator=(Identifier&& other)
{
    name = std::move(other.name);
    return *this;
}

Literal::~Literal() {
    out << "~Literal\n";
}

Type::~Type() {
    out << "~Type\n";
}


TypeDecl::TypeDecl(Str&& name, StrVec&& templateArgs, TypeSP&& type)
    : name(std::move(name)), templateArgs(std::move(templateArgs)), type(std::move(type)) {

}

Str skipIdentifier(CSIt& it, const CSIt& end) {
    skipWs(it, end);
    skipWs(it, end);

    Str name;
    char c;
    if (it != end && Identifier::first((c = *it)))
    {
        name += c;

        while (++it != end && ((c = *it) >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_' || c >= '0' && c <= '9'))
        {
            name += c;
        }
    }

    return name;
}

StrVec parseTemplateArgs(CSIt& it, const CSIt& end) {
    skipWs(it, end);
    StrVec templateArgs;

    if (it != end && *it == '<') {
        skipKeyword(it, end, "<");
        Identifier templateArg(it, end);
        templateArgs.push_back(templateArg.name);
        skipWs(it, end);
        while (it != end && *it == ',') {
            skipKeyword(it, end, ",");
            Identifier templateArg2(it, end);
            templateArgs.push_back(templateArg2.name);
        }
        skipKeyword(it, end, ">");
    }
    return templateArgs;
}


// static
TypeDecl TypeDecl::parse(CSIt& it, const CSIt& end) {
    skipWs(it, end);

    if (it != end && *it == 't') {
        skipKeyword(it, end, "type");
        Identifier name(it, end);
        StrVec templateArgs = parseTemplateArgs(it, end);

        skipKeyword(it, end, "=");
        TypeSP type = Type::parse(it, end);
        skipOptionalKeyword(it, end, ";");
        return TypeDecl(std::move(name.name), std::move(templateArgs), std::move(type));
    }
    else {
        skipKeyword(it, end, "interface");
        Identifier name(it, end);
        StrVec templateArgs = parseTemplateArgs(it, end);

        TypeDecl res(std::move(name.name), std::move(templateArgs), AttributeList::create(it, end));
        skipOptionalKeyword(it, end, ";");
        return res;
    }
}

// static
TypeSP Type::parse(CSIt& it, const CSIt& end) {
    TypeSP type;
    TypeCSPVec alternatives;

    do {
        skipWs(it, end);
        std::cerr << "Type::parse: ";
        if (it == end) {
            std::cerr << "it==end";
        }
        else {
            std::cerr << *it;
        }
        std::cerr << std::endl;

        if (Literal::first(it, end)) {
            type = Literal::create(it, end);
        }
        else if (AttributeList::first(it, end)) {
            type = AttributeList::create(it, end);
        }
        else if (Identifier::first(it, end)) {
            Identifier id(it, end);
            TypeCSPVec templateArgs;
            skipWs(it, end);
            if (it != end && *it == '<') {
                ++it;
                do {
                    templateArgs.push_back(Type::parse(it, end));
                    skipWs(it, end);

                    if (it != end && *it == '>') {
                        ++it;
                        break;
                    }

                    skipKeyword(it, end, ",");
                } while (true);
            }
            type = Reference::create(std::move(id.name), std::move(templateArgs));
        }

        skipWs(it, end);
        if (it != end && *it == '[') {
            ++it;
            skipKeyword(it, end, "]");
            type = Array::create(std::move(type));
        }

        skipWs(it, end);

        if (it != end && *it == '|') {
            ++it;
            alternatives.push_back(std::move(type));
            continue;
        }

        if (alternatives.empty()) {
            return type;
        }
        else {
            alternatives.push_back(std::move(type));
            return Union::create(std::move(alternatives));
        }
    } while (true);

}

Union::Union(TypeCSPVec&& alternatives) : alternatives(std::move(alternatives)) {
    std::cerr << "Union of length " << this->alternatives.size() << "\n";
}

// static
UnionSP Union::create(TypeCSPVec&& alternatives) {
    return UnionSP(new Union(std::move(alternatives)));
}

Array::Array(TypeCSP&& baseType) : baseType(std::move(baseType)) {
    std::cerr << "Array\n";
}


// static
ArraySP Array::create(TypeCSP&& baseType) {
    return ArraySP(new Array(std::move(baseType)));
}

Reference::Reference(Str&& name, TypeCSPVec&& templateArgs) : name(std::move(name)), templateArgs(std::move(templateArgs)) {
    std::cerr << "Reference [name " << this->name << "] - num template Args " << this->templateArgs.size() << "\n";
}

// static
ReferenceSP Reference::create(Str&& name, TypeCSPVec&& templateArgs) {
    return ReferenceSP(new Reference(std::move(name), std::move(templateArgs)));
}

// static
bool AttributeList::first(const CSIt& it, const CSIt& end) {
    return it != end && *it == '{';
}

AttributeList::AttributeList(CSIt& it, const CSIt& end)
    : attributes()
{
    std::cerr << "AttributeList\n";

    skipWs(it, end);
    skipKeyword(it, end, "{");
    skipWs(it, end);

    while (it != end && *it != '}') {
        attributes.emplace_back(it, end);
        skipWs(it, end);
        if (it != end && (*it == ',' || *it == ';')) {
            ++it;
        }
    }

    skipKeyword(it, end, "}");
}

Attribute::Attribute(CSIt& it, const CSIt& end)
{
    std::cerr << "Attribute\n";

    Identifier id(it, end);
    skipKeyword(it, end, ":");
    this->name = std::move(id.name);
    this->type = Type::parse(it, end);
}


// static
LiteralSP Literal::create(CSIt& it, const CSIt& end) {
    return LiteralSP(new Literal(it, end));
}



// static
bool Literal::first(const CSIt& it, const CSIt& end) {
    return it != end && (*it == '\'' || *it == '"');
}

Literal::Literal(CSIt& it, const CSIt& end)
    : val{}
{
    std::cerr << "Literal\n";
    skipWs(it, end);
    assertOrNoTypeFound(Literal::first(it, end));
    Str::value_type quote(*it);
    ++it;
    assertOrNoTypeFound(it != end);

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

}

using SStr = std::stringstream;

Str templateArgsToString(const StrVec& templateArgs) {
    SStr s;
    if (!templateArgs.empty()) {
        s << "<";
        auto it = templateArgs.begin();
        auto end = templateArgs.end();
        s << *it;
        while (++it != end) {
            s << ", " << *it;
        }
        s << ">";
    }

    return s.str();
}

Str templateArgsToString(const TypeCSPVec& templateArgs) {
    SStr s;
    if (!templateArgs.empty()) {
        s << "<";
        auto it = templateArgs.begin();
        auto end = templateArgs.end();
        s << (*it)->toString();
        while (++it != end) {
            s << ", " << (*it)->toString();
        }
        s << ">";
    }

    return s.str();

}

Str TypeDecl::toString() const {
    SStr s;

    s << "type " << name << templateArgsToString(templateArgs);
    // if (!templateArgs.empty()) {
    //     s << "<";
    //     auto it = templateArgs.begin();
    //     auto end = templateArgs.end();
    //     s << *it;
    //     while (++it != end) {
    //         s << ", " << *it;
    //     }
    //     s << ">";
    // }
    s << " = " << type->toString() << ";\n";

    return s.str();
}

Str Literal::toString() const {
    SStr o;
    o << "'";
    for (char c : val)
    {
        if (c == '\\') {
            o << "\\\\";
        }
        else if (c == '\'') {
            o << "\\'";
        }
        else {
            o << c;
        }
    }
    o << "'";

    return o.str();
}

Str Attribute::toString() const {
    SStr o;
    o << name << ": " << type->toString();
    return o.str();
}

Str AttributeList::toString() const {
    SStr o;
    o << "{\n";
    for (const auto& attribute : attributes) {
        o << attribute.toString() << ",\n";
    }
    o << "}";
    return o.str();
}

Str Reference::toString() const {
    SStr o;
    o << "[R] ";
    o << name << templateArgsToString(templateArgs);
    return o.str();
}

Str Array::toString() const {
    return baseType->toString() + "[]";
}

Str Union::toString() const {
    SStr o;
    o << "[U] ";
    auto it = alternatives.begin();
    auto end = alternatives.end();

    if (it != end) {
        o << (*it)->toString();
        while (++it != end) {
            o << " | \n";
            o << (*it)->toString();
        }
    }

    return o.str();
}

TypeCSP TypeDecl::bindTemplateArgs(const TypeCSPVec& binding) const {
    TemplateBinding m;
    auto bindingIt = binding.begin();
    const auto bindingEnd = binding.end();
    auto templateArgsIt = templateArgs.begin();
    const auto templateArgsEnd = templateArgs.end();
    for (bindingIt = binding.begin(), templateArgsIt = templateArgs.begin(); bindingIt != bindingEnd && templateArgsIt != templateArgsEnd; ++bindingIt, ++templateArgsIt) {
        TypeCSP binding = *bindingIt;
        out << "before emplace binding:\n" << binding->toString() << "\n";
        m.emplace(*templateArgsIt, std::move(binding));
    }

    return this->type->bind(m);
}


// virtual
TypeCSP Type::bind(const TemplateBinding& binding) const {
    out << "bind gibt sich selbst zurueck:\n" << this->toString() << "\n";
    return asTypeCSP();
}

TypeCSP Reference::bind(const TemplateBinding& binding) const {
    if (templateArgs.empty()) {
        auto found = binding.find(this->name);
        if (found != binding.end()) {
            return found->second;
        }
    }

    TypeCSPVec newArgs;

    for (const auto& oldArg : templateArgs) {
        newArgs.push_back(oldArg->bind(binding));
    }

    return Reference::create(Str(name), std::move(newArgs));
}

TypeCSP Array::bind(const TemplateBinding& binding) const {
    return Array::create(baseType->bind(binding));
}

TypeCSP Union::bind(const TemplateBinding& binding) const {
    TypeCSPVec newAlternatives;

    for (const auto& alternative : alternatives) {
        newAlternatives.push_back(alternative->bind(binding));
    }
    return Union::create(std::move(newAlternatives));
}

// virtual
ValidTypeCSP SimpleValidType::validate(const TypeDeclCMap& decls) const {
    return asValidTypeCSP();
}

ValidTypeCSP Union::validate(const TypeDeclCMap& decls) const {
    LiteralCSPVec literals;
    AttributeListCSPVec attributeLists;
    replaceReferences(decls, literals, attributeLists);
    return ValidUnion::create(std::move(literals), std::move(attributeLists));
}

void Literal::replaceReferences(const TypeDeclCMap& decls, LiteralCSPVec& literals, AttributeListCSPVec& attributeLists) const {
    literals.push_back(asLiteralCSP());
}

NotValidException::NotValidException()
{

}

void Reference::replaceReferences(const TypeDeclCMap& decls, LiteralCSPVec& literals, AttributeListCSPVec& attributeLists) const {
    out << "Reference::replaceReferences\n";
    TypeDeclCMap::const_iterator found = decls.find(this->name);
    if (found == decls.end()) {
        throw NotValidException();
    }
    out << "found typeDecl:\n" << found->second.toString() << "\n";
    out << "replaceReferences: this:\n" << toString() << "\n";
    auto bound = found->second.bindTemplateArgs(this->templateArgs);
    out << "bound:\n" << bound->toString() << "\n";
    bound->replaceReferences(decls, literals, attributeLists);
}

void Array::replaceReferences(const TypeDeclCMap& decls, LiteralCSPVec& literals, AttributeListCSPVec& attributeLists) const {
    throw NotValidException();
}

void Union::replaceReferences(const TypeDeclCMap& decls, LiteralCSPVec& literals, AttributeListCSPVec& attributeLists) const {
    std::ranges::for_each(alternatives,
        [&](const auto& alternative)
        {
            alternative->replaceReferences(decls, literals, attributeLists);
        }
    );
}

// static
AttributeListSP AttributeList::create(CSIt& it, const CSIt& end) {
    return AttributeListSP(new AttributeList(it, end));
}

void AttributeList::replaceReferences(const TypeDeclCMap& decls, LiteralCSPVec& literals, AttributeListCSPVec& attributeLists) const {
    // must check here, if does contain a valid type attribute
    bool hasValidType(false);
    std::ranges::for_each(attributes,
        [&](const auto& attribute)
        {
            if (attribute.name == "type") {
                const Literal* literal = dynamic_cast<const Literal*>(attribute.type.get());
                if (!literal) {
                    throw NotValidException();
                }
                hasValidType = true;
            }
        }
    );

    if (!hasValidType) {
        throw NotValidException();
    }

    attributeLists.push_back(asAttributeListCSP());
}

// static
ValidUnionSP ValidUnion::create(LiteralCSPVec&& literals, AttributeListCSPVec&& attributeLists) {
    return ValidUnionSP(new ValidUnion(std::move(literals), std::move(attributeLists)));
}

ValidUnion::ValidUnion(LiteralCSPVec&& literals, AttributeListCSPVec&& attributeLists)
    : literals(std::move(literals)), attributeLists(std::move(attributeLists))
{
}

Str ValidUnion::toString() const {
    SStr o;
    o << "{\n";
    o << "literals: [\n";
    std::ranges::for_each(literals,
        [&](const auto& literal)
        {
            o << literal->toString() << ", ";
        }
    );
    o << "],\n";
    o << "attributeLists: [\n";
    std::ranges::for_each(attributeLists,
        [&](const auto& attributeList)
        {
            o << attributeList->toString() << ", ";
        }
    );
    o << "]\n";
    o << "}";
    return o.str();
}

TypeCSP AttributeList::bind(const TemplateBinding& binding) const {
    AttributeVec newAttributes;
    std::ranges::for_each(this->attributes,
        [&](const auto& attribute)
        {
            newAttributes.emplace_back(Str(attribute.name), attribute.type->bind(binding));
        }
    );

    return AttributeList::create(std::move(newAttributes));
}

Attribute::Attribute(Str&& name, TypeCSP&& type) : name(std::move(name)), type(std::move(type)) {}

AttributeList::AttributeList(AttributeVec&& attributes) : attributes(std::move(attributes)) {}

AttributeListSP AttributeList::create(AttributeVec&& attributes) {
    return AttributeListSP(new AttributeList(std::move(attributes)));
}

// static
TypeDeclCMap TypeDecl::parseMap(CSIt& it, const CSIt& end) {
    TypeDeclCMap m;
    try {
        do {
            TypeDecl typeDecl = TypeDecl::parse(it, end);
            m.emplace(Str(typeDecl.name), std::move(typeDecl));
        } while (true);
    }
    catch (const NoTypeFound& ex) {
        ex.printStackTrace();
    }
    return m;
}

// void GenWriteFunc::gen(Output& out) const {
//     GenFuncArgsDecl args{ {{"dout", "DataOut"}, {"x", "X"}} };
//     GenExportFunc{ "write" + name, std::move(args), GenWriteFuncBodyCSP(new GenWriteFuncBody()) }.gen(out);
//     // out << out.indent << "export function write" << name << "()"
// }

void GenExportFunc::gen(Output& out) const {
    out << out.indent << "export function " << this->name;
    args.gen(out);
    out << " {\n";
    out.sub([&]()
        {
            assert(body);
            body->gen(out);
        }
    );
    out << out.indent << "}\n";
}

GenExportFuncs::GenExportFuncs(const TypeDecl& decl, const TypeDeclCMap& decls) : decl(decl), validType(decl.type->validate(decls)) {}

void GenExportFuncs::gen(Output& out) const {
    TypeCSPVec ownArgs;
    std::ranges::for_each(decl.templateArgs,
        [&](const auto& arg)
        {
            ownArgs.push_back(Reference::create(Str(arg), {}));
        }
    );
    TypeCSP ownType = Reference::create(Str(decl.name), std::move(ownArgs));
    GenFuncArgDecl ownArg{ "x", ownType };
    if (decl.templateArgs.empty()) {
        GenFuncArgsDecl writeArgsDecl{ GenFuncArgDeclVec{ GenFuncArgDecl{"dout", Reference::create("DataOut", {})}, std::move(ownArg)} };
        GenExportFunc(Str("write") + decl.name, std::move(writeArgsDecl), GenWriteFuncBodyCSP{ new GenWriteFuncBody{} }).gen(out);
        // GenWriteFunc{decl.name, GenWriteFuncRemainder{validType}}.gen(out);
        // GenReadFunc{decl.name, GenReadFuncRemainder{validType}}.gen(out);

    }
    // out << out.indent << "/* Must generate write and read func here for:\n";
    // out << this->decl.toString() << "\n";
    // out << this->validType->toString() << "\n";
    // out << "*/\n";
}

void GenReadFuncRemainder::gen(Output& out) const {

}

void GenReadFunc::gen(Output& out) const {

}

void GenFuncArgDecl::gen(Output& out) const {
    out << this->name << ": ";
    type->gen(out);
    // out << "/* nyi: GenFuncArgDecl */";
}

void GenFuncArgsDecl::gen(Output& out) const {
    out << "(";
    bool first = true;
    std::ranges::for_each(this->args,
        [&](const auto& arg)
        {
            if (first) {
                first = false;
            }
            else {
                out << ", ";
            }
            GenFuncArgDecl(arg).gen(out);
        }
    );
    out << ")";
}

void GenWriteFuncBody::gen(Output& out) const {
    // this->
    out << out.indent << "// nyi: GenWriteFuncBody\n";
}

void Literal::gen(Output& out) const {
    out << "'";
    for (const auto& c : val) {
        switch (c) {
        case '\'':
            out << "\\'";
            break;
        case '\\':
            out << "\\\\";
            break;
        default:
            out << c;
            break;
        }
    }
    out << "'";
}

void Attribute::gen(Output& out) const {
    out << this->name << ": ";
    this->type->gen(out);
}

void AttributeList::gen(Output& out) const {
    out << "{\n";
    out.sub([&]()
        {
            for (const auto& attribute : this->attributes) {
                out << out.indent;
                attribute.gen(out);
                out << ";\n";
            }
        }
    );
    out << out.indent << "}\n";
}

template <class T>
void genVec(Output& out, const std::vector<std::shared_ptr<const T> >& v, const Str& splitter) {
    bool first = true;
    for (const auto& t : v) {
        if (first) {
            first = false;
        }
        else {
            out << splitter;
        }
        t->gen(out);
    }

}

template <class T>
void genVec(Output& out, const std::vector<T>& v, const Str& splitter) {
    bool first = true;
    for (const auto& t : v) {
        if (first) {
            first = false;
        }
        else {
            out << splitter;
        }
        t.gen(out);
    }
}

void Reference::gen(Output& out) const {
    out << name;
    if (!templateArgs.empty()) {
        out << "<";
        genVec(out, templateArgs, ", ");
        out << ">";
    }
}

void Array::gen(Output& out) const {
    this->baseType->gen(out);
    out << "[]";
}

void Union::gen(Output& out) const {
    genVec(out, alternatives, " |\n" + out.indent.toString());
}
