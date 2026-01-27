#include "SRange.H"

#include <cassert>


void SRange::skipWs() {
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

char SRange::skipChar1() {
    if (it == end) return 0;
    char c = *it;
    ++it;
    return c;
}


char SRange::skipChar(char c) {
    assertOrThrow(it != end && *it == c);
    ++it;
    return c;
}

bool SRange::inAnyRange(const SRange::CharRanges& ranges) const {
    if (it == end) return false;
    char c = *it;

    for (const auto& range : ranges) {
        size_t size = range.size();
        assert(size == 1 || size == 2);
        auto rit = range.begin();
        char first = *rit;
        ++rit;
        char last = size == 1 ? first : *rit;

        if (c >= first && c <= last) {
            return true;
        }
    }

    return false;
}

char SRange::skipChar(const CharRanges& ranges) {
    if (inAnyRange(ranges)) {
        char c = *it;
        ++it;
        return c;
    }
    
    throw SRangeException();
}


std::string_view SRange::skipAll(const CharRanges& ranges) {
    auto sbegin = it;

    while (inAnyRange(ranges)) {
        ++it;
    }

    return std::string_view(sbegin, it);
}


void SRange::skipOptionalKeyword(const Str& keyword) {
    assert(!keyword.empty());
    skipWs();
    if (it == end) return;
    if (*it != keyword.front()) return;

    for (char c : keyword) {
        skipChar(c);
    }
}

void SRange::skipKeyword(const Str& keyword) {
   assert(!keyword.empty());
    skipWs();

    for (char c : keyword) {
        skipChar(c);
    }
}

void SRange::skipUntil(const Str& toSearch) {
    it = std::search(it, end, toSearch.begin(), toSearch.end());

    if (it != end) it += toSearch.length();
}

void SRange::assertOrThrow(bool cond) const {
    if (!cond) {
        throw SRangeException();
    }
}


char SRange::operator*() const {
    return it == end ? 0 : *it;
}
