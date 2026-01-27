#pragma once

#include "utils.H"
#include "StackTraceException.H"

class SRangeException : public StackTraceException {};

class SRange
{
private:
    CSIt it;
    const CSIt end;

public:
    using CharRanges = std::initializer_list<std::initializer_list<char>>;
    SRange(const CSIt& it, const CSIt& end) : it(it), end(end) {}
    ~SRange() {}

    void skipWs();
    /**
     * If at end, returns `0`.
     * Otherwise, returns next char and increments iterator.
     */
    char skipChar1();
    /**
     * @throws
     *      SRangeException, if it is at end or *it != c
     * 
     * checks, if `it` is not at end and points to `c` and increments `it`.
     */
    char skipChar(char c);
    /**
     * Each element in ranges must contain either 1 or 2 elements. If 1 element is contained, i.e. c, the range is equal to [c, c].
     * Otherwise, if 2 elements a and b are contained, the range is [a, b].
     */
    char skipChar(const CharRanges& ranges);
    std::string_view skipAll(const CharRanges& ranges);
    void skipOptionalKeyword(const Str& keyword);
    void skipKeyword(const Str& keyword);
    void skipUntil(const Str& toSearch);
    /**
     * @return
     *      0 if at end
     */
    char operator*() const;
    /**
     * If `it` is at end, returns `false`.
     * Otherwise, if `*it` is in one of the ranges in `ranges`, it returns `true`.
     */
    bool inAnyRange(const CharRanges& ranges) const;
    CSIt operator()() const { return it; }

private:
    void assertOrThrow(bool cond) const;
};
