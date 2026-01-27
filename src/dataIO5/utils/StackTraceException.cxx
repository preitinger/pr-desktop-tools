#include "StackTraceException.H"

#include <iostream>

void StackTraceException::printStackTrace() const {
    std::cerr << this->what() << "\n"
        << this->st << std::endl;
}
