#include "Type.H"
#include "utils.H"

#include <string>
#include <iostream>

int main()
{
    // std::string testInput(R"(
    // type ExY = {
    //     type: 'a'
    //     a: ExA<U8>;
    // } | {
    //     type: 'b'
    //     b: U8;
    //     c: ExA<ExY>;
    // };
    //     )");

    std::string testInput(R"(
    type ExY<Bla> = {
        type: 'a'
        a: ExA<U8[]>;
        name: Utf
        u8: U8
        i8: I8
        u16: U16
        i16: I16
        u32: U32
        i32: I32
        u32Array: U32[];
        blahs: Bla[]
    } | {
        type: 'b'
        b: U8;
        c: ExA<ExY<Utf>>;
        yOfUtf: ExY<Utf>;
    };
        )");

    testInput = "ty";

    // std::string testInput(R"(
    // type ExA<T> = {
    //     t: T;
    //     a: T[];
    //     name: Utf
    //     u8: U8
    //     i8: I8
    //     u16: U16
    //     i16: I16
    //     u32: U32
    //     i32: I32
    //     u32Array: U32[];
    // };
    //     )");

    CSIt it = testInput.begin();
    Type type(it, testInput.end());

    type.genWrite(std::cout);
    std::cout << "\n\n";
    type.genRead(std::cout);
    return 0;
}