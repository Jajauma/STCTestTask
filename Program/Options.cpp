#include "Options.hpp"

#if defined(BUILD_UNIT_TESTS)
#include <gtest/gtest.h>
#endif /* BUILD_UNIT_TESTS */

#include <unistd.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>

#if defined(BUILD_UNIT_TESTS)
#include <sstream>
#endif /* BUILD_UNIT_TESTS */

using namespace Program;

namespace {
auto const MaxInt = std::numeric_limits<int>::max();
char MessageBuffer[4096];

int
parseThreadCount(char const* opt)
{
    char* end;
    long const ret{std::strtol(opt, &end, 10)};
    if (errno == ERANGE || end == opt || *end != '\0' || ret < 0
        || ret > MaxInt)
    {
        std::snprintf(MessageBuffer,
                      sizeof(MessageBuffer),
                      "Invalid thread count value '%s'",
                      opt);
        throw std::runtime_error{MessageBuffer};
    }
    return static_cast<int>(ret);
}
} /* namespace */

Options::Options(int argc, char* argv[])
{
    assert(argc > 0);

    ::opterr = 0;
    while (true)
    {
        auto const opt = ::getopt(argc, argv, ":hi:t:");
        if (opt == -1)
            break;

        switch (opt)
        {
        case 'h':
            std::fprintf(
                stderr,
                "Usage: %s -i <path> [-i <path> [...]] [-t <threads>]\n\n"
                "Options:\n"
                "  -i STRING\tpath to the supported image file, may be\n"
                "\t\tspecified multiple times\n"
                "  -t NUMBER\toptional non-negative thread count, perform\n"
                "\t\tauto-detection if set to 0 or not specified\n"
                "  -h\t\tprint this text and exit\n\n",
                argv[0]);
            std::exit(EXIT_FAILURE);
            break;

        case 'i':
            if (std::strlen(::optarg))
                imagePaths.push_back(::optarg);
            else
            {
                std::snprintf(MessageBuffer,
                              sizeof(MessageBuffer),
                              "Path to an image can't be empty");
                throw std::runtime_error{MessageBuffer};
            }
            break;

        case 't':
            try
            {
                threadCount = parseThreadCount(::optarg);
            }
            catch (std::exception& e)
            {
                std::snprintf(MessageBuffer,
                              sizeof(MessageBuffer),
                              "%s, try '%s -h' for help",
                              e.what(),
                              argv[0]);
                throw std::runtime_error{MessageBuffer};
            }
            break;

        case '?':
            std::snprintf(MessageBuffer,
                          sizeof(MessageBuffer),
                          "Unknown option '%s', try '%s -h' for help",
                          argv[optind - 1],
                          argv[0]);
            throw std::runtime_error{MessageBuffer};

        case ':':
            std::snprintf(
                MessageBuffer,
                sizeof(MessageBuffer),
                "Option '%s' requires an argument, try '%s -h' for help",
                argv[optind - 1],
                argv[0]);
            throw std::runtime_error{MessageBuffer};
        }
    }

    if (imagePaths.empty())
    {
        std::snprintf(MessageBuffer,
                      sizeof(MessageBuffer),
                      "Paths to the images are required, try '%s -h' for help",
                      argv[0]);
        throw std::runtime_error{MessageBuffer};
    }
}

#if defined(BUILD_UNIT_TESTS)
TEST(Options, ParseThreadCountThrows)
{
    EXPECT_THROW(parseThreadCount(""), std::runtime_error);
    EXPECT_THROW(parseThreadCount("xxx"), std::runtime_error);
    EXPECT_THROW(parseThreadCount("-1"), std::runtime_error);
    std::ostringstream os;
    os << MaxInt + 1U;
    EXPECT_THROW(parseThreadCount(os.str().c_str()), std::runtime_error);
}

TEST(Options, ParseThreadCountOK)
{
    EXPECT_EQ(parseThreadCount("0"), 0);
    EXPECT_EQ(parseThreadCount("1"), 1);
    EXPECT_EQ(parseThreadCount("16"), 16);
    std::ostringstream os;
    os << MaxInt;
    EXPECT_EQ(parseThreadCount(os.str().c_str()), MaxInt);
}
#endif /* BUILD_UNIT_TESTS */
