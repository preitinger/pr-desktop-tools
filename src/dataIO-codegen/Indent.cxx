#include "Indent.H"
#include <cassert>

Indent::Indent()
    : s()
{
}
void Indent::inc()
{
    s += "    ";
}

void Indent::dec()
{
    assert(s.length() >= 4);
    s = s.substr(0, s.length() - 4);
}

std::ostream &operator<<(std::ostream &out, const Indent &indent)
{
    return out << indent.s;
}

void Indent::sub(const std::function<void(/* Indent& */)>& f) {
    inc();
    f(/* *this */);
    dec();
}
