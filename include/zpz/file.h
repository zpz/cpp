#ifndef _zpz_utilities_file_h_
#define _zpz_utilities_file_h_

#include "exception"
#include "string.h"

#include <sys/stat.h>
#include <fstream>


namespace zpz
{

inline bool file_exists(char const * filename)
{
    std::ifstream infile(filename);
    return infile.good();
}


inline bool file_exists(std::string const & filename)
{
    return file_exists(filename.c_str());
}


inline bool dir_exists(char const * dirname)
{
    struct stat sb;
    return (stat(dirname, &sb) == 0 && S_ISDIR(sb.st_mode));
}

inline bool dir_exists(std::string const & dirname)
{
    return dir_exists(dirname.c_str());
}


void check_file_exists(char const * filename)
{
    if (!file_exists(filename)) {
        throw Error(make_string(
                        "could not find file '",
                        filename,
                        "'"
                    ));
    }
}

void check_file_exists(std::string const & filename)
{
    check_file_exists(filename.c_str());
}


void check_dir_exists(char const * dirname)
{
    if (!dir_exists(dirname)) {
        throw Error(make_string(
                        "could not find directory '",
                        dirname,
                        "'"
                    ));
    }
}


void check_dir_exists(std::string const & dirname)
{
    check_dir_exists(dirname.c_str());
}


}

#endif