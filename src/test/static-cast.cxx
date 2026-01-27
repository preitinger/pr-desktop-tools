#include <iostream>

auto& out = std::cout;

struct Base1 {
    int base1;
    void fBase1();
};

struct Base2 {
    int otherValue;
    int base2;
    void fBase2();
};

struct A : public Base1, public Base2 {
    int a;
    void fA() {
        out << "A::fA: Mein a: " << a << "\n";
    }
};

template<class T>
void dump(const std::string& label, T a) {
    out << label << a << "\n";
}

void Base1::fBase1() {
    out << "Base1::fBase1: Mein base1: " << base1 << "\n";
    A* a1 = (A*)this;
    dump("a1: ", a1);
    A* a2 = static_cast<A*>(this);
    dump("a2: ", a2);
    A* a3 = reinterpret_cast<A*>(this);
    dump("a3: ", a3);
    dump("this: ", this);
}

void Base2::fBase2() {
    out << "Base2::fBase2: Mein base2: " << base2 << "\n";
    A* a1 = (A*)this;
    dump("a1: ", a1);
    A* a2 = static_cast<A*>(this);
    dump("a2: ", a2);
    A* a3 = reinterpret_cast<A*>(this);
    dump("a3: ", a3);
    dump("this: ", this);
}


int main() {
    out << "Hello, static-cast\n";

    A a{{1}, {0, 2}, 3};
    a.fA();
    a.fBase1();
    a.fBase2();

    return 0;
}