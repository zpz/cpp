#ifndef _zpz_utilities_string_h_
#define _zpz_utilities_string_h_

#include "io.h"

#include <random>
#include <string>
#include <vector>

namespace zpz
{

std::string random_string(std::size_t length)
{
    static auto& chrs = "0123456789"
                        "abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{ std::random_device{}() };
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string s;
    s.reserve(length);
    while (length--) {
        s += chrs[pick(rg)];
    }

    return s;
}


template <typename T>
std::string make_string(T x)
{
    std::stringstream ss;
    ss << x;
    return ss.str();
}

template <>
std::string make_string(std::string x)
{
    return x;
}

template <>
std::string make_string(std::string_view x)
{
    return std::string(x);
}

template <>
std::string make_string(char const* x)
{
    return std::string(x);
}

template <typename T, typename... Args>
std::string make_string(T const& x, Args... xs)
{
    return make_string(x) + make_string(xs...);
}

} // namespace zpz
#endif // _zpz_utilities_string_h_
