#ifndef __UTILS_H__
#define __UTILS_H__

#include "Indent.H"

#include <string>
#include <vector>
#include <exception>
#include <stacktrace>
#include <variant>

#include <cassert>

// erleichtert die Deklaration einer delegierenden Funktion in einer CRTP-Basisklasse:
#define DELEGATE_TO_DERIVED_CONST(func_name) \
    template<typename... Args> \
    auto func_name(Args&&... args) const { \
        return static_cast<const Derived*>(this)->func_name##Impl(std::forward<Args>(args)...); \
    }

#define DELEGATE_TO_DERIVED(func_name) \
    template<typename... Args> \
    auto func_name(Args&&... args) { \
        return static_cast<Derived*>(this)->func_name##Impl(std::forward<Args>(args)...); \
    }

// erleichtert die Deklaration einer delegierenden Funktion in einer von std::variant abgeleiteten Klasse.
#define DISPATCH(variant_obj, func_name, ...) \
    return std::visit(overloads { \
        [&](const auto& node) { return node.func_name(__VA_ARGS__); } \
    }, variant_obj)

#define CPOINTERS(ClassName) \
    using ClassName##SP = std::shared_ptr<ClassName>; \
    using ClassName##CSP = std::shared_ptr<const ClassName>; \
    using ClassName##UP = std::unique_ptr<ClassName>; \
    using ClassName##CUP = std::unique_ptr<const ClassName>; \
    using ClassName##WP = std::weak_ptr<ClassName>; \
    using ClassName##CWP = std::weak_ptr<const ClassName>

#define CDECL(ClassName) \
    class ClassName; \
    CPOINTERS(ClassName)

#define CVEC(TypeName) \
    using TypeName##Vec = std::vector<TypeName>

#define CVECS(ClassName) \
    CVEC(ClassName); \
    CVEC(ClassName##SP); \
    CVEC(ClassName##CSP); \
    CVEC(ClassName##UP); \
    CVEC(ClassName##CUP); \
    CVEC(ClassName##WP); \
    CVEC(ClassName##CWP)

#define CDECLVECS(ClassName) \
    CDECL(ClassName); \
    CVECS(ClassName)

#define TDECLVECS(TemplateArgs, ClassName) \
template TemplateArgs \
class ClassName; \
template TemplateArgs using TypeName##Vec = std::vector<TypeName TemplateArgs >


#define CMAP(Prefix, Key, Value) \
    using Prefix##Map = std::map<Key, Value>

#define CMAPS(Prefix, Key, Value) \
    CMAP(Prefix, Key, Value); \
    CMAP(Prefix##C, Key, const Value); \
    CMAP(Prefix##SP, Key, Value##SP); \
    CMAP(Prefix##CSP, Key, Value##CSP); \
    CMAP(Prefix##UP, Key, Value##UP); \
    CMAP(Prefix##WP, Key, Value##WP); \
    CMAP(Prefix##CWP, Key, Value##CWP)

using CSIt = std::string::const_iterator;
using Out = std::ostream;

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

template <class CSIt>
void skipImports(CSIt& it, const CSIt& end);

template <class CSIt>
void skipUntilBeginIOTypes(CSIt& it, const CSIt& end);

using Str = std::string;
using StrVec = std::vector<Str>;

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


#endif // __UTILS_H__
