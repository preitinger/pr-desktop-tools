#include "TestInitializer.H"

#include <iostream>
#include <vector>
#include <algorithm>

namespace ntestInitializer {

auto& o = std::cout;

class A {
public:
    // A();
    // A(std::initializer_list<int> l);
    A(int a, int b, std::string s) : v{ a, b }, s{s} {}
    void dump(std::ostream& o) const;

private:
    std::vector<int> v;
    std::string s;
};

// A::A(std::initializer_list<int> l) : v(l) {

// }

void A::dump(std::ostream& o) const {
    std::for_each(v.begin(), v.end(), [&o](int x) {
        o << x << "\n";
        });
    o << "s: " << s << "\n";
}

} // namespace testInitializer


void testInitializer() {
    using namespace ntestInitializer;

    o << "testInitializer\n";

    // A a{ 1, 2, 3 };
    A a{3, 2, "bla"};

    o << "dump a\n";
    a.dump(o);

    o << "\n\n";
    std::exit(0);
}
