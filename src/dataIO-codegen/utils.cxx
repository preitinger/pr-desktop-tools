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

void skipWs(CSIt& it, const CSIt& end)
{
    char c;

    do {
        if (it != end && ((c = *it) == ' ' || c == '\t' || c == '\r' || c == '\n'))
            ++it;
        else if (it != end && it + 1 != end) {
            if (*it == '/') {
                switch (it[1]) {
                case '/':
                    it += 2;
                    while (it != end && *it != '\n') ++it;
                    break;
                case '*':
                    it += 2;
                    while (it != end && it + 1 != end && !(*it == '*' && it[1] == '/')) ++it;
                    it += 2;
                    break;
                default:
                    return;
                }
            }
            else {
                return;
            }
        }
        else {
            return;
        }
    } while (true);
}

void assertChar(CSIt& it, const CSIt& end, char c) {
    assertOrNoTypeFound(it != end && *it == c);
    ++it;
}

CSIt& skipOptionalKeyword(CSIt& it, const CSIt& end, const std::string& keyword) {
    assert(!keyword.empty());
    skipWs(it, end);
    if (it == end) return it;
    if (*it != keyword.front()) return it;

    for (char c : keyword) {
        assertChar(it, end, c);
    }

    return it;
}

CSIt& skipKeyword(CSIt& it, const CSIt& end, const std::string& keyword)
{
    assert(!keyword.empty());
    skipWs(it, end);
    if (it == end) return it;

    for (char c : keyword) {
        assertChar(it, end, c);
    }

    return it;
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


void skipImports(CSIt& it, const CSIt& end)
{
    skipWs(it, end);

    while (it != end && startsWith(std::string(it, end), "import")) {
        while (it != end && *it != '\n') ++it;
        skipWs(it, end);
    }

}

void skipUntilBeginIOTypes(CSIt& it, const CSIt& end) {
    const std::string toSearch("// BEGIN IO TYPES");
    it = std::search(it, end, toSearch.begin(), toSearch.end());

    if (it != end) it += toSearch.length();
}

