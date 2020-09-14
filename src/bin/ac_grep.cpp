#include "ac_search.hpp"

#include <cstdlib>
#include <iostream>

void usage(char* argv0)
{
    std::cerr << "usage: " << argv0 << " pattern_file [file ...]" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
