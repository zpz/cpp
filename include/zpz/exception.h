#ifndef _zpz_utilities_exception_h_
#define _zpz_utilities_exception_h_

#include <exception>


namespace zpz
{


class ZpzError: public std::runtime_error
{
    public:
        ZpzError(string const & msg): std::runtime_error(msg) {}
        ZpzError(char const * msg): std::runtime_error(msg) {}
};


}  // namespace zpz

#endif  // _zpz_utilities_exception_h_
