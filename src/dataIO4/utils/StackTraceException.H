#pragma once

#include <exception>
#include <stacktrace>

class StackTraceException : public std::exception {
    public:
    std::stacktrace st;

    StackTraceException(): std::exception(), st(std::stacktrace::current()) {}
    void printStackTrace() const;
};
