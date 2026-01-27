#pragma once

#include "utils.H"
#include <functional>
#include <ostream>

using Out = std::ostream;

class Indent
{
public:
    Indent();
    void inc();
    void dec();

    void sub(const std::function<void(/* Indent& */)>& f);
    Str toString() const;

private:
    Str s;

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

Str readAllFromFile(std::string_view name);


///////////////////////////////////////////////////////////////////////////////////////
// TEMPLATE IMPLEMENTATIONS
///////////////////////////////////////////////////////////////////////////////////////

template<class T>
Output& operator<<(Output& out, const T& t) {
    out.out << t;
    return out;
}
