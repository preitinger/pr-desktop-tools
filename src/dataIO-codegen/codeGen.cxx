#include "utils.H"
#include "Type.H"
#include "TestInitializer.H"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iterator>
#include <map>
#include <memory>
#include <fstream>
#include <cstring>
#include <cassert>

// #include <stacktrace>


// struct Field
// {
//     std::string name;
//     std::string baseType;
//     std::string numberMethod = "u32";
//     bool canBeNull = false;
//     bool canBeUndefined = false;
//     bool isArray = false;
//     std::string literalValue = "";
// };

// struct Variant
// {
//     std::string typeTag = "";
//     std::vector<Field> fields;
// };

// std::vector<Field> parseFields(const std::string &block)
// {
//     std::vector<Field> fields;
//     // Erkennt: JSDoc, Name, Optional(?), Typ (inkl. Literale '...' oder "..." oder Unions), Kommentar
//     std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)(\?|)\s*:\s*([^;,}\n//]+)\s*;?,?\s*(?://\s*(\w+))?)");

//     auto it = std::sregex_iterator(block.begin(), block.end(), fieldRegex);
//     for (; it != std::sregex_iterator(); ++it)
//     {
//         std::smatch m = *it;
//         Field f;
//         f.name = m.str(2);
//         std::string fullType = m.str(4);
//         std::cout << "**** fullType in parseFields: '" << fullType << "'\n";

//         if (m.str(3) == "?")
//             f.canBeUndefined = true;
//         if (fullType.find("null") != std::string::npos)
//             f.canBeNull = true;
//         if (fullType.find("undefined") != std::string::npos)
//             f.canBeUndefined = true;

//         // Literal-Check für 'WERT' ODER "WERT"
//         std::regex litRegex(R"(['"]([^'"]+)['"])");
//         std::smatch litMatch;
//         if (std::regex_search(fullType, litMatch, litRegex))
//         {
//             f.literalValue = litMatch.str(1);
//             f.baseType = "literal";
//         }
//         else
//         {
//             f.isArray = (fullType.find("[]") != std::string::npos && fullType.find("Buffer") == std::string::npos);
//             if (fullType.find("Buffer[]") != std::string::npos)
//                 f.isArray = true;

//             std::regex baseRegex(R"((number|string|boolean|Buffer|[A-Z_][A-Za-z0-9_]*(<[^>]*>)?))");
//             std::smatch baseMatch;
//             if (std::regex_search(fullType, baseMatch, baseRegex))
//             {
//                 f.baseType = baseMatch.str(1);
//             }
//             std::cout << "***** baseType: '" << f.baseType << "'\n";
//         }

//         std::string searchPool = m.str(1) + " " + m.str(5);
//         std::regex numHint(R"((u8|i8|u16|i16|u32|i32))");
//         std::smatch hintMatch;
//         if (std::regex_search(searchPool, hintMatch, numHint))
//             f.numberMethod = hintMatch.str(1);

//         fields.push_back(f);
//     }
//     return fields;
// }

// void printFieldWrite(const Field &f, const std::string &access, const std::string &indent)
// {
//     if (f.baseType == "literal")
//         return;

//     if (f.canBeNull || f.canBeUndefined)
//     {
//         bool first = true;
//         if (f.canBeNull)
//         {
//             std::cout << indent << "if (" << access << " === null) { dout.u8(1); }" << std::endl;
//             first = false;
//         }
//         if (f.canBeUndefined)
//         {
//             std::cout << indent << (first ? "if" : "else if") << " (" << access << " === undefined) { dout.u8(2); }" << std::endl;
//         }

//         std::cout << indent << "else { dout.u8(0); " << std::endl;
//         std::string subIndent = indent + "    ";
//         if (f.isArray)
//         {
//             std::cout << subIndent << "dout.u32(" << access << ".length);" << std::endl;
//             std::cout << subIndent << "for (const item of " << access << ") {" << std::endl;
//             if (f.baseType == "number")
//                 std::cout << subIndent << "    dout." << f.numberMethod << "(item);" << std::endl;
//             else if (f.baseType == "string")
//                 std::cout << subIndent << "    dout.utf(item);" << std::endl;
//             else if (f.baseType == "boolean")
//                 std::cout << subIndent << "    dout.bool(item);" << std::endl;
//             else if (f.baseType == "Buffer")
//                 std::cout << subIndent << "    dout.bin(item);" << std::endl;
//             else
//                 std::cout << subIndent << "    write" << f.baseType << "(dout, item);" << std::endl;
//             std::cout << subIndent << "}" << std::endl;
//         }
//         else
//         {
//             if (f.baseType == "number")
//                 std::cout << subIndent << "dout." << f.numberMethod << "(" << access << ");" << std::endl;
//             else if (f.baseType == "string")
//                 std::cout << subIndent << "dout.utf(" << access << ");" << std::endl;
//             else if (f.baseType == "boolean")
//                 std::cout << subIndent << "dout.bool(" << access << ");" << std::endl;
//             else if (f.baseType == "Buffer")
//                 std::cout << subIndent << "dout.bin(" << access << ");" << std::endl;
//             else
//                 std::cout << subIndent << "write" << f.baseType << "(dout, " << access << ");" << std::endl;
//         }
//         std::cout << indent << "}" << std::endl;
//     }
//     else if (f.isArray)
//     {
//         std::cout << indent << "dout.u32(" << access << ".length);" << std::endl;
//         std::cout << indent << "for (const item of " << access << ") {" << std::endl;
//         if (f.baseType == "number")
//             std::cout << indent << "    dout." << f.numberMethod << "(item);" << std::endl;
//         else if (f.baseType == "string")
//             std::cout << indent << "    dout.utf(item);" << std::endl;
//         else if (f.baseType == "boolean")
//             std::cout << indent << "    dout.bool(item);" << std::endl;
//         else if (f.baseType == "Buffer")
//             std::cout << indent << "    dout.bin(item);" << std::endl;
//         else
//             std::cout << indent << "    write" << f.baseType << "(dout, item);" << std::endl;
//         std::cout << indent << "}" << std::endl;
//     }
//     else
//     {
//         if (f.baseType == "number")
//             std::cout << indent << "dout." << f.numberMethod << "(" << access << ");" << std::endl;
//         else if (f.baseType == "string")
//             std::cout << indent << "dout.utf(" << access << ");" << std::endl;
//         else if (f.baseType == "boolean")
//             std::cout << indent << "dout.bool(" << access << ");" << std::endl;
//         else if (f.baseType == "Buffer")
//             std::cout << indent << "dout.bin(" << access << ");" << std::endl;
//         else
//             std::cout << indent << "write" << f.baseType << "(dout, " << access << ");" << std::endl;
//     }
// }

// std::string getReadExpr(const Field &f)
// {
//     std::string core;
//     if (f.isArray)
//     {
//         std::string elem = (f.baseType == "number") ? "din." + f.numberMethod + "()" : (f.baseType == "string") ? "din.utf()"
//                                                                                    : (f.baseType == "boolean")  ? "din.bool()"
//                                                                                    : (f.baseType == "Buffer")   ? "din.bin()"
//                                                                                                                 : "read" + f.baseType + "(din)";
//         core = "Array.from({ length: din.u32() }, () => " + elem + ")";
//     }
//     else
//     {
//         if (f.baseType == "number")
//             core = "din." + f.numberMethod + "()";
//         else if (f.baseType == "string")
//             core = "din.utf()";
//         else if (f.baseType == "boolean")
//             core = "din.bool()";
//         else if (f.baseType == "Buffer")
//             core = "din.bin()";
//         else
//             core = "read" + f.baseType + "(din)";
//     }

//     if (!f.canBeNull && !f.canBeUndefined)
//         return core;

//     std::string res = "((tag = din.u8()) => ";
//     if (f.canBeNull)
//         res += "tag === 1 ? null : (";
//     if (f.canBeUndefined)
//         res += "tag === 2 ? undefined : (";
//     res += core;
//     if (f.canBeUndefined)
//         res += ")";
//     if (f.canBeNull)
//         res += ")";
//     res += ")()";
//     return res;
// }

// class TypeEntry;
// using SizeType = std::vector<TypeEntry>::size_type;

// // class TemplateInstanceOrSimpleType
// // {
// // public:
// //     // TemplateInstanceOrSimpleType(const std::string &simpleType) : name(simpleType), args() {}
// //     // TemplateInstanceOrSimpleType(const std::string &name, std::vector<std::shared_ptr<TemplateInstanceOrSimpleType>> &&args) : name(name), args(std::move(args)) {}

// //     TemplateInstanceOrSimpleType(CSIt &it, const CSIt &end);

// //     std::string name;
// //     std::vector<std::shared_ptr<TemplateInstanceOrSimpleType>> args;

// //     bool isSimple() const { return args.empty(); }
// //     bool operator<(const TemplateInstanceOrSimpleType &other) const
// //     {
// //         if (name < other.name)
// //             return true;
// //         if (other.name < name)
// //             return false;
// //         SizeType len = args.size();
// //         SizeType otherLen = other.args.size();
// //         if (len < otherLen)
// //             return true;
// //         if (otherLen < len)
// //             return false;

// //         for (auto i = args.begin(), j = other.args.begin(); i != args.end() && j != other.args.end(); ++i, ++j)
// //         {
// //             assert(*i && *j);
// //             if (**i < **j)
// //                 return true;
// //             if (**j < **i)
// //                 return false;
// //         }

// //         return false;
// //     }
// // };

// // TemplateInstanceOrSimpleType::TemplateInstanceOrSimpleType(CSIt &it, const CSIt &end)
// // : name(),
// // args()
// // {
// // }

// // class Templates;

// // class TypeEntry
// // {
// // public:
// //     SizeType idx;
// //     TemplateInstanceOrSimpleType templateInstanceOrSimpleType;
// //     TypeEntry(SizeType idx, TemplateInstanceOrSimpleType &&templateInstanceOrSimpleType, const std::string &readOrWrite, const Templates &templates);
// //     std::string funcDef;
// // };

// // TypeEntry::TypeEntry(SizeType idx, TemplateInstanceOrSimpleType &&templateInstanceOrSimpleType, const std::string &readOrWrite, const Templates &templates)
// //     : idx(idx),
// //       templateInstanceOrSimpleType(std::move(templateInstanceOrSimpleType)),
// //       funcDef()
// // {
// //     std::stringstream ss;
// // }

// // using TemplateMap = std::map<TemplateInstanceOrSimpleType, SizeType>;

// // class Templates
// // {
// // private:
// //     std::vector<TypeEntry> vec;
// //     TemplateMap m;

// // public:
// //     Templates() : vec(), m() {}
// //     ~Templates() {}

// //     void add(std::string &&typeName);
// //     const TypeEntry &get(SizeType idx) const;
// //     const TypeEntry &get(const std::string &typeName) const;

// //     std::string functionNameFor(const TemplateInstanceOrSimpleType& templateInstanceOrSimpleType, const std::string &readOrWrite) const;
// //     // std::string functionBodyFor(const TemplateInstanceOrSimpleType& templateInstanceOrSimpleTypeconst std::string &onlyName, SizeType idx, const std::vector<std::string> &args, const std::string &readOrWrite) const;
// // };

// // void Templates::add(std::string &&typeName)
// // {
// //     std::cerr << "nyi\n";
// //     // vec.emplace_back(vec.size(), std::move(typeName));
// // }

// // const TypeEntry &Templates::get(SizeType idx) const
// // {
// //     return vec.at(idx);
// // }

// // const TypeEntry &Templates::get(const std::string &typeName) const
// // {
// //     auto it = m.find(typeName);
// //     assert(it != m.end());
// //     SizeType idx = it->second;
// //     assert(idx < vec.size());
// //     return vec[idx];
// // }

// // std::string Templates::functionNameFor(const TemplateInstanceOrSimpleType& templateInstanceOrSimpleType, const std::string &readOrWrite) const
// // {
// //     auto it = m.find(templateInstanceOrSimpleType);
// //     if (it == m.end())
// //         return readOrWrite + typeName;
// //     std::stringstream ss;
// //     ss << readOrWrite << it->second;
// //     return ss.str();
// // }

// // std::string Templates::functionBodyFor(const std::string &onlyName, SizeType idx, const std::vector<std::string> &args, const std::string &readOrWrite) const
// // {
// //     std::stringstream ss;
// //     ss << "const " << readOrWrite << " = gen" << onlyName << "(";
// //     auto argsEnd = args.end();
// //     for (auto it = args.begin(); it != argsEnd; ++it)
// //     {
// //         ss << readOrWrite;
// //         auto found = m.find(*it);
// //         if (found == m.end())
// //         {
// //             ss << *it;
// //         }
// //         else
// //         {
// //             assert(found != m.end());
// //             SizeType argIdx = found->second;
// //             assert(argIdx < vec.size());
// //             ss << argIdx;
// //         }

// //         if (++it != argsEnd)
// //         {
// //             break;
// //         }
// //         ss << ", ";
// //     }

// //     ss << ");";

// //     return ss.str();
// // }

// using CSIt = std::string::const_iterator;

// void scanName(CSIt &it, const CSIt &end, std::string &name)
// {
//     name.clear();
//     char c;
//     if (it != end && (c = *it) >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_')
//     {
//         name += c;

//         while (++it != end && ((c = *it) >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_' || c >= '0' && c <= '9'))
//         {
//             name += c;
//         }
//     }
// }

// void skipWs(CSIt &it, const CSIt &end)
// {
//     char c;
//     while (it != end && ((c = *it) == ' ' || c == '\t' || c == '\r' || c == '\n'))
//         ++it;
// }

// void skipTemplateDef(CSIt &it, const CSIt &end)
// {
//     std::uint_fast32_t open = 0;
//     skipWs(it, end);

//     while (it != end)
//     {
//         switch (*it)
//         {
//         case '<':
//             ++open;
//             break;
//         case '>':
//             if (--open == 0)
//             {
//                 ++it;
//                 return;
//             }
//             break;
//         }

//         ++it;
//     }
// }

// // void processTemplates(CSIt &it, const CSIt &end, const std::string &readOrWrite, Templates &templates)
// // {
// //     skipWs(it, end);
// //     std::string name;
// //     scanName(it, end, name);
// //     if (name.empty())
// //     {
// //         return;
// //     }

// //     auto c = *it;
// //     if (c == '<')
// //     {
// //         // TODO step forward until the final '>' and check if the complete type is already contained in templates.
// //         // If not, step into recursively.
// //         std::vector<std::string> innerTypes;
// //         auto start = it;
// //         processTemplates(it, end, readOrWrite, templates);
// //         // std::string typeName = name + '<' + std::string(start)
// //     }
// //     // auto functionBody = templates.functionBodyFor(name, )
// // }

// void process(const std::string &typeName, const std::vector<Variant> &variants, const std::vector<std::string> &templateParams)
// {
//     bool isUnion = variants.size() > 1;

//     if (templateParams.empty())
//     {
//         std::cout << "function write" << typeName << "(dout: DataOut, t: " << typeName << "): void {" << std::endl;
//     }
//     else
//     {
//         std::cout << "function genWrite" << typeName << "<";
//         auto it = templateParams.begin();
//         auto end = templateParams.end();
//         while (true)
//         {
//             std::cout << *it;
//             if (++it != end)
//             {
//                 std::cout << ", ";
//             }
//             else
//             {
//                 break;
//             }
//         }
//         std::cout << ">(";
//         it = templateParams.begin();
//         while (true)
//         {
//         }
//         std::cout << ") {\n";
//     }
//     if (isUnion)
//     {
//         std::cout << "    switch (t.type) {" << std::endl;
//         for (size_t i = 0; i < variants.size(); ++i)
//         {
//             std::cout << "        case \"" << variants[i].typeTag << "\":" << std::endl;
//             std::cout << "            dout.u8(" << i << ");" << std::endl;
//             for (const auto &f : variants[i].fields)
//                 printFieldWrite(f, "t." + f.name, "            ");
//             std::cout << "            break;" << std::endl;
//         }
//         std::cout << "    }" << std::endl;
//     }
//     else
//     {
//         for (const auto &f : variants[0].fields)
//             printFieldWrite(f, "t." + f.name, "    ");
//     }
//     std::cout << "}\n"
//               << std::endl;

//     std::cout << "function read" << typeName << "(din: DataIn): " << typeName << " {" << std::endl;
//     if (isUnion)
//     {
//         std::cout << "    const _tag = din.u8();\n    switch (_tag) {" << std::endl;
//         for (size_t i = 0; i < variants.size(); ++i)
//         {
//             std::cout << "        case " << i << ": return {" << std::endl;
//             std::cout << "            type: \"" << variants[i].typeTag << "\"," << std::endl;
//             for (const auto &f : variants[i].fields)
//             {
//                 if (f.baseType == "literal")
//                     continue;
//                 std::cout << "            " << f.name << ": " << getReadExpr(f) << "," << std::endl;
//             }
//             std::cout << "        };" << std::endl;
//         }
//         std::cout << "        default: throw new Error('Invalid tag');\n    }" << std::endl;
//     }
//     else
//     {
//         std::cout << "    return {" << std::endl;
//         for (const auto &f : variants[0].fields)
//         {
//             std::cout << "        " << f.name << ": " << getReadExpr(f) << "," << std::endl;
//         }
//         std::cout << "    };" << std::endl;
//     }
//     std::cout << "}\n"
//               << std::endl;
// }

// int KI_main()
// {
//     std::string input = readAllFromStdin();
//     std::regex nameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
//     std::smatch m;
//     std::string typeName = "T";
//     if (std::regex_search(input, m, nameRegex))
//         typeName = m.str(1);

//     std::vector<Variant> variants;
//     // input.find('{') != input.find('|') ist Unsinn :-/
//     if (input.find('|') != std::string::npos && input.find('{') != input.find('|'))
//     {
//         std::regex varRegex(R"(\{[\s\S]*?\})");
//         auto it = std::sregex_iterator(input.begin(), input.end(), varRegex);
//         for (; it != std::sregex_iterator(); ++it)
//         {
//             Variant v;
//             v.fields = parseFields(it->str());
//             for (auto &f : v.fields)
//                 if (f.baseType == "literal")
//                     v.typeTag = f.literalValue;
//             variants.push_back(v);
//         }
//     }
//     else
//     {
//         Variant v;
//         v.fields = parseFields(input);
//         variants.push_back(v);
//     }
//     process(typeName, variants);
//     return 0;
// }

void parseTemplates(const std::string& s, std::vector<std::string>& result)
{
    std::regex reg(R"([A-Za-z_][A-Za-z0-9_]*)");
    auto end = std::sregex_iterator();

    for (auto it = std::sregex_iterator(s.begin(), s.end(), reg); it != end; ++it)
    {
        auto match = *it;
        result.push_back(match.str());
    }
}

// int mainOld()
// {
//     std::string input1 = readAllFromStdin();
//     std::regex nameRegex(R"((type\s+([A-Za-z0-9_]+)(<[^>]*>)?\s*=)|(interface\s+([A-Za-z0-9_]+)(<[^>]*>)?\s*))"); /*R"(type\s+([A-Za-z0-9_]+)(<[^>]*>)?\s*=)*/

//     auto it = std::sregex_iterator(input1.begin(), input1.end(), nameRegex);
//     auto end = std::sregex_iterator();

//     for (; it != end;)
//     {
//         auto match = *it;
//         ++it;

//         auto curEnd = (it == end ? input1.size() : it->position());

//         std::string input(input1.substr(match.position(), curEnd));

//         {
//             std::regex nameRegex(R"((type\s+([A-Za-z0-9_]+)(<[^>]*>)?\s*=)|(interface\s+([A-Za-z0-9_]+)(<[^>]*>)?\s*))");
//             std::smatch m;
//             std::string typeName = "T";
//             if (std::regex_search(input, m, nameRegex))
//             {
//                 // std::cout << "**** str(2): '" << m.str(2) << "'\n";
//                 // std::cout << "**** str(3): '" << m.str(3) << "'\n";
//                 // std::cout << "**** str(4): '" << m.str(4) << "'\n";
//                 auto interface = m.str(5);
//                 if (interface == "")
//                 {
//                     typeName = m.str(2);
//                 }
//                 else
//                 {
//                     typeName = interface;
//                 }
//             }
//             auto templates = m.str(6);
//             std::vector<std::string> templateParams;

//             if (templates != "")
//             {
//                 parseTemplates(templates, templateParams);
//             }

//             std::vector<Variant> variants;
//             // input.find('{') != input.find('|') ist Unsinn :-/
//             if (input.find('|') != std::string::npos && input.find('{') != std::string::npos)
//             {
//                 std::regex varRegex(R"(\{[^{}]*\})");
//                 auto it = std::sregex_iterator(input.begin(), input.end(), varRegex);
//                 for (; it != std::sregex_iterator(); ++it)
//                 {
//                     Variant v;
//                     v.fields = parseFields(it->str());
//                     for (auto &f : v.fields)
//                         if (f.baseType == "literal")
//                             v.typeTag = f.literalValue;
//                     variants.push_back(v);
//                 }
//             }
//             else
//             {
//                 Variant v;
//                 v.fields = parseFields(input);
//                 variants.push_back(v);
//             }
//             process(typeName, variants, templateParams);
//         }
//     }

//     return 0;
// }

static void usage(const char* name)
{
    std::cerr << "usage: " << name << " <byte streams path> <import path>\n";
}

int main(int argc, const char* argv[])
{
    // void testExampleType();
    // testExampleType();
    bool testInput{ false }, testOutput{ false };
    bool parsedTestInput{};
    while (argc > 1 && ((parsedTestInput = !std::strcmp("-testInput", argv[1])) || !std::strcmp("-testOutput", argv[1]))) {
        if (parsedTestInput) testInput = true;
        else testOutput = true;
        --argc;
        ++argv;
    }
    // testInitializer();

    // // BEGIN test
    // {
    //     Literals l;
    //     std::string input = "'a' | 'b'|'c'";
    //     CSIt it = input.begin();
    //     CSIt end = input.end();
    //     l = Literals(it, end);
    //     std::cerr << "l.vec.size() " << l.vec.size() << "\n";
    //     Literals l2;
    //     std::cerr << "vor l = l2\n";
    //     l = l2;

    //     Literals l3;
    //     std::cerr << "vor l3 = std::move(l)\n";
    //     l3 = std::move(l);
    // }
    // // END test

    if (argc < 3) {
        usage(argv[0]);
        return -1;
    }

    std::string_view byteStreamsPath = argv[1];
    std::string_view importPath = argv[2];

    std::string input1 = testInput ? readAllFromFile("./testInput.txt") : readAllFromStdin();
    Imports imports;
    std::stringstream sFuncsOut;
    Output funcsOut(sFuncsOut);
    AllTypes allTypes;
    std::vector<const Type*> typesInOrder; // OK Zeiger auf Werte in allTypes aufzuheben, da bis zum Programmende nichts aus allTypes geloescht wird und die Werte per Definition einer std::map auch nach dem Einfügen weiterer Einträge an der gleichen Position im Speicher bleiben.

    try
    {
        CSIt it = input1.begin();
        CSIt end = input1.end();
        skipUntilBeginIOTypes(it, end);

        do
        {
            std::cerr << "before parsing type" << std::endl;
            Type type(it, end, imports);
            std::cerr << "name " << type.id.name << std::endl;
            auto emplaceResult = allTypes.emplace(std::string(type.id.name), std::move(type));
            typesInOrder.emplace_back(&emplaceResult.first->second); // Das passt, da Zeiger/Referenzen sowohl auf eingefügte Schlüssel wie Werte einer std::map garantiert stabil bleiben, bis das Element aus der map entfernt wird!
            if (emplaceResult.second) {
                std::cerr << "was inserted" << std::endl;
            }
            else {
                std::cerr << "was NOT inserted" << std::endl;
            }
        } while (true);

    }
    catch (const NoTypeFound& e) {
        std::cerr << e.st << std::endl;
        // std::cerr << "Caught NoTypeFound" << std::endl;
    }

    AllTypeCollections allTypeCollections(allTypes);

    std::cerr << "allTypes.size() " << allTypes.size() << std::endl;
    for (const auto& type1 : typesInOrder) {
        // std::cerr << "collecting for " << type.first << std::endl;
        // StrVec literals, typeValues;
        // Str error;
        // type.second.collect(allTypes, literals, typeValues, error);
        // type1->collect(allTypes, literals, typeValues, error);

        // {
        //     std::cerr << "Collected for ";
        //     // type.second.genFullType(std::cerr);
        //     type1->genFullType(std::cerr);
        //     std::cerr << ":\n";
        //     auto dumpVec = [&](const Str& literal) -> void
        //         {
        //             std::cerr << "  " << literal << "\n";
        //         };
        //     std::cerr << "Literals:\n";
        //     std::for_each(literals.begin(), literals.end(), dumpVec);
        //     std::cerr << "Type values:\n";
        //     std::for_each(typeValues.begin(), typeValues.end(), dumpVec);
        // }

        // std::cerr << "SKIP generating for " << type.first << std::endl;
        // type.second.genWriteVariant(funcsOut, imports, allTypes, literals, typeValues);

        std::vector<const TemplateArg*> variants;
        FuncGenerator funcGen;

        // 2. Vorbereitung der Analyse (Schritt 1)
        std::stack<const TemplateArg*> dfs;

        for (const auto& variant1 : type1->variants) {
            ObjectVariantPtr ov = std::dynamic_pointer_cast<ObjectVariant>(variant1);
            if (ov) {
                const ObjectVariant& variant = *ov;
                for (const auto& attribute : variant.attributes.vec) {
                    if (const auto& templateArg = attribute.type.baseType) {
                        dfs.push(templateArg.get());
                    }
                }
            }
            else {
                SynonymVariantPtr sv = std::dynamic_pointer_cast<SynonymVariant>(variant1);
                assert(sv);
                const auto& baseType = sv->synonym.baseType;
                if (baseType) {
                    variants.push_back(baseType.get());
                }
            }
        }

        // 3. Durchführung der Analyse (Schritt 2)
        funcGen.analyzeTemplates(dfs, &variants); // funcGen darf Zeiger auf variants behalten, da variants mindestens so lange lebt wie variants (beide auf dem Stack)

        TypeOrigins typeOrigins(*type1, allTypeCollections);
        type1->genWriteVariant(funcsOut, imports, allTypeCollections, funcGen, typeOrigins);
        type1->genReadVariant(funcsOut, imports, allTypeCollections, funcGen, typeOrigins);
        type1->genWrite(funcsOut, imports, allTypeCollections, funcGen, typeOrigins);
        type1->genRead(funcsOut, imports, allTypeCollections, funcGen, typeOrigins);
        funcsOut << "\n\n// genRndObj:\n";
        type1->genRndObj(funcsOut);
        // type.second.genRead(funcsOut, imports, allTypes);
    }

    // imports.base.genImportList(std::cout, byteStreamsPath);

    std::cerr << "\n\nAll local names:\n\n";
    for (const auto& localName : imports.localNames) {
        std::cerr << localName << " ";
    }

    std::cerr << std::endl;

    std::cerr << "\nAll forced types:\n\n";
    for (const auto& forcedType : imports.forced) {
        std::cerr << forcedType << " ";
    }

    std::cerr << std::endl;

    std::ostream* pout = &std::cout;
    std::shared_ptr<std::ofstream> testOutputStream;

    if (testOutput) {
        testOutputStream.reset(new std::ofstream("testOutput.ts"));
        pout = testOutputStream.get();
    }

    Output out(*pout);

    imports.other.genImportList(out, importPath, imports.localNames, imports.forced);
    // std::cout << "import * as _IM from " << importPath << ";\n\n";
    out << sFuncsOut.str();
    return 0;
}
