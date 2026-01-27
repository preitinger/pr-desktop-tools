#include "templates.H"

#include <iostream>
#include <memory>

#include <cassert>

// using ParserIt = std::istreambuf_iterator<char, std::char_traits<char>>;

// void genWriteFunction(Output& out, const Type& type) {
//     std::visit(overloads{
//         [&](const auto& type1)
//         {
//             type1.genWriteFunction(out);
//         }
//         }, type);
// }

// void Type::genWriteFunction(Output& out) const {
//     DISPATCH(*this, genWriteFunctionImpl, out);
// }

// void Type::genReadFunction(Output& out) const {
//     DISPATCH(*this, genReadFunctionImpl, out);
// }


void Type::genType(Output& out) const {
    DISPATCH(*this, genTypeImpl, out);
}

// ValidType Type::validate(const TypeDeclMap& allTypeDecls) const {
//     DISPATCH(*this, validateImpl, allTypeDecls);
// }


// void Literal::genWriteFunctionImpl(Output& out) const {
//     out << "// Literal::genWriteFunctionImpl\n";
// }

// void Literal::genReadFunctionImpl(Output& out) const {
//     out << "// Literal::genReadFunctionImpl\n";
// }

// void Reference::genWriteFunctionImpl(Output& out) const {
//     out << "// Reference::genWriteFunctionImpl\n";
// }

// void Reference::genReadFunctionImpl(Output& out) const {
//     out << "// Reference::genReadFunctionImpl\n";
// }

void TypeDecl::genFuncs(Output& out, const TypeDeclMap& allDecls) const {
    out << out.indent << "// nyi: TypeDecl.genFuncs\n";
}

void Literal::genTypeImpl(Output& out) const {
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

void Reference::genTypeImpl(Output& out) const {
    out << this->name;
    if (!templateArgs.empty()) {
        out << "<";
        bool first = true;
        for (const auto& arg : templateArgs) {
            if (first) {
                first = false;
            }
            else {
                out << ", ";
            }
            arg.genType(out);
        }
        out << ">";
    }
}

void AttributeList::genTypeImpl(Output& out) const {
    out << "{ ";
    for (const auto& attribute : this->attributes) {
        out << attribute.name << ": ";
        attribute.type.genType(out);
        out << "; ";
    }
    out << " }";
}

void Array::genTypeImpl(Output& out) const {
    baseType->genType(out);
    out << "[]";
}

void Union::genTypeImpl(Output& out) const {
    bool first = true;
    for (const auto& type : this->alternatives) {
        if (first) {
            first = false;
        }
        else {
            out << " | ";
        }
        type->genType(out);
    }
}

Identifier& Identifier::operator=(Identifier&& other)
{
    name = std::move(other.name);
    return *this;
}

