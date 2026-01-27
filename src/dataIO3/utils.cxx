#include "utils.H"

#include <iostream>
#include <fstream>

#include <cassert>

void genOptionalTemplateArgs(std::ostream& o, const StrCSPVec& args) {
    if (!args.empty()) {
        o << "<";
        bool first{ true };
        for (const auto& arg : args) {
            if (first) {
                first = false;
            }
            else {
                o << ", ";
            }
            o << *arg;
        }
        o << ">";
    }
}


Indent::Indent()
    : s()
{
}
void Indent::inc()
{
    s += "    ";
}

void Indent::dec()
{
    assert(s.length() >= 4);
    s = s.substr(0, s.length() - 4);
}

std::ostream &operator<<(std::ostream &out, const Indent &indent)
{
    return out << indent.s;
}

void Indent::sub(const std::function<void(/* Indent& */)>& f) {
    inc();
    f(/* *this */);
    dec();
}

std::string Indent::toString() const {
    return s;
}


Output::Output(Out& out)
    : out(out)
{
}

Output::~Output()
{
}

void Output::sub(const std::function<void()>& f) {
    indent.sub(f);
}

void assertOrNoTypeFound(bool cond) {
    if (!cond) {
        throw NoTypeFound();
    }
}

void StackTraceException::printStackTrace() const {
    std::cerr << this->st << std::endl;
}

std::string readAllFromFile(std::string_view name) {
    std::ifstream sin(std::string(name), std::ios::in);
    return std::string(std::istreambuf_iterator<char>(sin), std::istreambuf_iterator<char>());
}
