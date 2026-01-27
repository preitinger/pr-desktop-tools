#include "utils.H"

#include <fstream>


std::string readAllFromFile(std::string_view name) {
    std::ifstream sin(std::string(name), std::ios::in);
    return std::string(std::istreambuf_iterator<char>(sin), std::istreambuf_iterator<char>());
}
