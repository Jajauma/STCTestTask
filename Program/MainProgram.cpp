#include "Options.hpp"

#include <cstdio>
#include <cstdlib>
#include <exception>

namespace Program {
int
mainProgram(int argc, char* argv[])
{
    try
    {
        Options const programOptions{argc, argv};
        return EXIT_SUCCESS;
    }
    catch (std::exception& e)
    {
        std::fprintf(stderr, "Fatal error: %s\n", e.what());
        return EXIT_FAILURE;
    }
}
} /* namespace Program */
