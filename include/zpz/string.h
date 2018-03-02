#ifndef _zpz_utilities_string_h_
#define _zpz_utilities_string_h_

#include <random>
#include <string>

namespace zpz {

std::string random_string(std::size_t length)
{
    static auto& chrs = "0123456789"
                        "abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{ std::random_device{}() };
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string s;
    s.reserve(length);
    while (length--)
        s += chrs[pick(rg)];

    return s;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, vector<T> const& x)
{
    auto it = x.cbegin();
    if (it != x.cend()) {
        os << "[" << *it;
    } else {
        return os;
    }
    it++;
    while (it != x.cend()) {
        os << ", " << *it;
        it++;
    }
    os << "]";
    return os;
}

template <typename S, typename T>
std::ostream& operator<<(std::ostream& os, std::pair<S, T> const& x)
{
    os << "<" << x.first << ", " << x.second << ">";
    return os;
}

template <typename T>
string make_string(T x)
{
    std::stringstream ss;
    ss << x;
    return ss.str();
}

template <>
string make_string(string x)
{
    return x;
}

template <>
string make_string(string_view x)
{
    return string(x);
}

template <>
string make_string(char const* x)
{
    return string(x);
}

template <typename T, typename... Args>
string make_string(T const& x, Args... xs)
{
    return make_string(x) + make_string(xs...);
}

} // namespace zpz
#endif // _zpz_utilities_string_h_
