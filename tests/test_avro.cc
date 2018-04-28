#include "zpz/avro.h"

#include <iostream>

using namespace zpz;


int main(int argc, char const * const * argv)
{
    (void)argc;
    auto reader = AvroReader(argv[1]);

    std::cout << "coef:\n";
    reader.seek("/", "coef");
    std::cout << reader.get_vector<double>() << std::endl;
    std::cout << "\nintercept:\n";
    reader.seek("/", "intercept");
    std::cout << reader.get_scalar<double>() << std::endl;
}
