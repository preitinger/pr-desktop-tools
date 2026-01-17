#include "Type.H"
#include "Indent.H"
#include "utils.H"
#include <cassert>
#include <ostream>
#include <stack>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <queue>
#include <ranges>
#include <sstream>
#include <stacktrace>


static std::string known[] = {
    "U8",
    "I8",
    "U16",
    "I16",
    "U32",
    "I32",
    "Utf",
    "Bool",
    "Bin",
    "Array",
    "Opt",
};
static const std::string preRead{ "read" };
static const std::string preWrite("write");
static const std::string genRead("genRead");
static const std::string genWrite("genWrite");

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

// TemplateArgVec::TemplateArgVec()
//     : vec()
// {
// }

TemplateArgVec::TemplateArgVec(CSIt& it, const CSIt& end)
    : vec()
{
    skipWs(it, end);
    if (*it != '<')
    {
        return;
    }
    ++it;
    do
    {
        // vec.emplace_back(it, end);
        vec.emplace_back(it, end);
        skipWs(it, end);
        if (*it != ',')
        {
            break;
        }

        ++it;
    } while (true);
    assertOrNoTypeFound(*it == '>');
    ++it;
}

TemplateArg::TemplateArg(CSIt& it, const CSIt& end)
    : id(it, end),
    templateArgs(it, end)
{
    skipWs(it, end);

    if (it == end)
        return;
    if (*it == '[')
    {
        skipKeyword(it, end, "[]");
        TemplateArg inner(std::move(*this));
        id.name = "Array";
        this->templateArgs.vec.clear();
        this->templateArgs.vec.push_back(std::move(inner));
    }
}

Type::Type(CSIt& it, const CSIt& end, Imports& imports)
    : id(),
    templateArgs(),
    variants()
{
    skipOptionalKeyword(it, end, "export");
    skipWs(it, end);
    assertOrNoTypeFound(it != end && *it == 't' || *it == 'i');

    switch (*it)
    {
    case 't': // type
        skipKeyword(it, end, "type");
        id = Identifier(it, end);
        templateArgs = TemplateArgVec(it, end);
        skipKeyword(it, end, "=");
        skipWs(it, end);
        assertOrNoTypeFound(it != end);
        // old:
        // if (*it == '{')
        // {
        //     variants = Variants(it, end);
        // }
        // else if (Literals::first(it, end))
        // {
        //     literals = Literals(it, end);
        // }
        // else
        // {
        //     synonym.reset(new AttributeType(it, end));
        // }

        // new: Jetzt können auch Alternativen angegeben werden, wie z.B. type T = A | B.
        // Dann muss aber A und B jeweils ein Literal 'type' haben und die vereinigten Literale aus allen Alternativen des Typs
        // und Varianten müssen disjunkt sein.


        do {
            variants.emplace_back(Variant::parse(it, end));
            skipWs(it, end);
            if (it == end || *it != '|') break;
            ++it;
        } while (true);

        break;
    case 'i': // interface
        skipKeyword(it, end, "interface");
        id = Identifier(it, end);
        templateArgs = TemplateArgVec(it, end);
        variants.emplace_back(new ObjectVariant(it, end));
        break;
    }

    if (id.name == "")
    {
        throw NoTypeFound();
    }
    skipOptionalKeyword(it, end, ";");

    // Entweder:
    // * genau eine beliebige Variante, darf dann als einzige auch ein Array sein
    // Oder:
    // * Mehrere Variants, aber dann keines davon ein Array und ObjectVariants muessen Attribute 'type' haben.
    if (this->variants.size() > 1)
    {
        for (const auto& variant : variants)
        {
            if (variant->isObjectVariant()) {
                assertOrNoTypeFound(std::dynamic_pointer_cast<ObjectVariant>(variant)->hasTypeAttribute());
            }
            else {
                assert(variant->isSynonym());
                auto casted = std::dynamic_pointer_cast<SynonymVariant>(variant);
                assertOrNoTypeFound(!casted->synonym.array);
            }
        }
    }

    imports.force(this->id.name);
    if (templateArgs.vec.empty()) {
        imports.localNames.insert(preWrite + this->id.name);
        imports.localNames.insert(preRead + this->id.name);
    }
    else
    {
        imports.localNames.insert(::genWrite + this->id.name);
        imports.localNames.insert(::genRead + this->id.name);

        for (const auto& arg : templateArgs.vec) {
            const std::string& name = arg.id.name;
            imports.localNames.insert(name);
            imports.localNames.insert(preWrite + name);
            imports.localNames.insert(preRead + name);
        }
    }
}

// // alt - vor Refaktorisierung
// Variants::Variants(CSIt& it, const CSIt& end)
//     : vec(),
//     type2idx(TemplateArgPLess()),
//     typeOrder()
// {
//     do
//     {
//         vec.emplace_back(it, end);
//         skipWs(it, end);
//         if (it == end)
//             break;
//         if (*it != '|')
//             break;
//         skipKeyword(it, end, "|");
//     } while (true);

//     // Ab hier vec unmodifieable, also kann mit Referenzen darin bis zur Zerstoerung von this.

//     std::stack<const TemplateArg*> dfs; // depth-first-search

//     for (const auto& variant : vec)
//     {
//         for (const auto& attribute : variant.attributes.vec)
//         {
//             const auto& templateArg = attribute.type.baseType;
//             if (templateArg)
//             {
//                 dfs.push(templateArg.get());
//             }
//         }
//     }

//     while (!dfs.empty())
//     {
//         const TemplateArg* templateArg = dfs.top();
//         // std::cerr << "dfs: ";
//         // templateArg->gen(std::cerr);
//         // std::cerr << std::endl;
//         auto findRes = type2idx.find(templateArg);
//         if (findRes == type2idx.end())
//         {
//             auto vecEnd = templateArg->templateArgs.vec.rend();
//             bool childrenAvailable = true;
//             for (auto it = templateArg->templateArgs.vec.rbegin(); it != vecEnd; ++it)
//             {
//                 const auto& child = *it;
//                 auto childRes = type2idx.find(&child);
//                 if (childRes != type2idx.end())
//                 {
//                     // ignore
//                 }
//                 else
//                 {
//                     if (child.templateArgs.vec.empty())
//                     {
//                         // ignore
//                     }
//                     else
//                     {
//                         childrenAvailable = false;
//                         dfs.push(&child);
//                     }
//                 }
//             }

//             if (childrenAvailable)
//             {
//                 if (!templateArg->templateArgs.vec.empty())
//                 {
//                     type2idx.emplace(templateArg, typeOrder.size());
//                     typeOrder.push_back(templateArg);
//                 }
//                 dfs.pop();
//             }
//         }

//         // nur evtl:
//         // dfs.pop();
//     }
// }

// // nach Refaktorisierung durch KI:
// Variants::Variants(CSIt& it, const CSIt& end)
//     : vec()
// {
//     // 1. Parser-Logik
//     do {
//         vec.emplace_back(it, end);
//         skipWs(it, end);
//         if (it == end || *it != '|') break;
//         skipKeyword(it, end, "|");
//         assertOrNoTypeFound(vec.size() < 256); // da als u8 codiert

//     } while (true);

//     // 2. Vorbereitung der Analyse (Schritt 1)
//     std::stack<const TemplateArg*> dfs;
//     for (const auto& variant1 : vec) {
//         const ObjectVariant& variant = dynamic_cast<const ObjectVariant&>(variant1);
//         for (const auto& attribute : variant.attributes.vec) {
//             if (const auto& templateArg = attribute.type.baseType) {
//                 dfs.push(templateArg.get());
//             }
//         }
//     }

//     // 3. Durchführung der Analyse (Schritt 2)
//     funcGen.analyzeTemplates(dfs);
// }

// TemplateArg::TemplateArg(const TemplateArg &other)
//     : id(other.id),
//       templateArgs(other.templateArgs)
// {
// }

bool TemplateArg::operator<(const TemplateArg& other) const
{
    if (id < other.id)
        return true;
    if (other.id < id)
        return false;
    return (templateArgs < other.templateArgs);
}

bool TemplateArgVec::operator<(const TemplateArgVec& other) const
{
    size_t size = vec.size();
    size_t otherSize = other.vec.size();
    if (size < otherSize)
        return true;
    if (otherSize < size)
        return false;
    auto vecEnd = vec.end();

    for (auto ownArg = vec.begin(), otherArg = other.vec.begin(); ownArg != vecEnd; ++ownArg, ++otherArg)
    {
        if (*ownArg < *otherArg)
            return true;
        if (*otherArg < *ownArg)
            return false;
    }

    return false;
}

bool Identifier::operator<(const Identifier& other) const
{
    return name < other.name;
}

ObjectVariant::ObjectVariant(CSIt& it, const CSIt& end)
    : attributes(skipKeyword(it, end, "{"), end)
{
    skipKeyword(it, end, "}");
}

Attributes::Attributes(CSIt& it, const CSIt& end)
    : vec()
{
    skipWs(it, end);
    while (Identifier::first(*it))
    {
        vec.emplace_back(it, end);
        skipWs(it, end);
    }
}

Attribute::Attribute(CSIt& it, const CSIt& end)
    : id(it, end),
    type(skipKeyword(it, end, ":"), end)
{
    skipOptionalKeyword(it, end, ",");
    skipOptionalKeyword(it, end, ";");
}

Attribute::Attribute(Attribute&& other)
    : id(std::move(other.id)),
    type(std::move(other.type))
{
}

AttributeType::AttributeType(CSIt& it, const CSIt& end)
    : baseType(),
    literal(),
    array(false)
{
    skipWs(it, end);
    assertOrNoTypeFound(it != end);
    if (Literal::first(*it))
    {
        literal.reset(new Literal(it, end));
    }
    else
    {
        baseType.reset(new TemplateArg(it, end));
        if (it == end)
            return;
        if (*it != '[')
            return;
        skipKeyword(it, end, "[]");
        array = true;
    }
}

AttributeType::AttributeType(AttributeType&& other)
    : baseType(std::move(other.baseType)),
    literal(std::move(other.literal)),
    array(other.array)
{
}

AttributeType& AttributeType::operator=(AttributeType&& other)
{
    baseType = std::move(other.baseType);
    literal = std::move(other.literal);
    array = other.array;
    return *this;
}

// static
bool Literal::first(char c)
{
    return c == '\'' || c == '"';
}

// static
bool Literal::first(const CSIt& it, const CSIt& end)
{
    return it != end && first(*it);
}


Literal::Literal(CSIt& it, const CSIt& end)
    : quote(),
    value()
{
    assertOrNoTypeFound(it != end);
    quote = *it;
    assertOrNoTypeFound(Literal::first(quote));
    ++it;
    assertOrNoTypeFound(it != end);

    while (*it != quote)
    {
        if (*it == '\\')
        {
            ++it;
            assertOrNoTypeFound(it != end);
            value += *it;
            ++it;
        }
        else
        {
            value += *it;
            ++it;
        }
    }
    ++it; // skip final quote
}

bool Attribute::isTypeAttribute() const
{
    return id.name == "type" && !!type.literal;
}

bool ObjectVariant::hasTypeAttribute() const
{
    for (const auto& attribute : attributes.vec)
    {
        if (attribute.isTypeAttribute())
        {
            return true;
        }
    }

    return false;
}

void Type::genWrite(std::ostream& out, Imports& imports, AllTypeCollections& allTypeCollections, const FuncGenerator& funcGen, const TypeOrigins& typeOrigins) const
{
    Indent indent;

    const TypeCollection& ownCol = allTypeCollections.get(id.name);

    auto genForTypeValues = [&](std::uint_fast8_t& tag)
        {
            if (ownCol.typeValues.size() > 1) {
                out << indent << "switch (x.type) {\n";
                indent.sub([&]()
                    {
                        std::ranges::for_each(ownCol.typeValues, [&](auto typeValue)
                            {
                                out << indent << "case '" << typeValue << "':\n";
                                indent.sub([&]()
                                    {
                                        out << indent << "dout.u8(" << (int)(tag++) << ");\n";
                                        out << indent << "break;\n";
                                    }
                                );
                            }
                        );
                    }
                );
                out << indent << "}\n";
                // TODO
                // assert(false);
            }
            else {
                out << indent << "// Fall typeValues.size() <= 1 noch nicht implementiert.\n";
            }

        };

    auto writeBody = [&]()
        {
            indent.sub([&]()
                {
                    // funcGen.genWriteFunctions(out, indent, imports);
                    // out << "\n";

                    if (ownCol.literals.size() + ownCol.typeValues.size() > 1) {
                        std::uint_fast8_t tag = 0;

                        if (!ownCol.literals.empty()) {
                            out << indent << "switch (x) {\n";
                            indent.sub([&]()
                                {
                                    for (const auto& literal : ownCol.literals) {
                                        out << indent << "case ";
                                        out << "'" << literal << "':\n";
                                        indent.sub([&]()
                                            {
                                                out << indent << "dout.u8(" << (int)tag << ");\n";
                                                out << indent << "break;\n";
                                            }
                                        );

                                        ++tag;
                                    }

                                    out << indent << "default:\n";
                                    indent.sub([&]()
                                        {
                                            genForTypeValues(tag);
                                        }
                                    );
                                }
                            );
                            out << indent << "} // switch\n";
                        }
                        else {
                            genForTypeValues(tag);
                        }

                        if (templateArgs.vec.empty()) {
                            out << indent << preWrite << id << "_Variant(dout, x);\n";
                        }
                        else {
                            out << indent << "genWrite" << id << "_Variant(";
                            auto templateArgsEnd = templateArgs.vec.end();
                            for (auto it = this->templateArgs.vec.begin(); it != templateArgsEnd;)
                            {
                                const auto& templateArg = *it;
                                out << "write" << templateArg.id;
                                if (++it == templateArgsEnd)
                                    break;
                                out << ", ";
                            }
                            out << ")(dout, x);\n";
                        }
                    }
                    else {
                        if (templateArgs.vec.empty()) {
                            out << indent << "write" << id << "_Variant(dout, x);\n";
                        }
                        else {
                            out << indent << "genWrite" << id << "_Variant(";
                            auto templateArgsEnd = templateArgs.vec.end();
                            for (auto it = this->templateArgs.vec.begin(); it != templateArgsEnd;)
                            {
                                const auto& templateArg = *it;
                                out << "write" << templateArg.id;
                                if (++it == templateArgsEnd)
                                    break;
                                out << ", ";
                            }
                            out << ")(dout, x);\n";

                        }
                    }

                    // out << indent << "write" << 
                }
            );

        };

    if (this->templateArgs.vec.empty()) {
        // TODO
        out << indent << "\nexport function write" << id.name << "(dout: DataOut, x: " << id.name << ") {\n";

        writeBody();
        out << indent << "}\n";
    }
    else {
        out << indent << "\nexport function genWrite" << id.name;
        genArgDecls(out, true);
        indent.sub([&]()
            {
                out << indent << "return function(dout: DataOut, x: " << id.name;
                templateArgs.genList(out);
                out << ") {\n";
                writeBody();
                out << indent << "}\n";
            }
        );
        out << indent << "}\n";
    }

    return;
    // OLD:
    // Indent indent;
    // imports.other.types.insert(this->id.name);

    // if (this->templateArgs.vec.empty())
    // {
    //     out << "\nexport function write" << this->id.name << "(dout: DataOut, x: ";
    //     this->genFullType(out);
    //     out << ") {\n";
    // }
    // else
    // {
    //     out << "\nexport function genWrite";
    //     this->genFullType(out);
    //     out << "(";
    //     auto templateArgsEnd = templateArgs.vec.end();
    //     for (auto it = this->templateArgs.vec.begin(); it != templateArgsEnd;)
    //     {
    //         const auto& templateArg = *it;
    //         templateArg.genWriteArg(out);
    //         if (++it == templateArgsEnd)
    //             break;
    //         out << ", ";
    //     }
    //     out << ") {\n";
    // }

    // indent.sub([&]()
    //     {
    //         if (templateArgs.vec.empty())
    //         {
    //             if (synonym) {
    //                 assert(!synonym->literal == !!synonym->baseType);
    //                 if (synonym->literal) {
    //                     out << indent << "// We do not write literals because they are constant and known by the reader.\n";
    //                 }
    //                 else {
    //                     assert(synonym->baseType);
    //                     FuncGenerator funcGen;
    //                     std::stack<const TemplateArg*> dfs;
    //                     dfs.push(synonym->baseType.get());
    //                     funcGen.analyzeTemplates(dfs);
    //                     funcGen.genWriteFunctions(out, indent, imports);
    //                     synonym->genWrite(out, indent, funcGen, "x", imports);
    //                 }
    //             }
    //             else if (!literals.vec.empty()) {
    //                 assertOrNoTypeFound(variants.vec.empty());
    //                 literals.genWrite(out, indent);
    //             }
    //             else {
    //                 variants.funcGen.genWriteFunctions(out, indent, imports);
    //                 variants.genWrite(out, indent, imports);
    //             }
    //         }
    //         else
    //         {
    //             out << indent << "return function write" << id.name << "(dout: DataOut, x: " << id.name;
    //             templateArgs.genList(out);
    //             out << ") {\n";
    //             indent.sub([&]()
    //                 {
    //                     if (synonym) {
    //                         assert(!synonym->literal == !!synonym->baseType);
    //                         if (synonym->literal) {
    //                             out << indent << "// We do not write literals because they are constant and known by the reader.\n";
    //                         }
    //                         else {
    //                             assert(synonym->baseType);
    //                             FuncGenerator funcGen;
    //                             std::stack<const TemplateArg*> dfs;
    //                             dfs.push(synonym->baseType.get());
    //                             funcGen.analyzeTemplates(dfs);
    //                             funcGen.genWriteFunctions(out, indent, imports);
    //                             synonym->genWrite(out, indent, funcGen, "x", imports);
    //                         }
    //                     }
    //                     else if (!literals.vec.empty()) {
    //                         assertOrNoTypeFound(variants.vec.empty());
    //                         literals.genWrite(out, indent);
    //                     }
    //                     else {
    //                         variants.funcGen.genWriteFunctions(out, indent, imports);
    //                         variants.genWrite(out, indent, imports);
    //                     }
    //                     // variants.funcGen.genWriteFunctions(out, indent);
    //                     // out << "\n";

    //                     // variants.genWrite(out, indent);


    //                 });
    //             out << indent << "};\n";
    //         }

    //     });

    // out << indent << "}\n";
}

void Type::genArgDecls(std::ostream& out, bool write) const {
    templateArgs.genList(out);
    out << "(";
    auto templateArgsEnd = templateArgs.vec.end();
    for (auto it = this->templateArgs.vec.begin(); it != templateArgsEnd;)
    {
        const auto& templateArg = *it;
        if (write) {
            templateArg.genWriteArg(out);
        }
        else {
            templateArg.genReadArg(out);
        }
        if (++it == templateArgsEnd)
            break;
        out << ", ";
    }
    out << ") {\n";

}

void Type::genReadVariant(std::ostream& out, Imports& imports, AllTypeCollections& allTypeCollections, const FuncGenerator& funcGen, const TypeOrigins& typeOrigins) const {
    Indent indent;
    const TypeCollection& ownCol = allTypeCollections.get(id.name);
    const StrVec& literals = ownCol.literals;
    const StrVec& typeValues = ownCol.typeValues;
    bool isTemplate = !templateArgs.vec.empty();
    bool hasTag = literals.size() + typeValues.size() > 1;

    auto caseForTag = [&](int tag, std::function<void()>&& body)
        {
            out << indent << "case " << tag << ":\n";
            indent.sub(body);
        };

    auto returnForLiteral = [&](const Str& literal)
        {
            out << indent << "return '" << literal << "';\n";
        };

    auto returnForVariant = [&](const Variant* variant, int tag)
        {
            if (variant->isObjectVariant()) {
                const ObjectVariant* ov = dynamic_cast<const ObjectVariant*>(variant);
                ov->genRead(out, indent, funcGen, imports);
            }
            else {
                assert(variant->isSynonym());
                const SynonymVariant* sv = dynamic_cast<const SynonymVariant*>(variant);

                if (sv->synonym.baseType) {
                    out << indent << "return read" << sv->synonym.baseType->id << "_Variant(din";
                    const TypeCollection& synCol = allTypeCollections.get(sv->synonym.baseType->id.name);
                    bool synColHasTag = synCol.literals.size() + synCol.typeValues.size() > 1;
                    if (synColHasTag) {
                        out << ", " << tag;
                    }
                    out << ");\n";

                }
                else {
                    assert(sv->synonym.literal);
                    out << indent << "return ";
                    sv->synonym.literal->gen(out);
                    out << ";\n";
                }
            }
        };
    auto returnForTypeValue = [&](const Str& typeValue)
        {
            auto variantAndTag = typeOrigins.find(typeValue);
            const Variant* variant = variantAndTag.first;
            int tag = variantAndTag.second;
            returnForVariant(variant, tag);
        };

    auto functionRest = [&]()
        {
            out << "(din: DataIn";
            if (hasTag) {
                out << ", tag: U8";
            }
            out << "): " << id;
            templateArgs.genList(out);
            out << " {\n";
            indent.sub([&]()
                {
                    funcGen.genReadFunctions(out, indent, imports);
                    out << "\n";

                    if (hasTag) {
                        out << indent << "switch (tag) {\n";
                        indent.sub([&]()
                            {
                                int tag = 0;
                                std::ranges::for_each(literals, [&](const Str& literal)
                                    {
                                        caseForTag(tag++, [&]()
                                            {
                                                returnForLiteral(literal);
                                            }
                                        );
                                    }
                                );

                                std::ranges::for_each(typeValues, [&](const Str& typeValue)
                                    {
                                        caseForTag(tag++, [&]()
                                            {
                                                returnForTypeValue(typeValue);
                                            }
                                        );
                                    }
                                );
                                out << indent << "default: throw new Error('Invalid tag for " << id << ": ' + tag);\n";
                            }
                        );
                        out << indent << "}\n";
                    }
                    else {
                        assert(variants.size() <= 1);
                        if (variants.empty()) {
                            out << indent << "return {};\n";
                        }
                        else {
                            returnForVariant(variants.front().get(), -1);
                        }
                    }

                }
            );
            out << indent << "}\n";

        };

    if (isTemplate) {
        out << indent << "\nexport function genRead" << id << "_Variant";
        genArgDecls(out, false); // incl. `{`
        indent.sub([&]()
            {
                out << indent << "return function ";
                functionRest();
            }
        );

        out << indent << "}\n";
    }
    else {
        out << indent << "\nexport function read" << id << "_Variant";
        functionRest();
    }
}

void Type::genRead(std::ostream& out, Imports& imports, AllTypeCollections& allTypeCollections, const FuncGenerator& funcGen, const TypeOrigins& typeOrigins) const {
    const TypeCollection& ownCol = allTypeCollections.get(id.name);
    const StrVec& literals = ownCol.literals;
    const StrVec& typeValues = ownCol.typeValues;
    bool isTemplate = !templateArgs.vec.empty();
    bool hasTag = literals.size() + typeValues.size() > 1;


    Indent indent;
    auto functionRest = [&]()
        {
            out << "(din: DataIn): " << id;
            templateArgs.genList(out);
            out << " {\n";
            indent.sub([&]()
                {
                    if (hasTag) {
                        out << indent << "const tag = din.u8();\n";
                    }
                    if (isTemplate) {
                        out << indent << "return genRead" << this->id << "_Variant(";

                        auto it = templateArgs.vec.begin();
                        auto end = templateArgs.vec.end();
                        while (it != end) {
                            funcGen.genFunc(out, *it, ::preRead, imports);
                            if (++it != end) out << ", ";
                        }

                        out << ")(din";
                        if (hasTag) {
                            out << ", tag";
                        }
                        out << ");\n";

                    }
                    else {
                        out << indent << "return read" << this->id << "_Variant(din";
                        if (hasTag) {
                            out << ", tag";
                        }
                        out << ");\n";
                    }
                }
            );
            out << indent << "}\n";

        };

    if (isTemplate) {
        out << indent << "\nexport function genRead" << id;
        genArgDecls(out, false); // incl. `{`
        indent.sub([&]()
            {
                out << indent << "return function ";
                functionRest();
            }
        );
        out << indent << "}\n";
    }
    else {
        out << indent << "\nexport function read" << id;
        functionRest();
    }

    // assert(false);
    // Indent indent;
    // imports.other.types.insert(this->id.name);

    // if (this->templateArgs.vec.empty())
    // {
    //     out << "\nexport function read" << this->id.name << "(din: DataIn): ";
    //     this->genFullType(out);
    //     out << " {\n";
    // }
    // else
    // {
    //     out << "\nexport function genRead";
    //     this->genFullType(out);
    //     out << "(";
    //     auto templateArgsEnd = templateArgs.vec.end();
    //     for (auto it = this->templateArgs.vec.begin(); it != templateArgsEnd;)
    //     {
    //         const auto& templateArg = *it;
    //         templateArg.genReadArg(out);
    //         if (++it == templateArgsEnd)
    //             break;
    //         out << ", ";
    //     }
    //     out << ") {\n";
    // }

    // indent.sub([&]()
    //     {
    //         if (templateArgs.vec.empty())
    //         {
    //             if (synonym) {
    //                 assert(!synonym->literal == !!synonym->baseType);

    //                 if (synonym->literal) {
    //                     // out << indent << "// We do not write literals because they are constant and known by the reader.\n";
    //                     out << indent << "return ";
    //                     synonym->literal->gen(out);
    //                     out << ";\n";
    //                 }
    //                 else {
    //                     assert(synonym->baseType);
    //                     FuncGenerator funcGen;
    //                     std::stack<const TemplateArg*> dfs;
    //                     dfs.push(synonym->baseType.get());
    //                     funcGen.analyzeTemplates(dfs);
    //                     funcGen.genReadFunctions(out, indent, imports);
    //                     out << indent << "return ";
    //                     synonym->genRead(out, indent, funcGen, imports);
    //                     out << ";\n";
    //                 }

    //             }
    //             else if (!literals.vec.empty()) {
    //                 literals.genRead(out, indent);
    //             }
    //             else {
    //                 variants.funcGen.genReadFunctions(out, indent, imports);
    //                 variants.genRead(out, indent, imports);
    //             }
    //         }
    //         else
    //         {
    //             out << indent << "return function read" << id.name << "(din: DataIn): " << id.name;
    //             templateArgs.genList(out);
    //             out << " {\n";
    //             indent.sub([&]()
    //                 {
    //                     if (synonym) {
    //                         assert(!synonym->literal == !!synonym->baseType);


    //                         if (synonym->literal) {
    //                             out << indent << "return ";
    //                             synonym->literal->gen(out);
    //                             out << ";\n";
    //                         }
    //                         else {
    //                             assert(synonym->baseType);
    //                             FuncGenerator funcGen;
    //                             std::stack<const TemplateArg*> dfs;
    //                             dfs.push(synonym->baseType.get());
    //                             funcGen.analyzeTemplates(dfs);
    //                             funcGen.genReadFunctions(out, indent, imports);
    //                             out << indent << "return ";
    //                             synonym->genRead(out, indent, funcGen, imports);
    //                             out << ";\n";
    //                         }

    //                     }
    //                     else if (!literals.vec.empty()) {
    //                         literals.genRead(out, indent);
    //                     }
    //                     else {
    //                         variants.funcGen.genReadFunctions(out, indent, imports);
    //                         variants.genRead(out, indent, imports);
    //                     }
    //                 }
    //             );
    //             out << indent << "};\n";
    //         }
    //     });

    // out << indent << "}\n";
}

// // alt - vor Refaktorisierung
// void Variants::genWrite(std::ostream& out, Indent& indent) const
// {
//     if (vec.size() > 1)
//     {
//         out << indent << "switch (x.type) {\n";
//         indent.sub([&]()
//             {
//                 auto variantsEnd = vec.end();
//                 size_t idx = 0;
//                 for (auto it = vec.begin(); it != variantsEnd; ++idx, ++it)
//                 {
//                     out << indent << "case ";
//                     auto typeAttribute = it->getTypeAttribute();
//                     assert(typeAttribute);
//                     assert(typeAttribute->type.literal);
//                     typeAttribute->type.literal->gen(out);
//                     out << ":\n";
//                     indent.sub([&]()
//                         {
//                             out << indent << "dout.u8(" << idx << ");\n";
//                             it->genWrite(out, indent, *this);
//                             out << indent << "break;\n";
//                         });
//                 }
//             });
//         out << indent << "} // switch\n";
//     }
//     else
//     {
//         if (!vec.empty())
//         {
//             vec.front().genWrite(out, indent, *this);
//         }
//     }
// }

// // nach Refaktorisierung durch KI:
// void Variants::genWrite(std::ostream& out, Indent& indent, Imports& imports) const {
//     if (vec.size() > 1) {
//         out << indent << "switch (x.type) {\n";
//         indent.sub([&]()
//             {
//                 size_t idx = 0;
//                 auto end = vec.end();
//                 for (auto it = vec.begin(); it != end; ++idx, ++it) {
//                     out << indent << "case ";
//                     auto typeAttribute = it->getTypeAttribute();
//                     assert(typeAttribute && typeAttribute->type.literal);
//                     typeAttribute->type.literal->gen(out);
//                     out << ":\n";
//                     indent.sub([&]()
//                         {
//                             out << indent << "dout.u8(" << idx << ");\n";
//                             it->genWrite(out, indent, funcGen, imports); // Übergibt Variants (enthält funcGen)
//                             out << indent << "break;\n";
//                         }
//                     );
//                 }
//                 out << indent << "default:\n";
//                 indent.sub([&] { out << indent << "throw new Error('Invalid type in ' + JSON.stringify(x));\n"; });
//             }
//         );
//         out << indent << "} // switch\n";
//     }
//     else if (!vec.empty()) {
//         vec.front().genWrite(out, indent, funcGen, imports);
//     }
// }

void Literals::genWrite(std::ostream& out, Indent& indent) const {
    if (vec.size() < 2) {
        // nothing to write because no choice
    }
    else {
        out << indent << "switch (x) {\n";
        indent.sub([&]()
            {
                size_t idx = 0;
                auto end = vec.end();
                for (auto it = vec.begin(); it != end; ++idx, ++it) {
                    out << indent << "case ";
                    it->gen(out);
                    out << ":\n";
                    indent.sub([&]()
                        {
                            out << indent << "dout.u8(" << idx << ");\n";
                            out << indent << "break;\n";
                        }
                    );
                }
                out << indent << "default:\n";
                indent.sub([&] { out << indent << "throw new Error('Invalid literal ' + x);\n"; });
            }
        );
        out << indent << "}\n";
    }
}

void Literals::genRead(std::ostream& out, Indent& indent) const {
    if (vec.size() < 2) {
        assert(vec.size() == 1);
        out << indent << "return ";
        vec.front().gen(out);
        out << ";\n";
    }
    else {
        out << indent << "const tag = din.u8();\n";
        out << indent << "switch (tag) {\n";
        indent.sub([&]()
            {
                size_t idx = 0;
                auto end = vec.end();
                for (auto it = vec.begin(); it != end; ++idx, ++it) {
                    out << indent << "case " << idx << ":\n";
                    indent.sub([&]()
                        {
                            out << indent << "return ";
                            it->gen(out);
                            out << ";\n";
                        }
                    );
                }
                out << indent << "default:\n";
                indent.sub([&] { out << indent << "throw new Error('Invalid literal tag ' + tag);\n"; });
            }
        );

        out << indent << "} // switch\n";
    }
}


// alt - vor Refaktorisierung
// void Variants::genRead(std::ostream& out, Indent& indent) const
// {
//     if (vec.size() > 1)
//     {
//         out << indent << "switch (din.u8()) {\n";
//         indent.sub([&]()
//             {
//                 auto variantsEnd = vec.end();
//                 size_t idx = 0;
//                 for (auto it = vec.begin(); it != variantsEnd; ++idx, ++it)
//                 {
//                     out << indent << "case " << idx << ":\n";
//                     indent.sub([&]()
//                         {
//                             it->genRead(out, indent, *this);
//                             // no break because genRead produces a return statement
//                         });
//                 }
//                 out << indent << "default:\n";
//                 indent.sub([&indent, &out]
//                     {
//                         out << indent << "throw new Error('Invalid type tag');\n";
//                     });

//             });
//         out << indent << "} // switch\n";
//     }
//     else
//     {
//         if (!vec.empty())
//         {
//             vec.front().genRead(out, indent, *this);
//         }
//         else
//         {
//             out << indent << "return {};\n";
//         }
//     }
// }

// // nach Refaktorisierung durch KI:
// void Variants::genRead(std::ostream& out, Indent& indent, Imports& imports) const {
//     if (vec.size() > 1) {
//         out << indent << "switch (din.u8()) {\n";
//         indent.sub([&]() {
//             for (size_t idx = 0; idx < vec.size(); ++idx) {
//                 out << indent << "case " << idx << ":\n";
//                 indent.sub([&]() {
//                     vec[idx].genRead(out, indent, funcGen, imports);
//                     });
//             }
//             out << indent << "default:\n";
//             indent.sub([&] { out << indent << "throw new Error('Invalid type tag');\n"; });
//             });
//         out << indent << "} // switch\n";
//     }
//     else if (!vec.empty()) {
//         vec.front().genRead(out, indent, funcGen, imports);
//     }
//     else {
//         out << indent << "return {};\n";
//     }
// }


void ObjectVariant::genWrite(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, Imports& imports) const
{
    for (const auto& attribute : attributes.vec)
    {
        if (!attribute.isTypeAttribute())
        {
            attribute.genWrite(out, indent, funcGen, imports);
        }
    }
}

void ObjectVariant::genRead(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, Imports& imports) const
{
    out << indent << "return {\n";
    indent.sub([&]()
        {
            for (const auto& attribute : attributes.vec)
            {
                // also for type attribute, differently than in genWrite().
                attribute.genRead(out, indent, funcGen, imports);
            }
        });
    out << indent << "};\n";
}

void Type::genFullType(std::ostream& out) const
{
    out << id.name;
    templateArgs.genList(out);
}

void TemplateArg::genWriteArg(std::ostream& o) const
{
    o << "write" << this->id.name << ": Writer<" << this->id << ">";
}

void TemplateArg::genReadArg(std::ostream& o) const
{
    o << "read" << this->id.name << ": Reader<" << this->id << ">";
}

void TemplateArgVec::genList(std::ostream& o) const
{
    if (vec.empty())
    {
        return;
    }

    o << "<";

    auto vecEnd = vec.end();
    for (auto it = vec.begin(); it != vecEnd;)
    {
        const auto& arg = *it;
        arg.gen(o);
        if (++it == vecEnd)
            break;
        o << ", ";
    }

    o << ">";
}

void TemplateArg::gen(std::ostream& o) const
{
    o << id.name;
    templateArgs.genList(o);
}

void Literal::gen(std::ostream& o) const
{
    o << quote;
    for (char c : value)
    {
        if (c == '\\')
        {
            o << "\\\\";
        }
        else
        {
            o << c;
        }
    }
    o << quote;
}

const Attribute* Attributes::getTypeAttribute() const
{
    for (const auto& attribute : this->vec)
    {
        if (attribute.isTypeAttribute())
        {
            return &attribute;
        }
    }

    return nullptr;
}

const Attribute* ObjectVariant::getTypeAttribute() const
{
    return this->attributes.getTypeAttribute();
}

// // Durch Refaktorisierung von KI entfernt:
// void Variants::genWriteFunctions(std::ostream& out, Indent& indent) const
// {
//     size_t len = typeOrder.size();
//     // std::cerr << "len " << len << std::endl;

//     for (size_t i = 0; i < len; ++i)
//     {
//         const TemplateArg& templateArg = *typeOrder[i];
//         out << indent << "const write" << i << " = ";
//         genWriteFunction(out, templateArg);
//     }
// }

// void Variants::genWriteFunction(std::ostream& out, const TemplateArg& templateArg) const
// {
//     out << "genWrite" << templateArg.id.name << "(";
//     auto end = templateArg.templateArgs.vec.end();
//     for (auto it = templateArg.templateArgs.vec.begin(); it != end;)
//     {
//         out << "write";
//         genFuncSuffix(out, *it);
//         if (++it == end)
//             break;
//         out << ", ";
//     }
//     out << ");\n";
// }

// void Variants::genReadFunctions(std::ostream& out, Indent& indent) const
// {
//     size_t len = typeOrder.size();

//     for (size_t i = 0; i < len; ++i)
//     {
//         const TemplateArg& templateArg = *typeOrder[i];
//         out << indent << "const read" << i << " = genRead" << templateArg.id.name << "(";
//         auto end = templateArg.templateArgs.vec.end();
//         for (auto it = templateArg.templateArgs.vec.begin(); it != end;)
//         {
//             out << "read";
//             genFuncSuffix(out, *it);
//             if (++it == end)
//                 break;
//             out << ", ";
//         }
//         out << ");\n";
//     }
// }

// void Variants::genFuncSuffix(std::ostream& out, const TemplateArg& type) const
// {
//     auto res = this->type2idx.find(&type);
//     if (res == type2idx.end())
//     {
//         assert(type.templateArgs.vec.empty());
//         out << type.id.name;
//     }
//     else
//     {
//         out << res->second;
//     }
// }

void Attribute::genBaseWrite(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, const std::string& varName, Imports& imports) const
{
    type.genBaseWrite(out, indent, funcGen, varName, imports);
}

void AttributeType::genBaseWrite(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, const std::string& varName, Imports& imports) const
{
    if (baseType)
    {
        if (baseType->id.name == "boolean")
        {
            out << indent << "dout.bool(" << varName << ");\n";
        }
        else if (baseType->id.name == "string")
        {
            out << indent << "dout.utf(" << varName << ");\n";
        }
        else
        {
            out << indent;
            funcGen.genFunc(out, *baseType, preWrite, imports);
            out << "(dout, " << varName << ");\n";
        }
    }
    else
    {
        // nothing to write because the literal is constant and known by the corresponding read function
    }
}

void Attribute::genBaseRead(std::ostream& out, const FuncGenerator& funcGen, Imports& imports) const
{
    type.genBaseRead(out, funcGen, imports);
}

void AttributeType::genBaseRead(std::ostream& out, const FuncGenerator& funcGen, Imports& imports) const
{
    if (baseType)
    {
        if (baseType->id.name == "boolean")
        {
            out << "din.bool()";
        }
        else if (baseType->id.name == "string")
        {
            out << "din.utf()";
        }
        else
        {
            funcGen.genFunc(out, *baseType, preRead, imports);
            out << "(din)";
        }
    }
    else
    {
        assert(literal);
        literal->gen(out);
    }
}

void Attribute::genWrite(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, Imports& imports) const
{
    type.genWrite(out, indent, funcGen, "x." + id.name, imports);
}

void AttributeType::genWrite(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, const std::string& varName, Imports& imports) const
{
    if (array)
    {
        out << indent << "dout.u32(" << varName << ".length);\n";
        out << indent << "for (const item of " << varName << ") {\n";
        indent.sub([&]()
            {
                genBaseWrite(out, indent, funcGen, "item", imports);
            });
        out << indent << "}\n";
    }
    else
    {
        genBaseWrite(out, indent, funcGen, varName, imports);
    }
}

void Attribute::genRead(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, Imports& imports) const
{
    out << indent << this->id << ": ";
    type.genRead(out, indent, funcGen, imports);
    out << ",\n";
}

void AttributeType::genRead(std::ostream& out, Indent& indent, const FuncGenerator& funcGen, Imports& imports) const
{

    if (array)
    {
        // Array.from({ length: din.u32() }, () => readActionOrBatch(din))
        out << "Array.from({ length: din.u32() }, () => ";
        genBaseRead(out, funcGen, imports);
        out << ")";
    }
    else
    {
        genBaseRead(out, funcGen, imports);
    }
}

// friend
std::ostream& operator<<(std::ostream& out, const Identifier& id)
{
    return out << id.name;
}

/**
 * Verarbeitet einen vorbereiteten Stack von Template-Argumenten.
 * Analysiert rekursiv Abhängigkeiten und baut die typeOrder auf.
 */
void FuncGenerator::analyzeTemplates(std::stack<const TemplateArg*>& dfs, const std::vector<const TemplateArg*>* variants) {
    this->variants = variants;
    if (variants) {
        std::ranges::for_each(*variants, [&](const TemplateArg* v)
            {
                std::ranges::for_each(v->templateArgs.vec,
                    [&](const TemplateArg& child) {
                        dfs.push(&child);
                    }
                );
            }
        );
    }

    while (!dfs.empty()) {
        const TemplateArg* templateArg = dfs.top();

        if (type2idx.find(templateArg) == type2idx.end()) {
            bool childrenProcessed = true;

            // Rückwärts-Iteration über Kinder, um DFS-Reihenfolge zu wahren
            for (auto rit = templateArg->templateArgs.vec.rbegin();
                rit != templateArg->templateArgs.vec.rend(); ++rit) {
                const auto& child = *rit;

                if (!child.templateArgs.vec.empty() && type2idx.find(&child) == type2idx.end()) {
                    childrenProcessed = false;
                    dfs.push(&child);
                }
            }

            if (childrenProcessed) {
                if (!templateArg->templateArgs.vec.empty()) {
                    type2idx.emplace(templateArg, typeOrder.size());
                    typeOrder.push_back(templateArg);
                }
                dfs.pop();
            }
        }
        else {
            dfs.pop();
        }
    }
}

// Aus Variants verschobene Methoden
void FuncGenerator::genWriteFunctions(std::ostream& out, Indent& indent, Imports& imports) const {
    for (size_t i = 0; i < typeOrder.size(); ++i) {
        const TemplateArg& templateArg = *typeOrder[i];
        out << indent << "const write" << i << " = ";
        genCalledFunction(out, templateArg, imports, genWrite, preWrite, false);
    }

    if (variants) {
        std::ranges::for_each(*variants, [&](const TemplateArg* v)
            {
                if (!v->templateArgs.vec.empty()) {
                    out << indent << "const write" << v->id << "_Variant = ";
                    genCalledFunction(out, *v, imports, genWrite, preWrite, true);
                }
            }
        );
    }
}

void FuncGenerator::genCalledFunction(std::ostream& out, const TemplateArg& templateArg, Imports& imports, const Str& prefix, const Str& argPrefix, bool variant) const {
    out << prefix << templateArg.id.name;
    if (variant) {
        out << "_Variant";
    }
    out << "(";
    if (!variant) {
        imports.eventuallyAdd(prefix, templateArg.id.name);
    }
    auto it = templateArg.templateArgs.vec.begin();
    auto end = templateArg.templateArgs.vec.end();
    while (it != end) {
        genFunc(out, *it, argPrefix, imports);
        if (++it != end) out << ", ";
    }
    out << ");\n";
}

void FuncGenerator::genReadFunctions(std::ostream& out, Indent& indent, Imports& imports) const {
    for (size_t i = 0; i < typeOrder.size(); ++i) {
        const TemplateArg& templateArg = *typeOrder[i];

        // alt:
        // out << indent << "const read" << i << " = genRead" << templateArg.id.name << "(";
        // imports.eventuallyAdd(genRead, templateArg.id.name);
        // auto it = templateArg.templateArgs.vec.begin();
        // auto end = templateArg.templateArgs.vec.end();
        // while (it != end) {
        //     genFunc(out, *it, preRead, imports);
        //     if (++it != end) out << ", ";
        // }
        // out << ");\n";

        // neu:
        out << indent << "const read" << i << " = ";
        genCalledFunction(out, templateArg, imports, genRead, preRead, false);
    }


    if (variants) {
        std::ranges::for_each(*variants, [&](const TemplateArg* v)
            {
                if (!v->templateArgs.vec.empty()) {
                    out << indent << "const read" << v->id << "_Variant = ";
                    genCalledFunction(out, *v, imports, genRead, preRead, true);
                }
            }
        );
    }

}

void FuncGenerator::genFunc(std::ostream& out, const TemplateArg& type, const std::string& prefix, Imports& imports) const {
    out << prefix;
    auto take = [&](const std::string& name)
        {
            out << name;
            imports.eventuallyAdd(prefix, name);
        };
    auto res = type2idx.find(&type);
    if (res == type2idx.end()) {
        assert(type.templateArgs.vec.empty());

        if (type.id.name == "string") {
            take("Utf");
        }
        else if (type.id.name == "boolean") {
            take("Bool");
        }
        else {
            take(type.id.name);
        }
    }
    else {
        out << res->second;
    }
}


void Imports::eventuallyAdd(const std::string& prefix, const std::string& name)
{
    // // std::size_t begin, end = func.length();
    // // if (startsWith(func, read))
    // // {
    // //     begin = read.length();
    // // }
    // // else if (startsWith(func, write))
    // // {
    // //     begin = write.length();
    // // }

    // auto compare = [&name](const std::string_view& name1)
    //     {
    //         return name == name1;
    //     };
    // for (const auto& name1 : known)
    // {
    //     if (compare(name1)) {
    //         base.funcs.insert(std::string(prefix) + name);
    //         base.types.insert(name);
    //         break;
    //         // } else {
    //         //     other.types.insert(name);
    //     }
    // }

    // other.types.insert(name);
    other.funcs.insert(prefix + name);
}

void Imports::force(const std::string& typeName)
{
    forced.insert(typeName);
}


Imports::Imports()
{
    // base.types.emplace("DataOut");
    // base.types.emplace("DataIn");
    // base.funcs.emplace("genWriteArray");
    // base.funcs.emplace("genReadArray");

    other.types.emplace("DataOut");
    other.types.emplace("DataIn");
    other.types.emplace("U8");
    other.types.emplace("Reader");
    other.types.emplace("Writer");
    localNames.emplace("Array");

}

void ImportsItem::genImportList(std::ostream& out, const std::string_view& importPath, const LocalNames& localNames, const Forced& forced) const
{
    // Beispiel:
    // import { ByteIn, ByteOut, genReadArray, genWriteArray, readI16, readI32, readI8, readU16, readU32, readU8, readUtf, writeI16, writeI32, writeI8, writeU16, writeU32, writeU8, writeUtf, type DataIn, type DataOut, type I16, type I32, type I8, type U16, type U32, type U8, type Utf } from "../both/byteStreams";

    std::set<std::string> types, funcs;
    std::set_difference(this->types.begin(), this->types.end(), localNames.begin(), localNames.end(), std::insert_iterator(types, types.begin()));
    std::set_difference(this->funcs.begin(), this->funcs.end(), localNames.begin(), localNames.end(), std::insert_iterator(funcs, funcs.begin()));

    types.insert(forced.begin(), forced.end());

    if (funcs.empty() && types.empty()) return;

    out << "import { ";
    auto funcsEnd = funcs.end();
    for (auto it = funcs.begin(); it != funcsEnd;)
    {
        out << *it;
        if (++it == funcsEnd) break;
        out << ", ";
    }

    if (!types.empty()) {
        if (!funcs.empty()) out << ", ";
        auto typesEnd = types.end();
        for (auto it = types.begin(); it != typesEnd;)
        {
            out << "type " << *it;
            if (++it == typesEnd) break;
            out << ", ";
        }
    }

    out << " } from " << importPath << ";\n";
}

Literals::Literals(CSIt& it, const CSIt& end)
    : vec()
{
    do {
        skipWs(it, end);
        if (!Literal::first(it, end)) break;

        vec.emplace_back(it, end);
        skipWs(it, end);
        if (!(it != end && *it == '|')) break;
        ++it;
        assertOrNoTypeFound(vec.size() < 256); // da als u8 codiert
    } while (true);
}


A::A() : x(0) {}

A::A(const A& other) : x(other.x) {
    std::cerr << "A(const&)\n";
}

A::A(A&& other) : x(std::move(other.x)) {
    std::cerr << "A(&&)\n";
}

A& A::operator=(const A& other) {
    std::cerr << "= const&\n";
    x = other.x;
    return *this;
}

A& A::operator=(A&& other) {
    std::cerr << "= &&\n";
    x = std::move(other.x);
    return *this;
}

// static
VariantPtr Variant::parse(CSIt& it, const CSIt& end) {
    skipWs(it, end);
    if (it == end) return VariantPtr();
    switch (*it) {
    case '{':
        return VariantPtr(new ObjectVariant(it, end));
    default:
        return VariantPtr(new SynonymVariant(it, end));
    }
}

SynonymVariant::SynonymVariant(CSIt& it, const CSIt& end) : synonym(it, end) {
}


void Type::checkTypeAttributes(const AllTypes& allTypes) const {
    std::set<std::string> typeVals;

    std::for_each(this->variants.begin(), this->variants.end(), [&](const VariantPtr& variant) {
        ObjectVariantPtr objectVariant = std::dynamic_pointer_cast<ObjectVariant>(variant);
        if (objectVariant) {
            const Attribute* typeAttribute = objectVariant->getTypeAttribute();
            assertOrNoTypeFound(typeAttribute);
            assertOrNoTypeFound(!!typeAttribute->type.literal);
            auto insRes = typeVals.insert(typeAttribute->type.literal->value);
            assertOrNoTypeFound(insRes.second);
        }
        });

}


TypeOrigins::TypeOrigins(const Type& type, AllTypeCollections& allTypeCollections)
{
    std::ranges::for_each(type.variants, [&](const VariantPtr& v)
        {
            if (v->isObjectVariant()) {
                ObjectVariant& ov = *std::dynamic_pointer_cast<ObjectVariant>(v);
                if (ov.hasTypeAttribute()) {
                    const Attribute* typeAttribute = ov.getTypeAttribute();
                    assert(typeAttribute);
                    const Str& typeValue = typeAttribute->type.literal->value;
                    auto res = origins.emplace(typeValue, std::make_pair(v.get(), -1)); // tag nicht benutzt bei ObjectVariant Instanzen
                    if (!res.second) {
                        std::cerr << "Probably a duplicate value for type: " << typeValue << "\n";
                    }
                }
            }
            else {
                SynonymVariant& sv = *std::dynamic_pointer_cast<SynonymVariant>(v);
                if (sv.synonym.baseType) {
                    auto& baseType = *sv.synonym.baseType;
                    const TypeCollection& variantCol = allTypeCollections.get(baseType.id.name);

                    std::uint_fast8_t tag = 0;

                    std::ranges::for_each(variantCol.typeValues, [&](const Str& typeValue)
                        {
                            auto res = origins.emplace(typeValue, std::make_pair(v.get(), tag++));
                            if (!res.second) {
                                std::cerr << "Probably a duplicate value for type: " << typeValue << "\n";
                            }
                        }
                    );


                    // auto found = allTypes.find(baseType.id.name);
                    // if (found != allTypes.end()) {
                    //     const Type& variantType = found->second;
                    //     StrVec variantLiterals, variantTypeValues;
                    //     Str error;
                    //     variantType.collect(allTypes, variantLiterals, variantTypeValues, error);
                    //     if (!error.empty()) {
                    //         std::cerr << "Fehler beim collect: " << error << "\n";
                    //         return;
                    //     }

                    //     std::uint_fast8_t tag = 0;

                    //     std::ranges::for_each(variantTypeValues, [&](const Str& typeValue)
                    //         {
                    //             auto res = origins.emplace(typeValue, std::make_pair(v.get(), tag++));
                    //             if (!res.second) {
                    //                 std::cerr << "Probably a duplicate value for type: " << typeValue << "\n";
                    //             }
                    //         }
                    //     );

                    // }

                }
            }
        });
}

std::pair<const Variant*, int> TypeOrigins::find(const Str& typeName) const {
    auto res = origins.find(typeName);
    if (res == origins.end()) return std::make_pair(nullptr, 0);
    return res->second;
}


void Type::genWriteVariant(std::ostream& out, Imports& imports, AllTypeCollections& allTypeCollections, const FuncGenerator& funcGen, const TypeOrigins& origins) const {
    const auto& typeCol = allTypeCollections.get(id.name);
    const auto& literals = typeCol.literals;
    const auto& typeValues = typeCol.typeValues;

    Indent indent;
    // TypeOrigins origins(*this, allTypes);

    auto genAWriteVariant1 = [&](const Variant* foundVar)
        {
            // out << indent << "// [in genAWriteVariant1]\n";
            if (foundVar) {
                if (foundVar->isSynonym()) {
                    const SynonymVariant* synonymVariant = dynamic_cast<const SynonymVariant*>(foundVar);
                    assert(synonymVariant);
                    if (synonymVariant->synonym.baseType) {
                        TemplateArg& baseType = *synonymVariant->synonym.baseType;
                        // if (baseType.templateArgs.vec.empty()) {
                        // out << indent << "// empty template args\n";
                        out << indent << "write" << baseType.id << "_Variant(dout, x);\n";
                        // }    
                        // else {
                        //     out << indent << "genWrite" << baseType.id << "_Variant(";
                        //     for (auto it = baseType.templateArgs.vec.begin(), end = baseType.templateArgs.vec.end(); it != end;) {
                        //         const auto& arg = *it;
                        //         funcGen.genFunc(out, arg, preWrite, imports);
                        //         // out << "write" << arg.id;
                        //         if (++it != end) {
                        //             out << ", ";
                        //         }
                        //     }

                        //     out << ")(dout, x);\n";
                        // }

                    }
                    else {
                        assert(synonymVariant->synonym.literal);
                        out << indent << "// Literal konstant, hier nix zu schreiben\n";
                    }

                }
                else {
                    const ObjectVariant* ov = dynamic_cast<const ObjectVariant*>(foundVar);
                    assert(ov);

                    // out << indent << "// !!! Hier genWrite aufgerufen.\n";
                    ov->genWrite(out, indent, funcGen, imports);
                }

            }

        };

    auto genAWriteVariant = [&](const Str& typeValue)
        {
            std::pair<const Variant*, int> foundVar = origins.find(typeValue);
            // out << indent << "// origin: " << foundVar.first << " " << (int)foundVar.second << " " << (foundVar.first ? foundVar.first->toString() : Str()) << "\n";
            if (foundVar.first) {
                genAWriteVariant1(foundVar.first);
                // if (foundVar->isSynonym()) {
                //     const SynonymVariant* synonymVariant = dynamic_cast<const SynonymVariant*>(foundVar);
                //     assert(synonymVariant);
                //     assert(synonymVariant->synonym.baseType);
                //     TemplateArg& baseType = *synonymVariant->synonym.baseType;
                //     if (baseType.templateArgs.vec.empty()) {
                //         out << indent << "write" << baseType.id << "_Variant(dout, x);\n";
                //     }
                //     else {
                //         out << indent << "genWrite" << baseType.id << "_Variant(";
                //         for (auto it = baseType.templateArgs.vec.begin(), end = baseType.templateArgs.vec.end(); it != end;) {
                //             const auto& arg = *it;
                //             funcGen.genFunc(out, arg, preWrite, imports);
                //             // out << "write" << arg.id;
                //             if (++it != end) {
                //                 out << ", ";
                //             }
                //         }

                //         out << ")(dout, x);\n";
                //     }

                // }
                // else {
                //     const ObjectVariant* ov = dynamic_cast<const ObjectVariant*>(foundVar);
                //     assert(ov);

                //     out << indent << "// !!! Hier genWrite aufgerufen.\n";
                //     ov->genWrite(out, indent, funcGen, imports);
                // }

            }
            else {
                out << indent << "// no variant found for type value: " << typeValue << "\n";
            }

        };

    auto genForTypeValues = [&]()
        {
            if (typeValues.size() > 1) {
                out << indent << "switch (x.type) {\n";
                indent.sub([&]()
                    {
                        std::ranges::for_each(typeValues, [&](auto typeValue)
                            {
                                out << indent << "case '" << typeValue << "':\n";
                                indent.sub([&]()
                                    {
                                        genAWriteVariant(typeValue);
                                        out << indent << "break;\n";
                                    }
                                );
                            }
                        );
                    }
                );
                out << indent << "}\n";
                // TODO
                // assert(false);
            }
            else {
                out << indent << "// Fall typeValues.size() <= 1 noch nicht implementiert.\n";
            }

        };

    auto writeBody = [&]()
        {
            indent.sub([&]()
                {
                    funcGen.genWriteFunctions(out, indent, imports);
                    out << "\n";

                    if (literals.size() + typeValues.size() > 1) {

                        if (!literals.empty()) {
                            out << indent << "switch (x) {\n";
                            indent.sub([&]()
                                {
                                    // std::uint_fast8_t tag = 0;
                                    for (const auto& literal : literals) {
                                        out << indent << "case ";
                                        out << "'" << literal << "':\n";
                                        indent.sub([&]()
                                            {
                                                out << indent << "// do nothing, here; Only in the main write function, the tag is written for this case.\n";
                                                out << indent << "break;\n";
                                            }
                                        );
                                    }

                                    out << indent << "default:\n";
                                    indent.sub([&]()
                                        {
                                            genForTypeValues();
                                        }
                                    );
                                }
                            );
                            out << indent << "} // switch\n";
                        }
                        else {
                            genForTypeValues();
                        }
                    }
                    else {
                        if (!variants.empty()) {
                            const auto& variant = variants.front();
                            genAWriteVariant1(variant.get());
                            // if (variant->isObjectVariant()) {
                            //     ObjectVariantPtr ov = std::dynamic_pointer_cast<ObjectVariant>(variant);
                            //     ov->genWrite(out, indent, funcGen, imports);
                            // }
                            // else {
                            //     SynonymVariantPtr sv = std::dynamic_pointer_cast<SynonymVariant>(variant);
                            //     if (sv->synonym.baseType) {
                            //         out << indent << "// Fall 'nur ein Synonym' noch nicht implementiert\n";
                            //         genAWriteVariant1(sv.get());
                            //     }
                            //     else {
                            //         out << indent << "// Fall nicht-baseType noch nicht implementiert.\n";
                            //     }
                            // }

                        }
                    }
                }
            );

        };

    if (this->templateArgs.vec.empty()) {
        // TODO
        out << indent << "\nexport function write" << id.name << "_Variant(dout: DataOut, x: " << id.name;
        templateArgs.genList(out);
        out << ") {\n";
        writeBody();
        out << indent << "}\n";
    }
    else {
        out << indent << "\nexport function genWrite" << id << "_Variant";
        genArgDecls(out, true);
        indent.sub([&]()
            {
                out << indent << "return function(dout: DataOut, x: " << id.name;
                templateArgs.genList(out);
                out << ") {\n";
                writeBody();
                out << indent << "}\n";
            }
        );
        out << indent << "}\n";
    }
}


// void Type::collect(const AllTypes& allTypes, StrVec& literals, StrVec& typeValues, Str& error) const {
//     std::queue<const Type*> remainingTypes;
//     std::set<Str> doneTypes;

//     remainingTypes.push(this);

//     while (!remainingTypes.empty()) {
//         const Type* t = remainingTypes.front();
//         remainingTypes.pop();
//         std::cerr << "\n **** Handling " << t->id.name << " ...\n";
//         for (const auto& variant : t->variants) {
//             if (variant->isObjectVariant()) {
//                 ObjectVariant& v = *std::dynamic_pointer_cast<ObjectVariant>(variant);
//                 const Attribute* typeAttribute = v.getTypeAttribute();
//                 std::cerr << "Doing variant " << (typeAttribute ? typeAttribute->type.literal->value : "null") << "\n";

//                 if (!typeAttribute && t->variants.size() > 1) {
//                     std::cerr << "Warnung: kein type Attribut in ObjectVariant von " << this->id.name << " obwohl dort mehrere Varianten existieren!\n";
//                 }
//                 if (typeAttribute) {
//                     if (typeAttribute->type.array) {
//                         std::cerr << "Fehler: type darf nicht als Array definiert sein in " << this->id.name << "!\n";
//                         continue;
//                     }
//                     if (!typeAttribute->type.literal) {
//                         std::cerr << "Fehler: type muss als String-Literal definiert sein in " << this->id.name << "!\n";
//                         continue;
//                     }

//                     typeValues.push_back(typeAttribute->type.literal->value);
//                 }
//             }
//             else {
//                 SynonymVariant& v = *std::dynamic_pointer_cast<SynonymVariant>(variant);
//                 if (v.synonym.literal) {
//                     literals.push_back(v.synonym.literal->value);
//                 }
//                 else if (v.synonym.array) {
//                     assert(v.synonym.baseType);
//                     if (t->variants.size() > 1) {
//                         std::cerr << "Fehler: bei mehreren Varianten, darf keine ein Array sein, so aber in: " << this->id.name << "!\n";
//                     }
//                 }
//                 else {
//                     std::cerr << "Suche " << v.synonym.baseType->id.name << " in allTypes ...\n";
//                     auto itNext = allTypes.find((v.synonym.baseType->id.name));
//                     if (itNext != allTypes.end()) {
//                         std::cerr << "   gefunden.\n";
//                         remainingTypes.push(&itNext->second);
//                     }
//                     else {
//                         std::cerr << "   nicht gefunden.\n";

//                     }

//                 }
//             }
//         }
//     }
// }

Str ObjectVariant::toString() const {
    const Attribute* typeAttr = getTypeAttribute();
    std::stringstream ss;
    typeAttr->type.literal->gen(ss);
    return ss.str();
}

TypeCollection::TypeCollection(const Type& type, AllTypeCollections& allTypeCollections) : type(type), literals(), typeValues(), error() {
    const Type* t = &type;
    for (const auto& variant : t->variants) {
        if (variant->isObjectVariant()) {
            ObjectVariant& v = *std::dynamic_pointer_cast<ObjectVariant>(variant);
            const Attribute* typeAttribute = v.getTypeAttribute();
            std::cerr << "Doing variant " << (typeAttribute ? typeAttribute->type.literal->value : "null") << "\n";

            if (!typeAttribute && t->variants.size() > 1) {
                std::cerr << "Warnung: kein type Attribut in ObjectVariant von " << t->id.name << " obwohl dort mehrere Varianten existieren!\n";
            }
            if (typeAttribute) {
                if (typeAttribute->type.array) {
                    std::cerr << "Fehler: type darf nicht als Array definiert sein in " << t->id.name << "!\n";
                    continue;
                }
                if (!typeAttribute->type.literal) {
                    std::cerr << "Fehler: type muss als String-Literal definiert sein in " << t->id.name << "!\n";
                    continue;
                }

                typeValues.push_back(typeAttribute->type.literal->value);
            }
        }
        else {
            SynonymVariant& v = *std::dynamic_pointer_cast<SynonymVariant>(variant);
            if (v.synonym.literal) {
                literals.push_back(v.synonym.literal->value);
            }
            else if (v.synonym.array) {
                assert(v.synonym.baseType);
                if (t->variants.size() > 1) {
                    std::cerr << "Fehler: bei mehreren Varianten, darf keine ein Array sein, so aber in: " << t->id.name << "!\n";
                }
            }
            else {
                std::cerr << "Suche " << v.synonym.baseType->id.name << " in allTypeCollections ...\n";
                const TypeCollection& synCollection = allTypeCollections.get(v.synonym.baseType->id.name);
                std::ranges::copy(synCollection.literals, std::back_inserter(literals));
                std::ranges::copy(synCollection.typeValues, std::back_inserter(typeValues));
            }
        }
    }

}


TypeNotFoundException::TypeNotFoundException() : st(std::stacktrace::current()) {}

AllTypeCollections::AllTypeCollections(const AllTypes& allTypes) : allTypes(allTypes), m() {
}

/**
 * @throws TypeNotFoundException
 */
const TypeCollection& AllTypeCollections::get(const Str& typeName) {
    auto it = m.find(typeName);
    if (it == m.end()) {
        auto typeIt = allTypes.find(typeName);
        if (typeIt == allTypes.end()) {
            throw TypeNotFoundException();
        }
        auto insertRes = m.emplace(typeName, TypeCollection(typeIt->second, *this));
        assert(insertRes.second); // because not found, before.
        it = insertRes.first;
    }

    return it->second;
}
