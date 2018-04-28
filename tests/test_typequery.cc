#include <cassert>
#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <string>


template<typename T, typename S>
bool type_is()
{
    bool z = (std::type_index(typeid(T)) == std::type_index(typeid(S)));
    return z;
}

template<typename T>
std::string get_type()
{
    if (type_is<T, int>()) {
        return "int";
    } else if (type_is<T, double>()) {
        return "double";
    } else if (type_is<T, std::string>()) {
        return "string";
    } else {
        return "unknown";
    }
}

int main()
{
    bool a = type_is<int, int>();
    assert(a);
    bool b = type_is<double, int>();
    assert(!b);

    assert(get_type<int>() == "int");
    assert(get_type<double>() == "double");
    assert(get_type<std::string>() == "string");
}


