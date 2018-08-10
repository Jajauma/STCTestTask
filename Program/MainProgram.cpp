#include "ImageProcessor.hpp"
#include "Options.hpp"

#include <omp.h>

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
        if (programOptions.getThreadCount() > 0)
            ::omp_set_num_threads(programOptions.getThreadCount());

        for (auto& path : programOptions.getImagePaths())
            try
            {
                ImageProcessor proc{path};
                proc.run();
            }
            catch (std::exception& e)
            {
                std::fprintf(stderr, "Error processing image: %s\n", e.what());
            }

        return EXIT_SUCCESS;
    }
    catch (std::exception& e)
    {
        std::fprintf(stderr, "Fatal error: %s\n", e.what());
        return EXIT_FAILURE;
    }
}
} /* namespace Program */
