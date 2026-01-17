#ifndef __INDENT_H__
#define __INDENT_H__

#include <string>
#include <ostream>
#include <functional>

class Indent
{
public:
    Indent();
    void inc();
    void dec();

    void sub(const std::function<void(/* Indent& */)>& f);

private:
    std::string s;

    friend std::ostream& operator<<(std::ostream& out, const Indent& indent);
};

std::ostream& operator<<(std::ostream& out, const Indent& indent);

#endif //  __INDENT_H__