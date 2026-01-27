#include "TSTypeBase.H"
#include "Literal.H"
#include "Reference.H"
#include "AttributeList.H"
#include "Union.H"
#include "Array.H"
#include "AnyTSType.H"
#include "AnyTSType_Impl.H"

#include <iostream>
#include <vector>
#include <iterator>

static std::ostream& out = std::cout;

StrSP name(const char* s) {
    return std::make_shared<Str>(s);
}

int main() {

    // LiteralParser literalParser;
    // LiteralSP parsedLiteral = literalParser.parseStr("\"x\\\"\\\\\\'y\"");
    // out << "parsed literal: " << parsedLiteral->toString() << "\n";
    // LiteralSP parsedLiteral2 = literalParser.parseStr(R"('x"\\\'y')");
    // out << "2nd parsed literal: " << parsedLiteral2->toString() << "\n";
    
    // LiteralSP literal = Literal::createSP("b'\\\"la");

    // out << "My literal: " << literal->toString() << "\n";

    // AnyTSTypeSP anyTSType(literal);

    // out << "any ts type: " << anyTSType->toString() << "\n";

    // AnyTSTypeSPVec vec;
    // vec.push_back(anyTSType);
    // vec.push_back(Literal::createSP("bla2"));
    // AnyTSTypeSPVec templateArgs{ Reference::createSP(name("T")) };
    // vec.push_back(Reference::createSP(name("A"), AnyTSTypeSPVec{ Reference::createSP(name("T")) }/* std::move(templateArgs) */));
    // vec.push_back(Reference::createSP(std::make_shared<Str>("B")));
    // vec.push_back(AttributeList::createSP(AttributeCSPVec{ Attribute::createSP(name("type"), AnyTSTypeSP(Literal::createSP("a"))) }));
    // vec.push_back(Union::createSP(
    //     AnyTSTypeSPVec{
    //         Literal::createSP("x"),
    //         literal,
    //         Reference::createSP(
    //             name("Mixed"),
    //             AnyTSTypeSPVec{
    //                 Reference::createSP(name("X")),
    //                 Reference::createSP(name("Y"))
    //         })
    //     }
    // ));
    // vec.push_back(Array::createSP(
    //     Reference::createSP(name("A"), AnyTSTypeSPVec{ Reference::createSP(name("T")) }/* std::move(templateArgs) */)
    // ));

    // out << "All ts types in vec:\n";
    // for (const auto& x : vec) {
    //     out << x->toString() << "\n";
    // }
    // out << "\n\n";

    StrVec inputFiles;
    inputFiles.emplace_back("testInput.txt");
    LiteralParser literalParser;
    LiteralSPVec foundLiterals;
    literalParser.parseFiles(inputFiles.begin(), inputFiles.end(), std::back_insert_iterator(foundLiterals));
 
    out << "foundLiterals.size() " << foundLiterals.size() << "\n\n";
    
    return 0;
}