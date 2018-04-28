#include <experimental/string_view>
#include <string>
#include <cassert>
#include <iostream>


int main()
{
    std::string a = "a new world";
    std::string b = "another world";
    std::string_view va(&a.data()[6], 5);
    std::string_view vb(&b.data()[8], 5);
    std::cout << static_cast<void const *>(va.data()) << ": '" << va << "'" << std::endl;
    std::cout << static_cast<void const *>(vb.data()) << ": '" << vb << "'" << std::endl;

    // verifies that string_view equality is about the actual string value,
    // not the pointers.
    assert(static_cast<void const *>(va.data()) != static_cast<void const *>(vb.data()));
    assert(va == vb);
}



