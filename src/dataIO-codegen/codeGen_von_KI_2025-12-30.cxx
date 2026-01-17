#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iterator>

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
    // Erkennt: JSDoc, Name, Optional(?), Typ (inkl. Literale '...' oder "..." oder Unions), Kommentar
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
        
        // Literal-Check für 'WERT' ODER "WERT"
        std::regex litRegex(R"(['"]([^'"]+)['"])");
        std::smatch litMatch;
        if (std::regex_search(fullType, litMatch, litRegex)) {
            f.literalValue = litMatch.str(1);
            f.baseType = "literal";
        } else {
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
    
    if (f.canBeNull || f.canBeUndefined) {
        bool first = true;
        if (f.canBeNull) {
            std::cout << indent << "if (" << access << " === null) { dout.u8(1); }" << std::endl;
            first = false;
        }
        if (f.canBeUndefined) {
            std::cout << indent << (first ? "if" : "else if") << " (" << access << " === undefined) { dout.u8(2); }" << std::endl;
        }
        
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
        } else {
            if (f.baseType == "number") std::cout << subIndent << "dout." << f.numberMethod << "(" << access << ");" << std::endl;
            else if (f.baseType == "string") std::cout << subIndent << "dout.utf(" << access << ");" << std::endl;
            else if (f.baseType == "Buffer") std::cout << subIndent << "dout.bin(" << access << ");" << std::endl;
            else std::cout << subIndent << "write" << f.baseType << "(dout, " << access << ");" << std::endl;
        }
        std::cout << indent << "}" << std::endl;
    } 
    else if (f.isArray) {
        std::cout << indent << "dout.u32(" << access << ".length);" << std::endl;
        std::cout << indent << "for (const item of " << access << ") {" << std::endl;
        if (f.baseType == "number") std::cout << indent << "    dout." << f.numberMethod << "(item);" << std::endl;
        else if (f.baseType == "string") std::cout << indent << "    dout.utf(item);" << std::endl;
        else if (f.baseType == "Buffer") std::cout << indent << "    dout.bin(item);" << std::endl;
        else std::cout << indent << "    write" << f.baseType << "(dout, item);" << std::endl;
        std::cout << indent << "}" << std::endl;
    } else {
        if (f.baseType == "number") std::cout << indent << "dout." << f.numberMethod << "(" << access << ");" << std::endl;
        else if (f.baseType == "string") std::cout << indent << "dout.utf(" << access << ");" << std::endl;
        else if (f.baseType == "Buffer") std::cout << indent << "dout.bin(" << access << ");" << std::endl;
        else std::cout << indent << "write" << f.baseType << "(dout, " << access << ");" << std::endl;
    }
}

std::string getReadExpr(const Field& f) {
    std::string core;
    if (f.isArray) {
        std::string elem = (f.baseType == "number") ? "din." + f.numberMethod + "()" :
                           (f.baseType == "string") ? "din.utf()" :
                           (f.baseType == "Buffer") ? "din.bin()" : "read" + f.baseType + "(din)";
        core = "Array.from({ length: din.u32() }, () => " + elem + ")";
    } else {
        if (f.baseType == "number") core = "din." + f.numberMethod + "()";
        else if (f.baseType == "string") core = "din.utf()";
        else if (f.baseType == "Buffer") core = "din.bin()";
        else core = "read" + f.baseType + "(din)";
    }

    if (!f.canBeNull && !f.canBeUndefined) return core;

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
            std::cout << "        case \"" << variants[i].typeTag << "\":" << std::endl;
            std::cout << "            dout.u8(" << i << ");" << std::endl;
            for (const auto& f : variants[i].fields) printFieldWrite(f, "t." + f.name, "            ");
            std::cout << "            break;" << std::endl;
        }
        std::cout << "    }" << std::endl;
    } else {
        for (const auto& f : variants[0].fields) printFieldWrite(f, "t." + f.name, "    ");
    }
    std::cout << "}\n" << std::endl;

    std::cout << "function read" << typeName << "(din: DataIn): " << typeName << " {" << std::endl;
    if (isUnion) {
        std::cout << "    const _tag = din.u8();\n    switch (_tag) {" << std::endl;
        for (size_t i = 0; i < variants.size(); ++i) {
            std::cout << "        case " << i << ": return {" << std::endl;
            std::cout << "            type: \"" << variants[i].typeTag << "\"," << std::endl;
            for (const auto& f : variants[i].fields) {
                if (f.baseType == "literal") continue;
                std::cout << "            " << f.name << ": " << getReadExpr(f) << "," << std::endl;
            }
            std::cout << "        };" << std::endl;
        }
        std::cout << "        default: throw new Error('Invalid tag');\n    }" << std::endl;
    } else {
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
    } else {
        Variant v; v.fields = parseFields(input);
        variants.push_back(v);
    }
    process(typeName, variants);
    return 0;
}
