#include "ImageProcessor.hpp"
#include "Options.hpp"

#if defined(BUILD_UNIT_TESTS)
#include <gtest/gtest.h>
#endif /* BUILD_UNIT_TESTS */

#include <omp.h>

#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>

namespace {
char const*
extractFileName(char const* path)
{
    assert(path != nullptr);
    auto const sep = std::strrchr(path, '/');
    if (!sep)
        return path;
    return sep + 1;
}
} /* namespace */

namespace Program {
int
mainProgram(int argc, char* argv[])
{
    try
    {
        Options const programOptions{argc, argv};

        int const threadCount{programOptions.getThreadCount()};
        if (threadCount > 0)
            ::omp_set_num_threads(threadCount);

        auto const imagePaths = programOptions.getImagePaths();
        for (auto i = 0U; i < imagePaths.size(); ++i)
            try
            {
                ImageProcessor imageProcessor{imagePaths[i]};

                std::fprintf(stderr, "Processing %s ... ", imagePaths[i]);
                std::fflush(stderr);

                auto const t0 = std::chrono::steady_clock::now();
                imageProcessor.run();
                std::chrono::duration<double, std::milli> const elapsed{
                    std::chrono::steady_clock::now() - t0};

                std::fprintf(stderr, "done in %.1f ms\n", elapsed.count());

                char fileName[1024];
                std::snprintf(fileName,
                              sizeof(fileName),
                              "%04u-%s.integral",
                              i,
                              extractFileName(imagePaths[i]));
                std::ofstream os{fileName, std::ios::trunc};
                if (!imageProcessor.exportResults(os))
                    std::fprintf(stderr, "Error writing file %s\n", fileName);
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

#if defined(BUILD_UNIT_TESTS)
TEST(MainProgram, ExtractFileName)
{
    EXPECT_STREQ(extractFileName(""), "");
    EXPECT_STREQ(extractFileName("xxx"), "xxx");
    EXPECT_STREQ(extractFileName("/zzz.1"), "zzz.1");
    EXPECT_STREQ(extractFileName("path/to/file.txt.gz"), "file.txt.gz");
    EXPECT_STREQ(extractFileName("a//b"), "b");
    EXPECT_STREQ(extractFileName("/"), "");
}
#endif /* BUILD_UNIT_TESTS */
