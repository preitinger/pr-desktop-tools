#pragma once

#include "macros.H"

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <stacktrace>

#include <cassert>

using Str = std::string;
using CSIt = std::string::const_iterator;

CPOINTERS(Str);
CVECS(Str);

using Out = std::ostream;

void genOptionalTemplateArgs(Out& o, const StrCSPVec& args);

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

class Indent
{
public:
    Indent();
    void inc();
    void dec();

    void sub(const std::function<void(/* Indent& */)>& f);
    std::string toString() const;

private:
    std::string s;

    friend std::ostream& operator<<(std::ostream& out, const Indent& indent);
};

Out& operator<<(Out& out, const Indent& indent);

class Output
{
private:
    Out& out;
public:
    Indent indent;

    Output(Out& out);
    ~Output();
    void sub(const std::function<void()>& f);

    template<class T>
    friend Output& operator<<(Output& out, const T&);
};

std::string readAllFromFile(std::string_view name);

class StackTraceException : public std::exception {
    public:
    std::stacktrace st;

    StackTraceException(): std::exception(), st(std::stacktrace::current()) {}
    void printStackTrace() const;
};

class NoTypeFound : public StackTraceException {
};

/**
 * throws NoTypeFound() if cond is false.
 */
void assertOrNoTypeFound(bool cond);

template <class CSIt>
void skipWs(CSIt& it, const CSIt& end);

// CSIt& skipExport(CSIt& it, const CSIt& end);
// CSIt& skipType(CSIt& it, const CSIt& end);

template <class CSIt>
CSIt& skipOptionalKeyword(CSIt& it, const CSIt& end, const std::string& keyword);

template <class CSIt>
CSIt& skipKeyword(CSIt& it, const CSIt& end, const std::string& keyword);


///////////////////////////////////////////////////////////////////////////////////////
// TEMPLATE IMPLEMENTATIONS
///////////////////////////////////////////////////////////////////////////////////////

template<class T>
Output& operator<<(Output& out, const T& t) {
    out.out << t;
    return out;
}

template<class CSIt>
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

template <class CSIt>
void assertChar(CSIt& it, const CSIt& end, char c) {
    assertOrNoTypeFound(it != end && *it == c);
    ++it;
}

template <class CSIt>
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

template <class CSIt>
CSIt& skipKeyword(CSIt& it, const CSIt& end, const std::string& keyword)
{
    assert(!keyword.empty());
    skipWs(it, end);

    for (char c : keyword) {
        assertChar(it, end, c);
    }

    return it;
}

template <class CSIt>
void skipImports(CSIt& it, const CSIt& end)
{
    skipWs(it, end);

    while (it != end && startsWith(std::string(it, end), "import")) {
        while (it != end && *it != '\n') ++it;
        skipWs(it, end);
    }

}

template <class CSIt>
void skipUntilBeginIOTypes(CSIt& it, const CSIt& end) {
    const std::string toSearch("// BEGIN IO TYPES");
    it = std::search(it, end, toSearch.begin(), toSearch.end());

    if (it != end) it += toSearch.length();
}
