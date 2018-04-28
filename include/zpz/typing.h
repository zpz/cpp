#ifndef _zpz_utilities_typing_h_
#define _zpz_utilities_typing_h_

#include "exception.h"

#include <string>
#include <typeindex>
#include <typeinfo>


namespace zpz
{
template <typename T, typename S>
constexpr bool type_equals()
{
    return std::type_index(typeid(T)) == std::type_index(typeid(S));
}

template <typename T>
char const* type_name()
{
    if (type_equals<T, int>()) {
        return "int";
    }
    if (type_equals<T, long>()) {
        return "long";
    }
    if (type_equals<T, double>()) {
        return "double";
    }
    if (type_equals<T, float>()) {
        return "float";
    }
    if (type_equals<T, std::string>()) {
        return "string";
    }
    if (type_equals<T, bool>()) {
        return "bool";
    }
    throw NOTIMPLEMENTED;
}

} // namespace zpz
#endif // _zpz_utilities_typing_h_
