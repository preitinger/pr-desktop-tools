#include "utils.H"

#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>

void assertOrNoTypeFound(bool cond) {
    if (!cond) {
        throw NoTypeFound();
    }
}

// CSIt& skipExport(CSIt& it, const CSIt& end)
// {

//     skipWs(it, end);
//     if (it == end) return it;
//     assertChar(it, end, 'e');
//     assertChar(it, end, 'x');
//     assertChar(it, end, 'p');
//     assertChar(it, end, 'o');
//     assertChar(it, end, 'r');
//     assertChar(it, end, 't');
//     return it;
// }

// CSIt& skipType(CSIt& it, const CSIt& end)
// {
//     return it;
// }


std::string readAllFromStdin()
{
    return std::string(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>());
}

std::string readAllFromFile(std::string_view name) {
    std::ifstream sin(std::string(name), std::ios::in);
    return std::string(std::istreambuf_iterator<char>(sin), std::istreambuf_iterator<char>());
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


void StackTraceException::printStackTrace() const {
    std::cerr << this->st << std::endl;
}
