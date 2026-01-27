#pragma once

#include "utils.H"

#include <string>
#include <vector>

template <class T, class Derived, class It>
class Parser {
public:
    template <class FileIt, class OutIt>
    inline void parseFiles(const FileIt& filesBegin, const FileIt& filesEnd, OutIt&& outIt) {
        parseFiles(filesBegin, filesEnd, outIt);
    }

    template <class FileIt, class OutIt>
    void parseFiles(const FileIt& filesBegin, const FileIt& filesEnd, OutIt& outIt);

    inline T parseStr(const Str& s) const {
        std::string::const_iterator it = s.begin();
        std::string::const_iterator end = s.end();
        return parse(it, end);
    }

    inline bool first(const It& it, const It& end) const {
        return it != end && firstChar(*it);
    }

    DELEGATE_TO_DERIVED_CONST(parse);
    DELEGATE_TO_DERIVED_CONST(firstChar);

};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEMPLATE IMPLEMENTATIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template <class T, class Derived, class It>
template <class FileIt, class OutIt>
void Parser<T, Derived, It>::parseFiles(const FileIt& filesBegin, const FileIt& filesEnd, OutIt& outIt) {

    for (FileIt filesIt = filesBegin; filesIt != filesEnd; ++filesIt) {
        // for (const auto& file : files) {
        const Str& file = *filesIt;
        Str input = readAllFromFile(file);
        It it = input.begin();
        It end = input.end();

        skipUntilBeginIOTypes(it, end);

        try {
            do {
                *outIt++ = parse(it, end);
            } while (true);
        }
        catch (const NoTypeFound& ex) {
            ex.printStackTrace();
        }
    }

}

