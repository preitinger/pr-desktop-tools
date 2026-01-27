#include "Type.H"

#include <iostream>

void testExampleType() {
    ExampleUtf utf;
    // auto& out = std::cout;
    Output out(std::cout);
    out << "\n\n\n***************************************************************\n\n\n";
    utf.genRndVal(out);
    ExampleNum u8("U8");
    out << "// U8\n";
    u8.genRndVal(out);
    out << "\n";
    ExampleNum i16("I16");
    out << "// I16\n";
    i16.genRndVal(out);
    out << "\n";
    i16.genRndVal(out);
    out << "\n\n\n";
 
}

