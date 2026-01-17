#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>
#include <exception>
#include <stacktrace>

using CSIt = std::string::const_iterator;


class NoTypeFound : public std::exception {
public:
    NoTypeFound() : std::exception(),
    st(std::stacktrace::current())
    {

    }

    std::stacktrace st;
};

/**
 * throws NoTypeFound() if cond is false.
 */
void assertOrNoTypeFound(bool cond);

void skipWs(CSIt& it, const CSIt& end);

// CSIt& skipExport(CSIt& it, const CSIt& end);
// CSIt& skipType(CSIt& it, const CSIt& end);
CSIt& skipOptionalKeyword(CSIt& it, const CSIt& end, const std::string& keyword);
CSIt& skipKeyword(CSIt& it, const CSIt& end, const std::string& keyword);

std::string readAllFromStdin();

std::string readAllFromFile(std::string_view name);


inline bool startsWith(const std::string& s, const std::string& start)
{
    // inefficient:
    // auto startLen = start.length();
    // return s.length() >= startLen && s.substr(0, startLen) == start;

    // More efficient because start is not copied
    auto sIt = s.begin();
    auto sEnd = s.end();
    auto startIt = start.begin();
    auto startEnd = start.end();
    while (startIt != startEnd) {
        if (sIt == sEnd || *sIt != *startIt) return false;
        ++sIt;
        ++startIt;
    }
    return true;
}

void skipImports(CSIt& it, const CSIt& end);

void skipUntilBeginIOTypes(CSIt& it, const CSIt& end);

using Str = std::string;
using StrVec = std::vector<Str>;

#endif // __UTILS_H__
