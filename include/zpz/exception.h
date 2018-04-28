#ifndef _zpz_utilities_exception_h_
#define _zpz_utilities_exception_h_

// #include <exception>
#include <stdexcept>
#include <string>

namespace zpz
{

class Error : public std::runtime_error
{
  public:
    Error(std::string const& msg)
        : std::runtime_error(msg)
    {
    }
    Error(char const* msg)
        : std::runtime_error(msg)
    {
    }
};

const auto UNREACHABLE = Error("You should never see this!");
const auto NOTIMPLEMENTED = Error("not implemented yet");

} // namespace zpz

#endif // _zpz_utilities_exception_h_
