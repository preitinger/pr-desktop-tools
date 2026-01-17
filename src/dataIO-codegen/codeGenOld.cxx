#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iterator>
#include <fstream>

struct Field {
    std::string name;
    std::string baseType;
    std::string numberMethod = "u32";
    bool canBeNull = false;
    bool canBeUndefined = false;
    bool isArray = false;
    std::string literalValue = "";
};

struct Variant {
    std::string typeTag = "";
    std::vector<Field> fields;
};

std::string readAllFromStdin() {
    return std::string(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
}

std::vector<Field> parseFields(const std::string& block) {
    std::vector<Field> fields;
    std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)(\?|)\s*:\s*([^;}\n//]+)\s*;?\s*(?://\s*(\w+))?)");
    auto it = std::sregex_iterator(block.begin(), block.end(), fieldRegex);

    for (; it != std::sregex_iterator(); ++it) {
        std::smatch m = *it;
        Field f;
        f.name = m.str(2);
        std::string fullType = m.str(4);

        if (m.str(3) == "?") f.canBeUndefined = true;
        if (fullType.find("null") != std::string::npos) f.canBeNull = true;
        if (fullType.find("undefined") != std::string::npos) f.canBeUndefined = true;

        std::regex litRegex(R"('([^']+)')");
        std::smatch litMatch;
        if (std::regex_search(fullType, litMatch, litRegex)) {
            f.literalValue = litMatch.str(1);
            f.baseType = "literal";
        }
        else {
            f.isArray = (fullType.find("[]") != std::string::npos && fullType.find("Buffer") == std::string::npos);
            if (fullType.find("Buffer[]") != std::string::npos) f.isArray = true;

            std::regex baseRegex(R"((number|string|Buffer|[A-Z][A-Za-z0-9_]+))");
            std::smatch baseMatch;
            if (std::regex_search(fullType, baseMatch, baseRegex)) f.baseType = baseMatch.str(1);
        }

        std::string searchPool = m.str(1) + " " + m.str(5);
        std::regex numHint(R"((u8|i8|u16|i16|u32|i32))");
        std::smatch hintMatch;
        if (std::regex_search(searchPool, hintMatch, numHint)) f.numberMethod = hintMatch.str(1);

        fields.push_back(f);
    }
    return fields;
}

void printFieldWrite(const Field& f, const std::string& access, const std::string& indent) {
    if (f.baseType == "literal") return;

    // Wenn das Feld null oder undefined sein kann, generieren wir die Checks
    if (f.canBeNull || f.canBeUndefined) {
        bool first = true;
        if (f.canBeNull) {
            std::cout << indent << "if (" << access << " === null) { dout.u8(1); }" << std::endl;
            first = false;
        }
        if (f.canBeUndefined) {
            std::cout << indent << (first ? "if" : "else if") << " (" << access << " === undefined) { dout.u8(2); }" << std::endl;
        }

        // Im "else"-Zweig ist der Wert garantiert vorhanden. 
        // Wir rufen die Logik direkt auf, ohne rekursives Casting-Objekt.
        std::cout << indent << "else { dout.u8(0); " << std::endl;
        std::string subIndent = indent + "    ";

        if (f.isArray) {
            std::cout << subIndent << "dout.u32(" << access << ".length);" << std::endl;
            std::cout << subIndent << "for (const item of " << access << ") {" << std::endl;
            if (f.baseType == "number") std::cout << subIndent << "    dout." << f.numberMethod << "(item);" << std::endl;
            else if (f.baseType == "string") std::cout << subIndent << "    dout.utf(item);" << std::endl;
            else if (f.baseType == "Buffer") std::cout << subIndent << "    dout.bin(item);" << std::endl;
            else std::cout << subIndent << "    write" << f.baseType << "(dout, item);" << std::endl;
            std::cout << subIndent << "}" << std::endl;
        }
        else {
            if (f.baseType == "number") std::cout << subIndent << "dout." << f.numberMethod << "(" << access << ");" << std::endl;
            else if (f.baseType == "string") std::cout << subIndent << "dout.utf(" << access << ");" << std::endl;
            else if (f.baseType == "Buffer") std::cout << subIndent << "dout.bin(" << access << ");" << std::endl;
            else std::cout << subIndent << "write" << f.baseType << "(dout, " << access << ");" << std::endl;
        }
        std::cout << indent << "}" << std::endl;
    }
    // Fall für normale Felder (nicht null/undefined)
    else if (f.isArray) {
        std::cout << indent << "dout.u32(" << access << ".length);" << std::endl;
        std::cout << indent << "for (const item of " << access << ") {" << std::endl;
        if (f.baseType == "number") std::cout << indent << "    dout." << f.numberMethod << "(item);" << std::endl;
        else if (f.baseType == "string") std::cout << indent << "    dout.utf(item);" << std::endl;
        else if (f.baseType == "Buffer") std::cout << indent << "    dout.bin(item);" << std::endl;
        else std::cout << indent << "    write" << f.baseType << "(dout, item);" << std::endl;
        std::cout << indent << "}" << std::endl;
    }
    else {
        if (f.baseType == "number") std::cout << indent << "dout." << f.numberMethod << "(" << access << ");" << std::endl;
        else if (f.baseType == "string") std::cout << indent << "dout.utf(" << access << ");" << std::endl;
        else if (f.baseType == "Buffer") std::cout << indent << "dout.bin(" << access << ");" << std::endl;
        else std::cout << indent << "write" << f.baseType << "(dout, " << access << ");" << std::endl;
    }
}

// void printFieldWrite(const Field& f, const std::string& access, const std::string& indent) {
//     if (f.baseType == "literal") return;

//     if (f.canBeNull || f.canBeUndefined) {
//         bool first = true;
//         if (f.canBeNull) {
//             std::cout << indent << "if (" << access << " === null) { dout.u8(1); }" << std::endl;
//             first = false;
//         }
//         if (f.canBeUndefined) {
//             std::cout << indent << (first ? "if" : "else if") << " (" << access << " === undefined) { dout.u8(2); }" << std::endl;
//         }
//         std::cout << indent << "else { dout.u8(0); " << std::endl;
//         printFieldWrite({f.name, f.baseType, f.numberMethod, false, false, f.isArray}, "(" + access + " as any)", indent + "    ");
//         std::cout << indent << "}" << std::endl;
//     } else if (f.isArray) {
//         std::cout << indent << "dout.u32(" << access << ".length);" << std::endl;
//         std::cout << indent << "for (const item of " << access << ") {" << std::endl;
//         if (f.baseType == "number") std::cout << indent << "    dout." << f.numberMethod << "(item);" << std::endl;
//         else if (f.baseType == "string") std::cout << indent << "    dout.utf(item);" << std::endl;
//         else if (f.baseType == "Buffer") std::cout << indent << "    dout.bin(item);" << std::endl;
//         else std::cout << indent << "    write" << f.baseType << "(dout, item);" << std::endl;
//         std::cout << indent << "}" << std::endl;
//     } else {
//         if (f.baseType == "number") std::cout << indent << "dout." << f.numberMethod << "(" << access << ");" << std::endl;
//         else if (f.baseType == "string") std::cout << indent << "dout.utf(" << access << ");" << std::endl;
//         else if (f.baseType == "Buffer") std::cout << indent << "dout.bin(" << access << ");" << std::endl;
//         else std::cout << indent << "write" << f.baseType << "(dout, " << access << ");" << std::endl;
//     }
// }

// Generiert einen strikten Read-Ausdruck ohne überflüssige Zweige
std::string getReadExpr(const Field& f) {
    std::string core;
    if (f.isArray) {
        std::string elem = (f.baseType == "number") ? "din." + f.numberMethod + "()" :
            (f.baseType == "string") ? "din.utf()" :
            (f.baseType == "Buffer") ? "din.bin()" : "read" + f.baseType + "(din)";
        core = "Array.from({ length: din.u32() }, () => " + elem + ")";
    }
    else {
        if (f.baseType == "number") core = "din." + f.numberMethod + "()";
        else if (f.baseType == "string") core = "din.utf()";
        else if (f.baseType == "Buffer") core = "din.bin()";
        else core = "read" + f.baseType + "(din)";
    }

    if (!f.canBeNull && !f.canBeUndefined) return core;

    // Nur generieren, was im Typ auch wirklich vorhanden ist
    std::string res = "((tag = din.u8()) => ";
    if (f.canBeNull) res += "tag === 1 ? null : (";
    if (f.canBeUndefined) res += "tag === 2 ? undefined : (";
    res += core;
    if (f.canBeUndefined) res += ")";
    if (f.canBeNull) res += ")";
    res += ")()";
    return res;
}

void process(const std::string& typeName, const std::vector<Variant>& variants) {
    bool isUnion = variants.size() > 1;

    std::cout << "function write" << typeName << "(dout: DataOut, t: " << typeName << "): void {" << std::endl;
    if (isUnion) {
        std::cout << "    switch (t.type) {" << std::endl;
        for (size_t i = 0; i < variants.size(); ++i) {
            std::cout << "        case '" << variants[i].typeTag << "':" << std::endl;
            std::cout << "            dout.u8(" << i << ");" << std::endl;
            for (const auto& f : variants[i].fields) printFieldWrite(f, "t." + f.name, "            ");
            std::cout << "            break;" << std::endl;
        }
        std::cout << "    }" << std::endl;
    }
    else {
        for (const auto& f : variants[0].fields) printFieldWrite(f, "t." + f.name, "    ");
    }
    std::cout << "}\n" << std::endl;

    std::cout << "function read" << typeName << "(din: DataIn): " << typeName << " {" << std::endl;
    if (isUnion) {
        std::cout << "    const _tag = din.u8();\n    switch (_tag) {" << std::endl;
        for (size_t i = 0; i < variants.size(); ++i) {
            std::cout << "        case " << i << ": return {" << std::endl;
            std::cout << "            type: '" << variants[i].typeTag << "'," << std::endl;
            for (const auto& f : variants[i].fields) {
                if (f.baseType == "literal") continue;
                std::cout << "            " << f.name << ": " << getReadExpr(f) << "," << std::endl;
            }
            std::cout << "        };" << std::endl;
        }
        std::cout << "        default: throw new Error('Invalid tag');\n    }" << std::endl;
    }
    else {
        std::cout << "    return {" << std::endl;
        for (const auto& f : variants[0].fields) {
            std::cout << "        " << f.name << ": " << getReadExpr(f) << "," << std::endl;
        }
        std::cout << "    };" << std::endl;
    }
    std::cout << "}\n" << std::endl;
}

int main() {
    std::string input = readAllFromStdin();
    std::regex nameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
    std::smatch m;
    std::string typeName = "T";
    if (std::regex_search(input, m, nameRegex)) typeName = m.str(1);

    std::vector<Variant> variants;
    if (input.find('|') != std::string::npos && input.find('{') != input.find('|')) {
        std::regex varRegex(R"(\{[\s\S]*?\})");
        auto it = std::sregex_iterator(input.begin(), input.end(), varRegex);
        for (; it != std::sregex_iterator(); ++it) {
            Variant v; v.fields = parseFields(it->str());
            for (auto& f : v.fields) if (f.baseType == "literal") v.typeTag = f.literalValue;
            variants.push_back(v);
        }
    }
    else {
        Variant v; v.fields = parseFields(input);
        variants.push_back(v);
    }
    process(typeName, variants);
    return 0;
}


// #include <iostream>
// #include <string>
// #include <vector>
// #include <regex>
// #include <iterator>

// struct Field {
//     std::string name;
//     std::string baseType;
//     std::string numberMethod = "u32";
//     bool canBeNull = false;
//     bool canBeUndefined = false;
//     bool isArray = false;
//     std::string literalValue = ""; 
// };

// struct Variant {
//     std::string typeTag = "";
//     std::vector<Field> fields;
// };

// std::string readAllFromStdin() {
//     return std::string(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
// }

// std::vector<Field> parseFields(const std::string& block) {
//     std::vector<Field> fields;
//     std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)(\?|)\s*:\s*([^;}\n//]+)\s*;?\s*(?://\s*(\w+))?)");

//     auto it = std::sregex_iterator(block.begin(), block.end(), fieldRegex);
//     for (; it != std::sregex_iterator(); ++it) {
//         std::smatch m = *it;
//         Field f;
//         f.name = m[2].str();
//         std::string fullType = m[4].str();

//         if (m[3].str() == "?") f.canBeUndefined = true;
//         if (fullType.find("null") != std::string::npos) f.canBeNull = true;
//         if (fullType.find("undefined") != std::string::npos) f.canBeUndefined = true;

//         // Literal-Check (z.B. type: 'success')
//         std::regex litRegex(R"('([^']+)')");
//         std::smatch litMatch;
//         if (std::regex_search(fullType, litMatch, litRegex)) {
//             f.literalValue = litMatch[1].str();
//             f.baseType = "literal";
//         } else {
//             // Array-Check (erkennt Typ[] oder Array<Typ>)
//             f.isArray = (fullType.find("[]") != std::string::npos);

//             std::regex baseRegex(R"((number|string|Buffer|[A-Z][A-Za-z0-9_]+))");
//             std::smatch baseMatch;
//             if (std::regex_search(fullType, baseMatch, baseRegex)) f.baseType = baseMatch[1].str();
//         }

//         // Nummern-Formatierung
//         std::string jsDoc = m[1].str();
//         std::string inlineComm = m[5].str();
//         std::string searchPool = jsDoc + " " + inlineComm;
//         std::regex numHint(R"((u8|i8|u16|i16|u32|i32))");
//         std::smatch hintMatch;
//         if (std::regex_search(searchPool, hintMatch, numHint)) f.numberMethod = hintMatch[1].str();

//         fields.push_back(f);
//     }
//     return fields;
// }

// // Rekursive Schreib-Logik
// void printFieldWrite(const Field& f, const std::string& access, const std::string& indent) {
//     if (f.baseType == "literal") return;

//     if (f.canBeNull || f.canBeUndefined) {
//         bool first = true;
//         if (f.canBeNull) {
//             std::cout << indent << "if (" << access << " === null) { dout.u8(1); }" << std::endl;
//             first = false;
//         }
//         if (f.canBeUndefined) {
//             std::cout << indent << (first ? "if" : "else if") << " (" << access << " === undefined) { dout.u8(2); }" << std::endl;
//         }
//         std::cout << indent << "else { dout.u8(0); " << std::endl;
//         std::string cast = f.baseType + (f.isArray ? "[]" : "");
//         printFieldWrite({f.name, f.baseType, f.numberMethod, false, false, f.isArray}, "(" + access + " as " + cast + ")", indent + "    ");
//         std::cout << indent << "}" << std::endl;
//     } else if (f.isArray) {
//         std::cout << indent << "dout.u32(" << access << ".length);" << std::endl;
//         std::cout << indent << "for (const item of " << access << ") {" << std::endl;
//         if (f.baseType == "number") std::cout << indent << "    dout." << f.numberMethod << "(item);" << std::endl;
//         else if (f.baseType == "string") std::cout << indent << "    dout.utf(item);" << std::endl;
//         else if (f.baseType == "Buffer") std::cout << indent << "    dout.bin(item);" << std::endl;
//         else std::cout << indent << "    write" << f.baseType << "(dout, item);" << std::endl;
//         std::cout << indent << "}" << std::endl;
//     } else {
//         if (f.baseType == "number") std::cout << indent << "dout." << f.numberMethod << "(" << access << ");" << std::endl;
//         else if (f.baseType == "string") std::cout << indent << "dout.utf(" << access << ");" << std::endl;
//         else if (f.baseType == "Buffer") std::cout << indent << "dout.bin(" << access << ");" << std::endl;
//         else std::cout << indent << "write" << f.baseType << "(dout, " << access << ");" << std::endl;
//     }
// }

// // Hilfsfunktion für Read-Ausdrücke
// std::string getReadExpr(const Field& f) {
//     if (f.isArray) {
//         std::string elem = (f.baseType == "number") ? "din." + f.numberMethod + "()" :
//                            (f.baseType == "string") ? "din.utf()" :
//                            (f.baseType == "Buffer") ? "din.bin()" : "read" + f.baseType + "(din)";
//         return "Array.from({ length: din.u32() }, () => " + elem + ")";
//     }
//     if (f.baseType == "number") return "din." + f.numberMethod + "()";
//     if (f.baseType == "string") return "din.utf()";
//     if (f.baseType == "Buffer") return "din.bin()";
//     return "read" + f.baseType + "(din)";
// }

// void process(const std::string& typeName, const std::vector<Variant>& variants) {
//     bool isUnion = variants.size() > 1;

//     // --- WRITE ---
//     std::cout << "function write" << typeName << "(dout: DataOut, t: " << typeName << "): void {" << std::endl;
//     if (isUnion) {
//         std::cout << "    switch (t.type) {" << std::endl;
//         for (size_t i = 0; i < variants.size(); ++i) {
//             std::cout << "        case '" << variants[i].typeTag << "':" << std::endl;
//             std::cout << "            dout.u8(" << i << ");" << std::endl;
//             for (const auto& f : variants[i].fields) printFieldWrite(f, "t." + f.name, "            ");
//             std::cout << "            break;" << std::endl;
//         }
//         std::cout << "    }" << std::endl;
//     } else {
//         for (const auto& f : variants[0].fields) printFieldWrite(f, "t." + f.name, "    ");
//     }
//     std::cout << "}\n" << std::endl;

//     // --- READ ---
//     std::cout << "function read" << typeName << "(din: DataIn): " << typeName << " {" << std::endl;
//     if (isUnion) {
//         std::cout << "    const _tag = din.u8();" << std::endl;
//         std::cout << "    switch (_tag) {" << std::endl;
//         for (size_t i = 0; i < variants.size(); ++i) {
//             std::cout << "        case " << i << ": return {" << std::endl;
//             std::cout << "            type: '" << variants[i].typeTag << "'," << std::endl;
//             for (const auto& f : variants[i].fields) {
//                 if (f.baseType == "literal") continue;
//                 std::cout << "            " << f.name << ": ";
//                 if (f.canBeNull || f.canBeUndefined) {
//                     std::cout << "((tag = din.u8()) => tag === 1 ? null : (tag === 2 ? undefined : " << getReadExpr(f) << "))()," << std::endl;
//                 } else std::cout << getReadExpr(f) << "," << std::endl;
//             }
//             std::cout << "        };" << std::endl;
//         }
//         std::cout << "        default: throw new Error('Invalid tag');\n    }" << std::endl;
//     } else {
//         std::cout << "    return {" << std::endl;
//         for (const auto& f : variants[0].fields) {
//             std::cout << "        " << f.name << ": ";
//             if (f.canBeNull || f.canBeUndefined) {
//                 std::cout << "((tag = din.u8()) => tag === 1 ? null : (tag === 2 ? undefined : " << getReadExpr(f) << "))()," << std::endl;
//             } else std::cout << getReadExpr(f) << "," << std::endl;
//         }
//         std::cout << "    };" << std::endl;
//     }
//     std::cout << "}\n" << std::endl;
// }

// int main() {
//     std::string input = readAllFromStdin();
//     std::regex nameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
//     std::smatch m;
//     std::string typeName = "T";
//     if (std::regex_search(input, m, nameRegex)) typeName = m[1].str();

//     std::vector<Variant> variants;
//     if (input.find('|') != std::string::npos) {
//         std::regex varRegex(R"(\{[\s\S]*?\})");
//         auto it = std::sregex_iterator(input.begin(), input.end(), varRegex);
//         for (; it != std::sregex_iterator(); ++it) {
//             Variant v; v.fields = parseFields(it->str());
//             for (auto& f : v.fields) if (f.baseType == "literal") v.typeTag = f.literalValue;
//             variants.push_back(v);
//         }
//     } else {
//         Variant v; v.fields = parseFields(input);
//         variants.push_back(v);
//     }
//     process(typeName, variants);
//     return 0;
// }


// #include <iostream>
// #include <string>
// #include <vector>
// #include <regex>
// #include <iterator>

// struct Field {
//     std::string name;
//     std::string baseType;
//     std::string numberMethod = "u32";
//     bool canBeNull = false;
//     bool canBeUndefined = false;
//     bool isArray = false;
//     std::string literalValue = ""; 
// };

// struct Variant {
//     std::string typeTag = "";
//     std::vector<Field> fields;
// };

// std::string readAllFromStdin() {
//     return std::string(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
// }

// std::vector<Field> parseFields(const std::string& block) {
//     std::vector<Field> fields;
//     // Erkennt: JSDoc, Name, Optional(?), Typ (inkl. Literale '...' oder Unions), Kommentar
//     std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)(\?|)\s*:\s*([^;}\n//]+)\s*;?\s*(?://\s*(\w+))?)");

//     auto it = std::sregex_iterator(block.begin(), block.end(), fieldRegex);
//     for (; it != std::sregex_iterator(); ++it) {
//         std::smatch m = *it;
//         Field f;
//         f.name = m[2].str();
//         std::string fullType = m[4].str();

//         if (m[3].str() == "?") f.canBeUndefined = true;
//         if (fullType.find("null") != std::string::npos) f.canBeNull = true;
//         if (fullType.find("undefined") != std::string::npos) f.canBeUndefined = true;

//         // Prüfen auf String-Literal (z.B. type: 'success')
//         std::regex litRegex(R"('([^']+)')");
//         std::smatch litMatch;
//         if (std::regex_search(fullType, litMatch, litRegex)) {
//             f.literalValue = litMatch[1].str();
//             f.baseType = "literal";
//         } else {
//             f.isArray = (fullType.find("[]") != std::string::npos && fullType.find("Buffer") == std::string::npos);
//             std::regex baseRegex(R"((number|string|Buffer|[A-Z][A-Za-z0-9_]+))");
//             std::smatch baseMatch;
//             if (std::regex_search(fullType, baseMatch, baseRegex)) f.baseType = baseMatch.str();
//         }

//         std::string searchPool = m[1].str() + " " + m[5].str();
//         std::regex numHint(R"((u8|i8|u16|i16|u32|i32))");
//         std::smatch hintMatch;
//         if (std::regex_search(searchPool, hintMatch, numHint)) f.numberMethod = hintMatch.str();

//         fields.push_back(f);
//     }
//     return fields;
// }

// void printFieldWrite(const Field& f, const std::string& access, const std::string& indent) {
//     if (f.baseType == "literal") return;
//     std::string currentAccess = access;

//     if (f.canBeNull || f.canBeUndefined) {
//         bool first = true;
//         if (f.canBeNull) {
//             std::cout << indent << "if (" << access << " === null) { dout.u8(1); }" << std::endl;
//             first = false;
//         }
//         if (f.canBeUndefined) {
//             std::cout << indent << (first ? "if" : "else if") << " (" << access << " === undefined) { dout.u8(2); }" << std::endl;
//         }
//         std::cout << indent << "else { dout.u8(0); " << std::endl;
//         currentAccess = "( " + access + " as any )"; 
//         printFieldWrite({f.name, f.baseType, f.numberMethod, false, false, f.isArray}, currentAccess, indent + "    ");
//         std::cout << indent << "}" << std::endl;
//     } else if (f.isArray) {
//         std::cout << indent << "dout.u32(" << access << ".length);" << std::endl;
//         std::cout << indent << "for (const item of " << access << ") {" << std::endl;
//         if (f.baseType == "number") std::cout << indent << "    dout." << f.numberMethod << "(item);" << std::endl;
//         else if (f.baseType == "string") std::cout << indent << "    dout.utf(item);" << std::endl;
//         else if (f.baseType == "Buffer") std::cout << indent << "    dout.bin(item);" << std::endl;
//         else std::cout << indent << "    write" << f.baseType << "(dout, item);" << std::endl;
//         std::cout << indent << "}" << std::endl;
//     } else {
//         if (f.baseType == "number") std::cout << indent << "dout." << f.numberMethod << "(" << access << ");" << std::endl;
//         else if (f.baseType == "string") std::cout << indent << "dout.utf(" << access << ");" << std::endl;
//         else if (f.baseType == "Buffer") std::cout << indent << "dout.bin(" << access << ");" << std::endl;
//         else std::cout << indent << "write" << f.baseType << "(dout, " << access << ");" << std::endl;
//     }
// }

// std::string getFieldReadExpr(const Field& f) {
//     if (f.isArray) {
//         std::string elem = (f.baseType == "number") ? "din." + f.numberMethod + "()" :
//                            (f.baseType == "string") ? "din.utf()" :
//                            (f.baseType == "Buffer") ? "din.bin()" : "read" + f.baseType + "(din)";
//         return "Array.from({ length: din.u32() }, () => " + elem + ")";
//     }
//     if (f.baseType == "number") return "din." + f.numberMethod + "()";
//     if (f.baseType == "string") return "din.utf()";
//     if (f.baseType == "Buffer") return "din.bin()";
//     return "read" + f.baseType + "(din)";
// }

// void process(const std::string& typeName, const std::vector<Variant>& variants) {
//     bool isUnion = variants.size() > 1;

//     // --- WRITE ---
//     std::cout << "function write" << typeName << "(dout: DataOut, t: " << typeName << "): void {" << std::endl;
//     if (isUnion) {
//         std::cout << "    switch (t.type) {" << std::endl;
//         for (size_t i = 0; i < variants.size(); ++i) {
//             std::cout << "        case '" << variants[i].typeTag << "':" << std::endl;
//             std::cout << "            dout.u8(" << i << ");" << std::endl;
//             for (const auto& f : variants[i].fields) printFieldWrite(f, "t." + f.name, "            ");
//             std::cout << "            break;" << std::endl;
//         }
//         std::cout << "    }" << std::endl;
//     } else {
//         for (const auto& f : variants[0].fields) printFieldWrite(f, "t." + f.name, "    ");
//     }
//     std::cout << "}\n" << std::endl;

//     // --- READ ---
//     std::cout << "function read" << typeName << "(din: DataIn): " << typeName << " {" << std::endl;
//     if (isUnion) {
//         std::cout << "    const _tag = din.u8();" << std::endl;
//         std::cout << "    switch (_tag) {" << std::endl;
//         for (size_t i = 0; i < variants.size(); ++i) {
//             std::cout << "        case " << i << ": return {" << std::endl;
//             std::cout << "            type: '" << variants[i].typeTag << "'," << std::endl;
//             for (const auto& f : variants[i].fields) {
//                 if (f.baseType == "literal") continue;
//                 std::cout << "            " << f.name << ": ";
//                 if (f.canBeNull || f.canBeUndefined) {
//                     std::cout << "((tag = din.u8()) => tag === 1 ? null : (tag === 2 ? undefined : " << getFieldReadExpr(f) << "))()," << std::endl;
//                 } else std::cout << getFieldReadExpr(f) << "," << std::endl;
//             }
//             std::cout << "        };" << std::endl;
//         }
//         std::cout << "        default: throw new Error('Invalid tag');\n    }" << std::endl;
//     } else {
//         std::cout << "    return {" << std::endl;
//         for (const auto& f : variants[0].fields) {
//             std::cout << "        " << f.name << ": ";
//             if (f.canBeNull || f.canBeUndefined) {
//                 std::cout << "((tag = din.u8()) => tag === 1 ? null : (tag === 2 ? undefined : " << getFieldReadExpr(f) << "))()," << std::endl;
//             } else std::cout << getFieldReadExpr(f) << "," << std::endl;
//         }
//         std::cout << "    };" << std::endl;
//     }
//     std::cout << "}\n" << std::endl;
// }

// int main() {
//     std::string input = readAllFromStdin();
//     std::regex nameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
//     std::smatch m;
//     std::string typeName = "T";
//     if (std::regex_search(input, m, nameRegex)) typeName = m[1].str();

//     std::vector<Variant> variants;
//     if (input.find('|') != std::string::npos && input.find('{') != input.find('|')) {
//         std::regex varRegex(R"(\{[\s\S]*?\})");
//         auto it = std::sregex_iterator(input.begin(), input.end(), varRegex);
//         for (; it != std::sregex_iterator(); ++it) {
//             Variant v; v.fields = parseFields(it->str());
//             for (auto& f : v.fields) if (f.baseType == "literal") v.typeTag = f.literalValue;
//             variants.push_back(v);
//         }
//     } else {
//         Variant v; v.fields = parseFields(input);
//         variants.push_back(v);
//     }

//     process(typeName, variants);
//     return 0;
// }


// #include <iostream>
// #include <string>
// #include <vector>
// #include <regex>
// #include <iterator>

// struct Field {
//     std::string name;
//     std::string baseType;
//     std::string numberMethod = "u32";
//     bool canBeNull = false;
//     bool canBeUndefined = false;
//     bool isArray = false;
// };

// std::string readAllFromStdin() {
//     return std::string(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
// }

// std::vector<Field> parseFields(const std::string& input) {
//     std::vector<Field> fields;
//     std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)\s*:\s*([^;}\n//]+)\s*;?\s*(?://\s*(\w+))?)");
//     auto it = std::sregex_iterator(input.begin(), input.end(), fieldRegex);

//     for (; it != std::sregex_iterator(); ++it) {
//         std::smatch m = *it;
//         Field f;
//         f.name = m[2].str();
//         std::string fullType = m[3].str();

//         f.canBeNull = (fullType.find("null") != std::string::npos);
//         f.canBeUndefined = (fullType.find("undefined") != std::string::npos);
//         // Arrays erkennen, aber Buffer selbst ist kein Array im Sinne einer Schleife
//         f.isArray = (fullType.find("[]") != std::string::npos && fullType.find("Buffer") == std::string::npos);

//         // Basis-Typ finden
//         std::regex baseRegex(R"((number|string|Buffer|[A-Z][A-Za-z0-9_]+))");
//         std::smatch baseMatch;
//         if (std::regex_search(fullType, baseMatch, baseRegex)) f.baseType = baseMatch.str();

//         std::string jsDoc = m[1].str();
//         std::string inlineComm = m[4].str();
//         std::string searchPool = jsDoc + " " + inlineComm;
//         std::regex numHint(R"((u8|i8|u16|i16|u32|i32))");
//         std::smatch hintMatch;
//         if (std::regex_search(searchPool, hintMatch, numHint)) f.numberMethod = hintMatch.str();

//         fields.push_back(f);
//     }
//     return fields;
// }

// void printFunctions(const std::string& typeName, const std::vector<Field>& fields) {
//     // --- WRITE ---
//     std::cout << "function write" << typeName << "(dout: DataOut, t: " << typeName << "): void {" << std::endl;
//     for (const auto& f : fields) {
//         std::string access = "t." + f.name;

//         if (f.canBeNull || f.canBeUndefined) {
//             bool first = true;
//             if (f.canBeNull) {
//                 std::cout << "    if (" << access << " === null) { dout.u8(1); }" << std::endl;
//                 first = false;
//             }
//             if (f.canBeUndefined) {
//                 std::cout << "    " << (first ? "if" : "else if") << " (" << access << " === undefined) { dout.u8(2); }" << std::endl;
//             }
//             std::cout << "    else { dout.u8(0); " << std::endl;
//             std::string castType = f.baseType + (f.isArray ? "[]" : "");
//             access = "(" + access + " as " + castType + ")";
//         }

//         if (f.isArray) {
//             std::cout << "        dout.u32(" << access << ".length);" << std::endl;
//             std::cout << "        for (const item of " << access << ") {" << std::endl;
//             if (f.baseType == "number") std::cout << "            dout." << f.numberMethod << "(item);" << std::endl;
//             else if (f.baseType == "string") std::cout << "            dout.utf(item);" << std::endl;
//             else std::cout << "            write" << f.baseType << "(dout, item);" << std::endl;
//             std::cout << "        }" << std::endl;
//         } else {
//             std::cout << "        ";
//             if (f.baseType == "number") std::cout << "dout." << f.numberMethod << "(" << access << ");" << std::endl;
//             else if (f.baseType == "string") std::cout << "dout.utf(" << access << ");" << std::endl;
//             else if (f.baseType == "Buffer") std::cout << "dout.bin(" << access << ");" << std::endl;
//             else std::cout << "write" << f.baseType << "(dout, " << access << ");" << std::endl;
//         }

//         if (f.canBeNull || f.canBeUndefined) std::cout << "    }" << std::endl;
//     }
//     std::cout << "}\n" << std::endl;

//     // --- READ ---
//     std::cout << "function read" << typeName << "(din: DataIn): " << typeName << " {" << std::endl;
//     for (const auto& f : fields) {
//         std::string readVal;
//         if (f.isArray) {
//             std::string elementRead = (f.baseType == "number") ? "din." + f.numberMethod + "()" : 
//                                       (f.baseType == "string") ? "din.utf()" : "read" + f.baseType + "(din)";
//             readVal = "Array.from({ length: din.u32() }, () => " + elementRead + ")";
//         } else {
//             readVal = (f.baseType == "number") ? "din." + f.numberMethod + "()" : 
//                       (f.baseType == "string") ? "din.utf()" : 
//                       (f.baseType == "Buffer") ? "din.bin()" : "read" + f.baseType + "(din)";
//         }

//         if (f.canBeNull || f.canBeUndefined) {
//             std::cout << "    const _tag_" << f.name << " = din.u8();" << std::endl;
//             std::cout << "    const " << f.name << " = ";
//             if (f.canBeNull) std::cout << "_tag_" << f.name << " === 1 ? null : (";
//             if (f.canBeUndefined) std::cout << "_tag_" << f.name << " === 2 ? undefined : (";
//             std::cout << readVal;
//             if (f.canBeUndefined) std::cout << ")";
//             if (f.canBeNull) std::cout << ")";
//             std::cout << ";" << std::endl;
//         } else {
//             std::cout << "    const " << f.name << " = " << readVal << ";" << std::endl;
//         }
//     }
//     std::cout << "    return { ";
//     for (size_t i = 0; i < fields.size(); ++i) std::cout << fields[i].name << (i < fields.size() - 1 ? ", " : " ");
//     std::cout << "};\n}" << std::endl;
// }

// int main() {
//     std::string input = readAllFromStdin();
//     std::regex nameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
//     std::smatch m;
//     if (std::regex_search(input, m, nameRegex)) {
//         printFunctions(m[1].str(), parseFields(input));
//     }
//     return 0;
// }



// #include <iostream>
// #include <string>
// #include <vector>
// #include <regex>
// #include <iterator>

// struct Field {
//     std::string name;
//     std::string baseType;
//     std::string numberMethod = "u32";
//     bool canBeNull = false;
//     bool canBeUndefined = false;
//     bool isArray = false;
// };

// std::string readAllFromStdin() {
//     return std::string(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
// }

// std::vector<Field> parseFields(const std::string& input) {
//     std::vector<Field> fields;
//     std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)\s*:\s*([^;}\n//]+)\s*;?\s*(?://\s*(\w+))?)");
//     auto it = std::sregex_iterator(input.begin(), input.end(), fieldRegex);

//     for (; it != std::sregex_iterator(); ++it) {
//         std::smatch m = *it;
//         Field f;
//         f.name = m[2].str();
//         std::string fullType = m[3].str();

//         f.canBeNull = (fullType.find("null") != std::string::npos);
//         f.canBeUndefined = (fullType.find("undefined") != std::string::npos);
//         f.isArray = (fullType.find("[]") != std::string::npos);

//         // Basis-Typ ohne Array-Klammern und ohne Union-Partner finden
//         std::regex baseRegex(R"((number|string|[A-Z][A-Za-z0-9_]+))");
//         std::smatch baseMatch;
//         if (std::regex_search(fullType, baseMatch, baseRegex)) f.baseType = baseMatch.str();

//         std::string searchPool = m[1].str() + " " + m[4].str();
//         std::regex numHint(R"((u8|i8|u16|i16|u32|i32))");
//         std::smatch hintMatch;
//         if (std::regex_search(searchPool, hintMatch, numHint)) f.numberMethod = hintMatch.str();

//         fields.push_back(f);
//     }
//     return fields;
// }

// void printFunctions(const std::string& typeName, const std::vector<Field>& fields) {
//     // --- WRITE ---
//     std::cout << "function write" << typeName << "(dout: DataOut, t: " << typeName << "): void {" << std::endl;
//     for (const auto& f : fields) {
//         std::string access = "t." + f.name;

//         if (f.canBeNull || f.canBeUndefined) {
//             if (f.canBeNull) std::cout << "    if (" << access << " === null) { dout.u8(1); }" << std::endl;
//             if (f.canBeUndefined) std::cout << "    " << (f.canBeNull ? "else if" : "if") << " (" << access << " === undefined) { dout.u8(2); }" << std::endl;
//             std::cout << "    else { dout.u8(0); " << std::endl;
//             access = "(" + access + " as " + f.baseType + (f.isArray ? "[]" : "") + ")";
//         }

//         if (f.isArray) {
//             std::cout << "        dout.u32(" << access << ".length);" << std::endl;
//             std::cout << "        for (const item of " << access << ") {" << std::endl;
//             if (f.baseType == "number") std::cout << "            dout." << f.numberMethod << "(item);" << std::endl;
//             else if (f.baseType == "string") std::cout << "            dout.utf(item);" << std::endl;
//             else std::cout << "            write" << f.baseType << "(dout, item);" << std::endl;
//             std::cout << "        }" << std::endl;
//         } else {
//             std::cout << "        ";
//             if (f.baseType == "number") std::cout << "dout." << f.numberMethod << "(" << access << ");" << std::endl;
//             else if (f.baseType == "string") std::cout << "dout.utf(" << access << ");" << std::endl;
//             else std::cout << "write" << f.baseType << "(dout, " << access << ");" << std::endl;
//         }

//         if (f.canBeNull || f.canBeUndefined) std::cout << "    }" << std::endl;
//     }
//     std::cout << "}\n" << std::endl;

//     // --- READ ---
//     std::cout << "function read" << typeName << "(din: DataIn): " << typeName << " {" << std::endl;
//     for (const auto& f : fields) {
//         std::string readVal;
//         if (f.isArray) {
//             std::string elementRead = (f.baseType == "number") ? "din." + f.numberMethod + "()" : 
//                                       (f.baseType == "string") ? "din.utf()" : "read" + f.baseType + "(din)";
//             readVal = "Array.from({ length: din.u32() }, () => " + elementRead + ")";
//         } else {
//             readVal = (f.baseType == "number") ? "din." + f.numberMethod + "()" : 
//                       (f.baseType == "string") ? "din.utf()" : "read" + f.baseType + "(din)";
//         }

//         if (f.canBeNull || f.canBeUndefined) {
//             std::cout << "    const _tag_" << f.name << " = din.u8();" << std::endl;
//             std::cout << "    const " << f.name << " = ";
//             if (f.canBeNull) std::cout << "_tag_" << f.name << " === 1 ? null : (";
//             if (f.canBeUndefined) std::cout << "_tag_" << f.name << " === 2 ? undefined : (";
//             std::cout << readVal;
//             if (f.canBeUndefined) std::cout << ")";
//             if (f.canBeNull) std::cout << ")";
//             std::cout << ";" << std::endl;
//         } else {
//             std::cout << "    const " << f.name << " = " << readVal << ";" << std::endl;
//         }
//     }
//     std::cout << "    return { ";
//     for (size_t i = 0; i < fields.size(); ++i) std::cout << fields[i].name << (i < fields.size() - 1 ? ", " : " ");
//     std::cout << "};\n}" << std::endl;
// }

// int main() {
//     std::string input = readAllFromStdin();
//     std::regex nameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
//     std::smatch m;
//     if (std::regex_search(input, m, nameRegex)) {
//         printFunctions(m[1].str(), parseFields(input));
//     }
//     return 0;
// }



// #include <iostream>
// #include <string>
// #include <vector>
// #include <regex>
// #include <iterator>

// struct Field {
//     std::string name;
//     std::string baseType;
//     std::string numberMethod = "u32";
//     bool canBeNull = false;
//     bool canBeUndefined = false;
// };

// std::string readAllFromStdin() {
//     return std::string(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
// }

// std::vector<Field> parseFields(const std::string& input) {
//     std::vector<Field> fields;
//     std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)\s*:\s*([^;}\n//]+)\s*;?\s*(?://\s*(\w+))?)");
//     auto it = std::sregex_iterator(input.begin(), input.end(), fieldRegex);

//     for (; it != std::sregex_iterator(); ++it) {
//         std::smatch m = *it;
//         Field f;
//         f.name = m[2].str();
//         std::string fullType = m[3].str();

//         f.canBeNull = (fullType.find("null") != std::string::npos);
//         f.canBeUndefined = (fullType.find("undefined") != std::string::npos);

//         std::regex baseRegex(R"((number|string|[A-Z][A-Za-z0-9_]+))");
//         std::smatch baseMatch;
//         if (std::regex_search(fullType, baseMatch, baseRegex)) f.baseType = baseMatch.str();

//         std::string jsDoc = m[1].str();
//         std::string inlineComm = m[4].str();
//         std::string searchPool = jsDoc + " " + inlineComm;
//         std::regex numHint(R"((u8|i8|u16|i16|u32|i32))");
//         std::smatch hintMatch;
//         if (std::regex_search(searchPool, hintMatch, numHint)) f.numberMethod = hintMatch.str();

//         fields.push_back(f);
//     }
//     return fields;
// }

// void printFunctions(const std::string& typeName, const std::vector<Field>& fields) {
//     // --- WRITE ---
//     std::cout << "function write" << typeName << "(dout: DataOut, t: " << typeName << "): void {" << std::endl;
//     for (const auto& f : fields) {
//         std::string access = "t." + f.name;
//         if (f.canBeNull || f.canBeUndefined) {
//             bool first = true;
//             if (f.canBeNull) {
//                 std::cout << "    if (" << access << " === null) { dout.u8(1); }" << std::endl;
//                 first = false;
//             }
//             if (f.canBeUndefined) {
//                 std::cout << "    " << (first ? "if" : "else if") << " (" << access << " === undefined) { dout.u8(2); }" << std::endl;
//             }
//             std::cout << "    else { dout.u8(0); ";
//             if (f.baseType == "number") std::cout << "dout." << f.numberMethod << "(" << access << " as number); ";
//             else if (f.baseType == "string") std::cout << "dout.utf(" << access << " as string); ";
//             else std::cout << "write" << f.baseType << "(dout, " << access << " as " << f.baseType << "); ";
//             std::cout << "}" << std::endl;
//         } else {
//             if (f.baseType == "number") std::cout << "    dout." << f.numberMethod << "(" << access << ");" << std::endl;
//             else if (f.baseType == "string") std::cout << "    dout.utf(" << access << ");" << std::endl;
//             else std::cout << "    write" << f.baseType << "(dout, " << access << ");" << std::endl;
//         }
//     }
//     std::cout << "}\n" << std::endl;

//     // --- READ ---
//     std::cout << "function read" << typeName << "(din: DataIn): " << typeName << " {" << std::endl;
//     for (const auto& f : fields) {
//         if (f.canBeNull || f.canBeUndefined) {
//             std::cout << "    const _tag_" << f.name << " = din.u8();" << std::endl;
//             std::cout << "    const " << f.name << " = ";

//             // Generiere nur die Zweige, die im Typ tatsächlich erlaubt sind
//             if (f.canBeNull) std::cout << "_tag_" << f.name << " === 1 ? null : (";
//             if (f.canBeUndefined) std::cout << "_tag_" << f.name << " === 2 ? undefined : (";

//             // Der eigentliche Wert-Zweig (Tag 0)
//             if (f.baseType == "number") std::cout << "din." << f.numberMethod << "()";
//             else if (f.baseType == "string") std::cout << "din.utf()";
//             else std::cout << "read" << f.baseType << "(din)";

//             // Klammern schließen
//             if (f.canBeUndefined) std::cout << ")";
//             if (f.canBeNull) std::cout << ")";
//             std::cout << ";" << std::endl;
//         } else {
//             std::cout << "    const " << f.name << " = ";
//             if (f.baseType == "number") std::cout << "din." << f.numberMethod << "();" << std::endl;
//             else if (f.baseType == "string") std::cout << "din.utf();" << std::endl;
//             else std::cout << "read" << f.baseType << "(din);" << std::endl;
//         }
//     }
//     std::cout << "    return { ";
//     for (size_t i = 0; i < fields.size(); ++i) std::cout << fields[i].name << (i < fields.size() - 1 ? ", " : " ");
//     std::cout << "};\n}" << std::endl;
// }

// int main() {
//     std::string input = readAllFromStdin();
//     std::regex nameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
//     std::smatch m;
//     if (std::regex_search(input, m, nameRegex)) {
//         printFunctions(m[1].str(), parseFields(input));
//     }
//     return 0;
// }


// #include <iostream>
// #include <string>
// #include <vector>
// #include <regex>
// #include <iterator>

// struct Field {
//     std::string name;
//     std::string baseType;
//     std::string numberMethod = "u32";
//     bool canBeNull = false;
//     bool canBeUndefined = false;
// };

// std::string readAllFromStdin() {
//     return std::string(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
// }

// std::vector<Field> parseFields(const std::string& input) {
//     std::vector<Field> fields;
//     // Regex für JSDoc, Name, Typ-Union und Inline-Kommentar
//     std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)\s*:\s*([^;}\n//]+)\s*;?\s*(?://\s*(\w+))?)");
//     auto it = std::sregex_iterator(input.begin(), input.end(), fieldRegex);

//     for (; it != std::sregex_iterator(); ++it) {
//         std::smatch m = *it;
//         Field f;
//         f.name = m[2];
//         std::string fullType = m[3];

//         f.canBeNull = (fullType.find("null") != std::string::npos);
//         f.canBeUndefined = (fullType.find("undefined") != std::string::npos);

//         // Extrahiere den tatsächlichen Datentyp (nicht null/undefined)
//         std::regex baseRegex(R"((number|string|[A-Z][A-Za-z0-9_]+))");
//         std::smatch baseMatch;
//         if (std::regex_search(fullType, baseMatch, baseRegex)) f.baseType = baseMatch.str();

//         // Nummern-Präzision
//         std::string jsDoc = m[1], inlineComm = m[4];
//         std::regex numHint(R"((u8|i8|u16|i16|u32|i32))");
//         std::smatch hintMatch;
//         std::string bla = jsDoc + inlineComm;
//         if (std::regex_search(bla, hintMatch, numHint)) f.numberMethod = hintMatch.str();

//         fields.push_back(f);
//     }
//     return fields;
// }

// void printFunctions(const std::string& typeName, const std::vector<Field>& fields) {
//     // --- WRITE ---
//     std::cout << "function write" << typeName << "(dout: DataOut, t: " << typeName << "): void {" << std::endl;
//     for (const auto& f : fields) {
//         std::string access = "t." + f.name;
//         if (f.canBeNull || f.canBeUndefined) {
//             if (f.canBeNull) std::cout << "    if (" << access << " === null) { dout.u8(1); }" << std::endl;
//             if (f.canBeUndefined) std::cout << "    " << (f.canBeNull ? "else if" : "if") << " (" << access << " === undefined) { dout.u8(2); }" << std::endl;
//             std::cout << "    else { dout.u8(0); ";
//             if (f.baseType == "number") std::cout << "dout." << f.numberMethod << "(" << access << "); ";
//             else if (f.baseType == "string") std::cout << "dout.utf(" << access << "); ";
//             else std::cout << "write" << f.baseType << "(dout, " << access << "); ";
//             std::cout << "}" << std::endl;
//         } else {
//             if (f.baseType == "number") std::cout << "    dout." << f.numberMethod << "(" << access << ");" << std::endl;
//             else if (f.baseType == "string") std::cout << "    dout.utf(" << access << ");" << std::endl;
//             else std::cout << "    write" << f.baseType << "(dout, " << access << ");" << std::endl;
//         }
//     }
//     std::cout << "}\n" << std::endl;

//     // --- READ ---
//     std::cout << "function read" << typeName << "(din: DataIn): " << typeName << " {" << std::endl;
//     for (const auto& f : fields) {
//         if (f.canBeNull || f.canBeUndefined) {
//             std::cout << "    const _tag_" << f.name << " = din.u8();" << std::endl;
//             std::cout << "    const " << f.name << " = _tag_" << f.name << " === 1 ? null : (_tag_" << f.name << " === 2 ? undefined : ";
//             if (f.baseType == "number") std::cout << "din." << f.numberMethod << "());" << std::endl;
//             else if (f.baseType == "string") std::cout << "din.utf());" << std::endl;
//             else std::cout << "read" << f.baseType << "(din));" << std::endl;
//         } else {
//             std::cout << "    const " << f.name << " = ";
//             if (f.baseType == "number") std::cout << "din." << f.numberMethod << "();" << std::endl;
//             else if (f.baseType == "string") std::cout << "din.utf();" << std::endl;
//             else std::cout << "read" << f.baseType << "(din);" << std::endl;
//         }
//     }
//     std::cout << "    return { ";
//     for (size_t i = 0; i < fields.size(); ++i) std::cout << fields[i].name << (i < fields.size() - 1 ? ", " : " ");
//     std::cout << "};\n}" << std::endl;
// }

// int main() {
//     std::string input = readAllFromStdin();
//     std::regex nameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
//     std::smatch m;
//     if (std::regex_search(input, m, nameRegex)) {
//         printFunctions(m[1], parseFields(input));
//     }
//     return 0;
// }



// #include <iostream>
// #include <string>
// #include <vector>
// #include <regex>
// #include <iterator>
// #include <algorithm>

// struct Field {
//     std::string name;
//     std::string tsType;
//     std::string numberMethod = "u32";
//     bool isNullable = false; // Erkennt 'null' oder 'undefined'
// };

// struct TypeDef {
//     std::string name;
//     std::vector<Field> fields;
// };

// // Hilfsfunktion zum Einlesen von stdin
// std::string readAllFromStdin() {
//     return std::string(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
// }

// std::vector<Field> parseFields(const std::string& input) {
//     std::vector<Field> fields;
//     // Regex für: JSDoc, Name, Typ-Definition (inkl. Unions mit |), Inline-Kommentar
//     std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)\s*:\s*([^;}\n//]+)\s*;?\s*(?://\s*(\w+))?)");

//     auto words_begin = std::sregex_iterator(input.begin(), input.end(), fieldRegex);
//     auto words_end = std::sregex_iterator();

//     for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
//         std::smatch m = *i;
//         std::string fieldName = m[2].str();
//         std::string typeStr = m[3].str();
//         std::string jsDoc = m[1].str();
//         std::string inlineComment = m[4].str();

//         if (fieldName == "type") continue;

//         Field f;
//         f.name = fieldName;

//         // Prüfen auf null oder undefined in der Union
//         if (typeStr.find("undefined") != std::string::npos || typeStr.find("null") != std::string::npos) {
//             f.isNullable = true;
//         }

//         // Basis-Typ extrahieren (das was nicht null/undefined ist)
//         std::regex baseTypeRegex(R"((number|string|[A-Z][A-Za-z0-9_]+))");
//         std::smatch baseMatch;
//         if (std::regex_search(typeStr, baseMatch, baseTypeRegex)) {
//             f.tsType = baseMatch.str();
//         }

//         // Nummern-Methode bestimmen
//         f.numberMethod = "u32"; // default
//         std::string searchPool = jsDoc + " " + inlineComment;
//         std::regex numHint(R"((u8|i8|u16|i16|u32|i32))");
//         std::smatch hintMatch;
//         if (std::regex_search(searchPool, hintMatch, numHint)) {
//             f.numberMethod = hintMatch.str();
//         }

//         fields.push_back(f);
//     }
//     return fields;
// }

// void processInput(const std::string& input) {
//     TypeDef td;
//     std::regex nameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
//     std::smatch nameMatch;
//     if (std::regex_search(input, nameMatch, nameRegex)) td.name = nameMatch[1].str();

//     td.fields = parseFields(input);

//     // --- WRITE FUNCTION ---
//     std::cout << "function write" << td.name << "(dout: DataOut, t: " << td.name << "): void {" << std::endl;
//     for (const auto& f : td.fields) {
//         if (f.isNullable) {
//             std::cout << "    if (t." << f.name << " === null || t." << f.name << " === undefined) {" << std::endl;
//             std::cout << "        dout.u8(0);" << std::endl;
//             std::cout << "    } else {" << std::endl;
//             std::cout << "        dout.u8(1);" << std::endl;
//             std::string access = "t." + f.name;
//             if (f.tsType == "number") std::cout << "        dout." << f.numberMethod << "(" << access << ");" << std::endl;
//             else if (f.tsType == "string") std::cout << "        dout.utf(" << access << ");" << std::endl;
//             else std::cout << "        write" << f.tsType << "(dout, " << access << ");" << std::endl;
//             std::cout << "    }" << std::endl;
//         } else {
//             if (f.tsType == "number") std::cout << "    dout." << f.numberMethod << "(t." << f.name << ");" << std::endl;
//             else if (f.tsType == "string") std::cout << "    dout.utf(t." << f.name << ");" << std::endl;
//             else std::cout << "    write" << f.tsType << "(dout, t." << f.name << ");" << std::endl;
//         }
//     }
//     std::cout << "}\n" << std::endl;

//     // --- READ FUNCTION ---
//     std::cout << "function read" << td.name << "(din: DataIn): " << td.name << " {" << std::endl;
//     for (const auto& f : td.fields) {
//         if (f.isNullable) {
//             std::cout << "    const " << f.name << " = din.u8() === 0 ? undefined : ";
//             if (f.tsType == "number") std::cout << "din." << f.numberMethod << "();" << std::endl;
//             else if (f.tsType == "string") std::cout << "din.utf();" << std::endl;
//             else std::cout << "read" << f.tsType << "(din);" << std::endl;
//         } else {
//             std::cout << "    const " << f.name << " = ";
//             if (f.tsType == "number") std::cout << "din." << f.numberMethod << "();" << std::endl;
//             else if (f.tsType == "string") std::cout << "din.utf();" << std::endl;
//             else std::cout << "read" << f.tsType << "(din);" << std::endl;
//         }
//     }
//     std::cout << "    return { ";
//     for (size_t i = 0; i < td.fields.size(); ++i) {
//         std::cout << td.fields[i].name << (i < td.fields.size() - 1 ? ", " : "");
//     }
//     std::cout << " };\n}" << std::endl;
// }

// int main() {
//     std::string input = readAllFromStdin();
//     if (!input.empty()) processInput(input);
//     return 0;
// }



// #include <iostream>
// #include <string>
// #include <vector>
// #include <regex>
// #include <iterator>

// struct Field {
//     std::string name;
//     std::string tsType;
//     std::string numberMethod = "u32";
//     bool isArray = false;
//     bool isOptional = false;
//     std::string literalValue = ""; // Für 'type: a'
// };

// struct TypeVariant {
//     std::string typeTag = ""; // Falls vorhanden (für Unions)
//     std::vector<Field> fields;
// };

// struct TypeDef {
//     std::string name;
//     bool isUnion = false;
//     std::vector<TypeVariant> variants;
// };

// // Hilfsfunktion zum Parsen von Feldern aus einem Block { ... }
// std::vector<Field> parseFields(std::string block) {
//     std::vector<Field> fields;
//     // Regex erkennt: JSDoc, Name, Optional?, Typ (Literal oder Name), Array?, Semikolon, Inline-Kommentar
//     std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)(\?|)\s*:\s*(?:'([^']+)'|([A-Za-z0-9_\[\]]+))\s*;?\s*(?://\s*(\w+))?)");

//     auto it = std::sregex_iterator(block.begin(), block.end(), fieldRegex);
//     auto end = std::sregex_iterator();

//     for (; it != end; ++it) {
//         std::smatch m = *it;
//         Field f;
//         f.name = m[2];
//         f.isOptional = (m[3] == "?");

//         if (m[4].matched) { // Es ist ein String-Literal wie 'a'
//             f.literalValue = m[4];
//             f.tsType = "string_literal";
//         } else {
//             f.tsType = m[5];
//         }

//         if (f.tsType.find("[]") != std::string::npos) {
//             f.isArray = true;
//             f.tsType = f.tsType.substr(0, f.tsType.find("[]"));
//         }

//         // Typ-Hinweis aus JSDoc oder Inline
//         std::string jsDoc = m[1];
//         std::string inlineComm = m[6];
//         if (!inlineComm.empty()) f.numberMethod = inlineComm;
//         else if (jsDoc.find("u8") != std::string::npos) f.numberMethod = "u8";
//         else if (jsDoc.find("i8") != std::string::npos) f.numberMethod = "i8";
//         else if (jsDoc.find("u16") != std::string::npos) f.numberMethod = "u16";
//         else if (jsDoc.find("i16") != std::string::npos) f.numberMethod = "i16";
//         else if (jsDoc.find("i32") != std::string::npos) f.numberMethod = "i32";

//         fields.push_back(f);
//     }
//     return fields;
// }

// void printFieldWrite(const Field& f, std::string accessPrefix, std::string indent) {
//     std::string access = accessPrefix + f.name;
//     if (f.tsType == "string_literal") return; // Literale werden nicht geschrieben (sind implizit durch Diskriminator)

//     if (f.isOptional) {
//         std::cout << indent << "dout.u8(" << access << " !== undefined ? 1 : 0);" << std::endl;
//         std::cout << indent << "if (" << access << " !== undefined) {" << std::endl;
//         Field sub = f; sub.isOptional = false;
//         printFieldWrite(sub, accessPrefix, indent + "    ");
//         std::cout << indent << "}" << std::endl;
//     } else if (f.isArray) {
//         std::cout << indent << "dout.u32(" << access << ".length);" << std::endl;
//         std::cout << indent << "for (const item of " << access << ") {" << std::endl;
//         if (f.tsType == "number") std::cout << indent << "    dout." << f.numberMethod << "(item);" << std::endl;
//         else if (f.tsType == "string") std::cout << indent << "    dout.utf(item);" << std::endl;
//         else std::cout << indent << "    write" << f.tsType << "(dout, item);" << std::endl;
//         std::cout << indent << "}" << std::endl;
//     } else {
//         if (f.tsType == "number") std::cout << indent << "dout." << f.numberMethod << "(" << access << ");" << std::endl;
//         else if (f.tsType == "string") std::cout << indent << "dout.utf(" << access << ");" << std::endl;
//         else std::cout << indent << "write" << f.tsType << "(dout, " << access << ");" << std::endl;
//     }
// }

// void printGenerators(const TypeDef& td) {
//     // --- WRITE FUNCTION ---
//     std::cout << "function write" << td.name << "(dout: DataOut, t: " << td.name << ") {" << std::endl;
//     if (td.isUnion) {
//         std::cout << "    switch (t.type) {" << std::endl;
//         for (size_t i = 0; i < td.variants.size(); ++i) {
//             std::cout << "        case '" << td.variants[i].typeTag << "':" << std::endl;
//             std::cout << "            dout.u8(" << i << ");" << std::endl;
//             for (const auto& f : td.variants[i].fields) printFieldWrite(f, "t.", "            ");
//             std::cout << "            break;" << std::endl;
//         }
//         std::cout << "    }" << std::endl;
//     } else {
//         for (const auto& f : td.variants[0].fields) printFieldWrite(f, "t.", "    ");
//     }
//     std::cout << "}\n" << std::endl;

//     // --- READ FUNCTION ---
//     std::cout << "function read" << td.name << "(din: DataIn): " << td.name << " {" << std::endl;
//     if (td.isUnion) {
//         std::cout << "    const kind = din.u8();" << std::endl;
//         std::cout << "    switch (kind) {" << std::endl;
//         for (size_t i = 0; i < td.variants.size(); ++i) {
//             std::cout << "        case " << i << ": return {" << std::endl;
//             std::cout << "            type: '" << td.variants[i].typeTag << "'," << std::endl;
//             for (const auto& f : td.variants[i].fields) {
//                 if (f.tsType == "string_literal") continue;
//                 std::cout << "            " << f.name << ": ";
//                 if (f.isArray) {
//                     std::cout << "Array.from({length: din.u32()}, () => " << (f.tsType == "number" ? "din." + f.numberMethod + "()" : (f.tsType == "string" ? "din.utf()" : "read" + f.tsType + "(din)")) << ")," << std::endl;
//                 } else {
//                     if (f.tsType == "number") std::cout << "din." << f.numberMethod << "()," << std::endl;
//                     else if (f.tsType == "string") std::cout << "din.utf()," << std::endl;
//                     else std::cout << "read" << f.tsType << "(din)," << std::endl;
//                 }
//             }
//             std::cout << "        };" << std::endl;
//         }
//         std::cout << "        default: throw new Error('Invalid variant');" << std::endl;
//         std::cout << "    }" << std::endl;
//     } else {
//         std::cout << "    return {" << std::endl;
//         for (const auto& f : td.variants[0].fields) {
//             std::cout << "        " << f.name << ": ";
//             if (f.isArray) std::cout << "Array.from({length: din.u32()}, () => " << (f.tsType == "number" ? "din." + f.numberMethod + "()" : (f.tsType == "string" ? "din.utf()" : "read" + f.tsType + "(din)")) << ")," << std::endl;
//             else if (f.tsType == "number") std::cout << "din." << f.numberMethod << "()," << std::endl;
//             else if (f.tsType == "string") std::cout << "din.utf()," << std::endl;
//             else std::cout << "read" << f.tsType << "(din)," << std::endl;
//         }
//         std::cout << "    };" << std::endl;
//     }
//     std::cout << "}\n" << std::endl;
// }

// int main() {
//     std::string input = std::string(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
//     if(input.empty()) return 0;

//     TypeDef td;
//     std::regex nameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
//     std::smatch m;
//     if (std::regex_search(input, m, nameRegex)) td.name = m[1];

//     if (input.find('|') != std::string::npos) {
//         td.isUnion = true;
//         std::regex variantRegex(R"(\{[\s\S]*?\})");
//         auto it = std::sregex_iterator(input.begin(), input.end(), variantRegex);
//         for (; it != std::sregex_iterator(); ++it) {
//             TypeVariant tv;
//             tv.fields = parseFields(it->str());
//             for (auto& f : tv.fields) if (f.tsType == "string_literal") tv.typeTag = f.literalValue;
//             td.variants.push_back(tv);
//         }
//     } else {
//         TypeVariant tv;
//         tv.fields = parseFields(input);
//         td.variants.push_back(tv);
//     }

//     printGenerators(td);
//     return 0;
// }


// #include <iostream>
// #include <string>
// #include <vector>
// #include <regex>
// #include <iterator>
// #include <map>

// struct Field {
//     std::string name;
//     std::string tsType;
//     std::string numberMethod = "u32";
//     bool isArray = false;
//     bool isOptional = false;
//     std::string literalValue = ""; // Für 'type: a'
// };

// struct TypeVariant {
//     std::string typeTag; // z.B. "a", "b", "c"
//     std::vector<Field> fields;
// };

// struct TypeDef {
//     std::string name;
//     bool isUnion = false;
//     std::vector<TypeVariant> variants;
// };

// // Hilfsfunktion zum Parsen von Feldern aus einem Block { ... }
// std::vector<Field> parseFields(std::string block) {
//     std::vector<Field> fields;
//     std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)(\?|)\s*:\s*(?:'([^']+)'|([A-Za-z0-9_\[\]]+))\s*;?\s*(?://\s*(\w+))?)");

//     auto it = std::sregex_iterator(block.begin(), block.end(), fieldRegex);
//     auto end = std::sregex_iterator();

//     for (; it != end; ++it) {
//         std::smatch m = *it;
//         Field f;
//         f.name = m[2];
//         f.isOptional = (m[3] == "?");

//         if (m[4].matched) { // Literal-Typ wie 'a'
//             f.literalValue = m[4];
//             f.tsType = "string";
//         } else {
//             f.tsType = m[5];
//         }

//         if (f.tsType.find("[]") != std::string::npos) {
//             f.isArray = true;
//             f.tsType = f.tsType.substr(0, f.tsType.find("[]"));
//         }

//         // Typ-Hinweis
//         std::string jsDoc = m[1];
//         std::string inlineComm = m[6];
//         if (!inlineComm.empty()) f.numberMethod = inlineComm;
//         else if (jsDoc.find("u8") != std::string::npos) f.numberMethod = "u8";
//         else if (jsDoc.find("i8") != std::string::npos) f.numberMethod = "i8";
//         else if (jsDoc.find("u16") != std::string::npos) f.numberMethod = "u16";
//         else if (jsDoc.find("i16") != std::string::npos) f.numberMethod = "i16";
//         else if (jsDoc.find("i32") != std::string::npos) f.numberMethod = "i32";

//         fields.push_back(f);
//     }
//     return fields;
// }

// void printGenerators(const TypeDef& td) {
//     // --- WRITE FUNCTION ---
//     std::cout << "function write" << td.name << "(dout: DataOut, t: " << td.name << ") {" << std::endl;
//     if (td.isUnion) {
//         std::cout << "    switch (t.type) {" << std::endl;
//         for (size_t i = 0; i < td.variants.size(); ++i) {
//             std::cout << "        case '" << td.variants[i].typeTag << "':" << std::endl;
//             std::cout << "            dout.u8(" << i << "); // Discriminator" << std::endl;
//             for (const auto& f : td.variants[i].fields) {
//                 if (f.name == "type") continue;
//                 std::string access = "t." + f.name;
//                 if (f.isOptional) {
//                     std::cout << "            dout.u8(" << access << " !== undefined ? 1 : 0);" << std::endl;
//                     std::cout << "            if (" << access << " !== undefined) {" << std::endl;
//                     // (Innere Logik für Felder hier)
//                     std::cout << "            }" << std::endl;
//                 } else if (f.isArray) {
//                     std::cout << "            dout.u32(" << access << ".length);" << std::endl;
//                     std::cout << "            for (const item of " << access << ") {" << std::endl;
//                     std::cout << "                dout." << (f.tsType == "number" ? f.numberMethod : (f.tsType == "string" ? "utf" : "write" + f.tsType)) << "(item);" << std::endl;
//                     std::cout << "            }" << std::endl;
//                 } else {
//                     if (f.tsType == "number") std::cout << "            dout." << f.numberMethod << "(" << access << ");" << std::endl;
//                     else if (f.tsType == "string") std::cout << "            dout.utf(" << access << ");" << std::endl;
//                     else std::cout << "            write" << f.tsType << "(dout, " << access << ");" << std::endl;
//                 }
//             }
//             std::cout << "            break;" << std::endl;
//         }
//         std::cout << "    }" << std::endl;
//     }
//     std::cout << "}\n" << std::endl;

//     // --- READ FUNCTION ---
//     std::cout << "function read" << td.name << "(din: DataIn): " << td.name << " {" << std::endl;
//     if (td.isUnion) {
//         std::cout << "    const kind = din.u8();" << std::endl;
//         std::cout << "    switch (kind) {" << std::endl;
//         for (size_t i = 0; i < td.variants.size(); ++i) {
//             std::cout << "        case " << i << ": return {" << std::endl;
//             std::cout << "            type: '" << td.variants[i].typeTag << "'," << std::endl;
//             for (const auto& f : td.variants[i].fields) {
//                 if (f.name == "type") continue;
//                 std::cout << "            " << f.name << ": ";
//                 if (f.isArray) {
//                     std::cout << "Array.from({length: din.u32()}, () => " << (f.tsType == "number" ? "din." + f.numberMethod + "()" : "read" + f.tsType + "(din)") << ")," << std::endl;
//                 } else {
//                     if (f.tsType == "number") std::cout << "din." << f.numberMethod << "()," << std::endl;
//                     else if (f.tsType == "string") std::cout << "din.utf()," << std::endl;
//                     else std::cout << "read" << f.tsType << "(din)," << std::endl;
//                 }
//             }
//             std::cout << "        };" << std::endl;
//         }
//         std::cout << "        default: throw new Error('Unknown variant ' + kind);" << std::endl;
//         std::cout << "    }" << std::endl;
//     }
//     std::cout << "}\n" << std::endl;
// }

// int main() {
//     std::string input = std::string(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());

//     TypeDef td;
//     std::regex nameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
//     std::smatch m;
//     if (std::regex_search(input, m, nameRegex)) td.name = m[1];

//     if (input.find('|') != std::string::npos) {
//         td.isUnion = true;
//         std::regex variantRegex(R"(\{[\s\S]*?\})");
//         auto it = std::sregex_iterator(input.begin(), input.end(), variantRegex);
//         auto end = std::sregex_iterator();
//         for (; it != end; ++it) {
//             TypeVariant tv;
//             tv.fields = parseFields(it->str());
//             for (auto& f : tv.fields) if (f.name == "type") tv.typeTag = f.literalValue;
//             td.variants.push_back(tv);
//         }
//     } else {
//         TypeVariant tv;
//         tv.fields = parseFields(input);
//         td.variants.push_back(tv);
//     }

//     printGenerators(td);
//     return 0;
// }


// #include <iostream>
// #include <string>
// #include <vector>
// #include <regex>
// #include <iterator>

// struct Field {
//     std::string name;
//     std::string tsType;
//     std::string numberMethod;
// };

// // Hilfsfunktion zum Einlesen von stdin
// std::string readAllFromStdin() {
//     return std::string(std::istreambuf_iterator<char>(std::cin), 
//                        std::istreambuf_iterator<char>());
// }

// // Generiert die Schreib-Logik
// void printWriteFunction(const std::string& typeName, const std::vector<Field>& fields) {
//     std::cout << "function write" << typeName << "(dout: DataOut, t: " << typeName << "): void {" << std::endl;
//     for (const auto& field : fields) {
//         if (field.tsType == "string") {
//             std::cout << "    dout.utf(t." << field.name << ");" << std::endl;
//         } else if (field.tsType == "number") {
//             std::cout << "    dout." << field.numberMethod << "(t." << field.name << ");" << std::endl;
//         } else {
//             std::cout << "    write" << field.tsType << "(dout, t." << field.name << ");" << std::endl;
//         }
//     }
//     std::cout << "}\n" << std::endl;
// }

// // Generiert die Lese-Logik
// void printReadFunction(const std::string& typeName, const std::vector<Field>& fields) {
//     std::cout << "function read" << typeName << "(din: DataIn): " << typeName << " {" << std::endl;
//     for (const auto& field : fields) {
//         std::cout << "    const " << field.name << " = ";
//         if (field.tsType == "string") {
//             std::cout << "din.utf();" << std::endl;
//         } else if (field.tsType == "number") {
//             std::cout << "din." << field.numberMethod << "();" << std::endl;
//         } else {
//             std::cout << "read" << field.tsType << "(din);" << std::endl;
//         }
//     }
//     std::cout << "    return {" << std::endl;
//     for (size_t i = 0; i < fields.size(); ++i) {
//         std::cout << "        " << fields[i].name << (i < fields.size() - 1 ? "," : "") << std::endl;
//     }
//     std::cout << "    };" << std::endl;
//     std::cout << "}\n" << std::endl;
// }

// void processInput(const std::string& input) {
//     // 1. Typnamen extrahieren
//     std::regex typeNameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
//     std::smatch typeMatch;
//     std::string typeName = "Unknown";
//     if (std::regex_search(input, typeMatch, typeNameRegex)) {
//         typeName = typeMatch[1].str();
//     }

//     // 2. Felder extrahieren
//     std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)\s*:\s*([A-Za-z0-9_]+)\s*;?\s*(?://\s*(\w+))?)");
//     std::vector<Field> fields;

//     auto words_begin = std::sregex_iterator(input.begin(), input.end(), fieldRegex);
//     auto words_end = std::sregex_iterator();

//     for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
//         std::smatch m = *i;
//         std::string jsDoc = m[1].str();
//         std::string fieldName = m[2].str();
//         std::string tsType = m[3].str();
//         std::string inlineComment = m[4].str();

//         if (fieldName == "type" || fieldName == typeName) continue;

//         Field f;
//         f.name = fieldName;
//         f.tsType = tsType;
//         f.numberMethod = "u32"; // Default

//         // Typ-Hinweis aus Inline-Kommentar oder JSDoc suchen
//         if (!inlineComment.empty()) {
//             f.numberMethod = inlineComment;
//         } else if (!jsDoc.empty()) {
//             std::regex hintRegex(R"((u8|i8|u16|i16|u32|i32))");
//             std::smatch hintMatch;
//             if (std::regex_search(jsDoc, hintMatch, hintRegex)) {
//                 f.numberMethod = hintMatch.str();
//             }
//         }
//         fields.push_back(f);
//     }

//     // 3. Beide Funktionen generieren
//     std::cout << "/* GENERATED CODE FOR TYPE " << typeName << " */\n" << std::endl;
//     printWriteFunction(typeName, fields);
//     printReadFunction(typeName, fields);
// }

// int main() {
//     // Liest alles von stdin (Pipe oder manuelle Eingabe)
//     std::string input = readAllFromStdin();

//     if (input.empty()) {
//         std::cerr << "Fehler: Keine Eingabe erhalten." << std::endl;
//         return 1;
//     }

//     processInput(input);

//     return 0;
// }



// #include <iostream>
// #include <string>
// #include <vector>
// #include <regex>

// struct Field {
//     std::string name;           // Name des Attributs (z.B. "id")
//     std::string tsType;         // Typ-Name (z.B. "string", "number", "Z")
//     std::string numberMethod;   // Falls number: die Methode (z.B. "u8")
//     bool isElementary;          // true bei string/number
// };

// void generateWriteFunction(const std::string& typeName, const std::vector<Field>& fields) {
//     if (typeName.empty()) return;

//     std::cout << "function write" << typeName << "(dout: DataOut, t: " << typeName << ") {" << std::endl;
//     for (const auto& field : fields) {
//         if (field.tsType == "string") {
//             std::cout << "    dout.utf(t." << field.name << ");" << std::endl;
//         } else if (field.tsType == "number") {
//             std::cout << "    dout." << field.numberMethod << "(t." << field.name << ");" << std::endl;
//         } else {
//             std::cout << "    write" << field.tsType << "(dout, t." << field.name << ");" << std::endl;
//         }
//     }
//     std::cout << "}\n" << std::endl;
// }

// void processInput(const std::string& input) {
//     // 1. Regex für den Typnamen
//     std::regex typeNameRegex(R"(type\s+([A-Za-z0-9_]+)\s*=)");
//     std::smatch typeMatch;
//     std::string currentType;
//     if (std::regex_search(input, typeMatch, typeNameRegex)) {
//         currentType = typeMatch[1].str();
//     }

//     // 2. Regex für Felder
//     // Erklärt: 
//     // (/\*\*[\s\S]*?\*/)? -> Optionaler JSDoc Block
//     // ([A-Za-z0-9_]+)     -> Feldname
//     // \s*:\s*
//     // ([A-Za-z0-9_]+)     -> Typ
//     // \s*;?               -> Optionales Semikolon
//     // (?:\s*//\s*(\w+))?  -> Optionaler Inline-Kommentar
//     std::regex fieldRegex(R"((/\*\*[\s\S]*?\*/)?\s*([A-Za-z0-9_]+)\s*:\s*([A-Za-z0-9_]+)\s*;?\s*(?://\s*(\w+))?)");

//     std::vector<Field> fields;
//     auto words_begin = std::sregex_iterator(input.begin(), input.end(), fieldRegex);
//     auto words_end = std::sregex_iterator();

//     for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
//         std::smatch m = *i;

//         std::string jsDoc = m[1].str();
//         std::string fieldName = m[2].str();
//         std::string tsType = m[3].str();
//         std::string inlineComment = m[4].str();

//         // Wir ignorieren den Haupt-Typnamen selbst, falls er im Loop auftaucht
//         if (fieldName == "type" || fieldName == currentType) continue;

//         Field f;
//         f.name = fieldName;
//         f.tsType = tsType;
//         f.numberMethod = "u32"; // Default

//         // Prio 1: Inline Kommentar (// u8)
//         if (!inlineComment.empty()) {
//             f.numberMethod = inlineComment;
//         } 
//         // Prio 2: JSDoc Analyse (/** ... u8 ... */)
//         else if (!jsDoc.empty()) {
//             std::regex hintRegex(R"((u8|i8|u16|i16|u32|i32))");
//             std::smatch hintMatch;
//             if (std::regex_search(jsDoc, hintMatch, hintRegex)) {
//                 f.numberMethod = hintMatch[1].str();
//             }
//         }

//         fields.push_back(f);
//     }

//     generateWriteFunction(currentType, fields);
// }

// // void generateWriteFunction(const std::string& typeName, const std::vector<Field>& fields) {
// //     std::cout << "/** Automatisch generierte Schreibfunktion für " << typeName << " */" << std::endl;
// //     std::cout << "function write" << typeName << "(dout: DataOut, t: " << typeName << ") {" << std::endl;

// //     for (const auto& field : fields) {
// //         if (field.tsType == "string") {
// //             std::cout << "    dout.utf(t." << field.name << ");" << std::endl;
// //         } 
// //         else if (field.tsType == "number") {
// //             // Nutzt den Kommentar-Hinweis oder default u32
// //             std::cout << "    dout." << field.numberMethod << "(t." << field.name << ");" << std::endl;
// //         } 
// //         else {
// //             // Dynamische Generierung des Funktionsnamens: "write" + TypName
// //             std::cout << "    write" << field.tsType << "(dout, t." << field.name << ");" << std::endl;
// //         }
// //     }

// //     std::cout << "}" << std::endl;
// // }

// std::string readAllFromStdin() {
//     // Erzeugt einen String direkt aus den Iteratoren des Stream-Buffers.
//     // std::istreambuf_iterator<char>(std::cin) ist der Start (EOF wird automatisch erkannt).
//     // Der leere Konstruktor {} dient als "End-of-Stream" Iterator.
//     return std::string(std::istreambuf_iterator<char>(std::cin), 
//                        std::istreambuf_iterator<char>());
// }

// int main() {
//     // Beispiel-Eingabe mit verschiedenen Typen
//     // std::string input = R"(
//     //     type T = {
//     //         id: number; // u32
//     //         name: string;
//     //         header: MsgHeader;
//     //         payload: Blob;
//     //         count: number; // i16
//     //     }
//     // )";

//     auto input = readAllFromStdin();


//     processInput(input);
//     return 0;
// }
