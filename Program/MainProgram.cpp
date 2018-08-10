#include "ImageProcessor.hpp"
#include "Options.hpp"

#include <omp.h>

#include <chrono>
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

                std::fprintf(stderr, "Processing %s ... ", path);
                std::fflush(stderr);

                auto const t0 = std::chrono::steady_clock::now();
                proc.run();
                std::chrono::duration<double, std::milli> const elapsed{
                    std::chrono::steady_clock::now() - t0};

                std::fprintf(stderr, "done in %.1f ms\n", elapsed.count());
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
