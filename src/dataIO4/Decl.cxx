#include "Decl.H"
#include "utils/utils.H"
#include "utils/SRange.H"
#include "parse.H"
#include "TSAttributeList.H"

#include <iostream>

// Decl::Decl(SRange& r) {
//     r.skipWs();

//     switch (*r) {
//     case 't': // type
//         r.skipKeyword("type");
//         name = ms<const Str>(parseName(r));
//         r.skipKeyword("=");
//         this->exp = TypeExp::parse(r);
//         r.skipOptionalKeyword(";");
//         break;
//     case 'i': // interface
//         r.skipKeyword("interface");
//         std::cerr << "Skipped keyword interface\n";
//         this->name = ms<const Str>(parseName(r));
//         this->exp = ms<TSAttributeList>(r);
//         r.skipOptionalKeyword(";");
//         break;
//     default:
//         std::cerr << "No decl parsed\n";
//         break;
//     }
// }

