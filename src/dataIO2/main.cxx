// #include "TypeDecl.H"
// #include "templates.H"

#include <memory>
#include <iostream>
#include <fstream>
#include <algorithm>

static auto& out = std::cout;

struct B {};
struct D : B { B b; };


// int main(int argc, const char* argv[]) {

//     // {
//     //     D d;
//     //     B& br1 = d;
//     //     B& br2 = d.b;

//     //     static_cast<D&>(br1); // OK, lvalue denoting the original “d” object
//     //     static_cast<D&>(br2); // UB: the “b” subobject is not a base class subobject
//     // }

//     {
//         // TypeDecl typeDecl(Str("Test"), StrVec{}, std::make_shared<AttributeList>());
//         Str testInput("type X<T> = {type: 'a';} | {type: 'b'; x: U8; y: I8[];z:Array<T> | Utf};");
//         CSIt it = testInput.begin();
//         CSIt end = testInput.end();
//         TypeDecl typeDecl = TypeDecl::parse(it, end);
//         AttributeList* attributeList = dynamic_cast<AttributeList*>(typeDecl.type.get());

//         out << "typeDecl.toString():\n" << typeDecl.toString() << std::endl;


//     }

//     {
//         Str testInput("type A<T> = B<Utf, T> | T[]");
//         CSIt it = testInput.begin();
//         CSIt end = testInput.end();
//         TypeDecl typeDecl = TypeDecl::parse(it, end);
//         Str utf("Utf");
//         CSIt utfIt = utf.begin();
//         CSIt utfEnd = utf.end();
//         TypeCSP bound = typeDecl.bindTemplateArgs(TypeCSPVec{ Type::parse(utfIt, utfEnd) });

//         out << "bound:\n" << bound->toString() << "\n";

//     }

//     {
//         Str inputA("type A<T> = B<Utf, T> | C<T, I8>");
//         Str inputB("type B<T, S> = {type: 'b1'; t: T} | {type: 'b2'; s: S}");
//         Str inputC("type C<T, S> = {type: 'c1'; t: T} | {type: 'c2'; s: S}");
//         TypeDeclCMap typeDecls;
//         TypeDecl typeDeclA = TypeDecl::parse(inputA);
//         out << "A am Anfang:\n" << typeDeclA.toString() << "\n";
//         auto insertedA = typeDecls.emplace(typeDeclA.name, std::move(typeDeclA));
//         TypeDecl typeDeclB = TypeDecl::parse(inputB);
//         auto insertedB = typeDecls.emplace(typeDeclB.name, std::move(typeDeclB));
//         TypeDecl typeDeclC = TypeDecl::parse(inputC);
//         auto insertedC = typeDecls.emplace(typeDeclC.name, std::move(typeDeclC));

//         const TypeDecl& refA = insertedA.first->second;
//         out << "A:\n" << refA.type->toString() << "\n";

//         ValidTypeCSP validType = refA.type->validate(typeDecls);
//         out << "validType:\n" << validType->toString() << "\n";
//     }

//     {
//         // Str input = R"(
//         // type A<T> = B<Utf, T> | C<T, I8>
//         // type B<T, S> = {type: 'b1'; t: T} | {type: 'b2'; s: S}
//         // type C<T, S> = {type: 'c1'; t: T} | {type: 'c2'; s: S}
//         // )";
//         Str input = R"(
//         type A = B<Utf, T> | C<T, I8>
//         type B<T, S> = {type: 'b1'; t: T} | {type: 'b2'; s: S}
//         type C<T, S> = {type: 'c1'; t: T} | {type: 'c2'; s: S}
//         )";
//         TypeDeclCMap typeDecls = TypeDecl::parseMap(input);
//         std::ofstream fout("dataIO2_testOutput.txt");
//         Output output(fout);
//         for (const auto& entry : typeDecls) {
//             out << "Parsed: " << entry.second.toString() << "\n";
//             auto validType = entry.second.type->validate(typeDecls);
//             out << entry.first << " validated: " << validType->toString() << "\n";
//             out << "\n\n\n*******************************************************\n\n\n";
//             GenExportFuncs{entry.second, typeDecls}.gen(output);
//             out << "\n\n\n*******************************************************\n\n\n";
//         }
//     }

//     // TypeSP type = std::make_shared<Literal>();
//     // Type* type2 = new Literal();
//     // delete type2;

//     return 0;
// }

// using ParserIt = CSIt;

// int main(int, const char* []) {
//     Output out(std::cout);

//     TypeDeclParser<ParserIt> parser;
//     TypeDeclVec foundTypeDecls;
//     std::vector<Str> files{ "testInput.txt" };
//     std::back_insert_iterator foundTypeDeclsOutIt(foundTypeDecls);
//     parser.parseFiles(files.begin(), files.end(), foundTypeDeclsOutIt);

//     out << "num of found type decls: " << foundTypeDecls.size() << "\n\n";

//     int underScores = countUnderScores(foundTypeDecls.begin(), foundTypeDecls.end());
//     out << "underScores " << underScores << "\n";
//     Str localPrefix = "";
//     for (int i = 0; i <= underScores; ++i) {
//         localPrefix += '_';
//     }
//     out << "localPrefix: '" << localPrefix << "'\n\n";



//     return 0;
// }

int main() {
    return 0;
}