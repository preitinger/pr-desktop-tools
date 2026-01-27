#pragma once

#include "utils/utils.H"

class Output;

template <class T, class Format>
void format(Output& out, const Vec<T>& v, const Str& separator, Format format);


//
// TEMPLATE IMPLEMENTATIONS
//

#include "utils/Output.H"

template <class T, class Format>
void format(Output& out, const Vec<T>& v, const Str& separator, Format format) {
    bool first = true;
    
    for (const T& t : v) {
        if (first) {
            first = false;
        } else {
            out << separator;
        }
        format(out, t);
    }
}
