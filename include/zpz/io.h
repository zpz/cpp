#ifndef _zpz_io_h_
#define _zpz_io_h_

#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <sstream>
#include <tuple>
#include <vector>


namespace zpz
{

template<typename T>
std::ostream & operator<<(std::ostream & os, std::vector<T> const & x)
{
    auto it = x.cbegin();
    if (it != x.cend()) {
        os << "[" << *it;
    } else {
        return os;
    }
    it++;
    while (it != x.cend())
    {
        os << ", " << *it;
        it++;
    }
    os << "]";
    return os;
}


template<typename S, typename T>
std::ostream & operator<<(std::ostream & os, std::pair<S, T> const & x)
{
    os << "<" << x.first << ", " << x.second << ">";
    return os;
}


template<typename K, typename V>
std::ostream & operator<<(std::ostream & os, std::map<K, V> const & x)
{
    os << "{";
    int n = 0;
    for (auto const & [k, v] : x) {
        if (n > 0) {
            os << ", ";
        }
        os << '"' << k << "\": \"" << v << "\"";
    }
    os << "}";
    return os;
}


template<typename K, typename V>
std::ostream & operator<<(std::ostream & os, std::unordered_map<K, V> const & x)
{
    os << "{";
    int n = 0;
    for (auto const & [k, v] : x) {
        if (n > 0) {
            os << ", ";
        }
        os << '"' << k << "\": \"" << v << "\"";
    }
    os << "}";
    return os;
}


std::string read_text_file(std::string const & filename)
{
    std::ifstream f(filename);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}


std::string read_binary_file(std::string const & filename) 
{
    auto text = read_text_file(filename);
    // auto v = std::vector<char>(text.begin(), text.end());
    // return std::string(v.cbegin(), v.cend());
    return text;
}


}  // namespace zpz
#endif  // _zpz_io_h_
