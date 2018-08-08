#if defined(BUILD_UNIT_TESTS)
#include <cstdlib>
#include <cstring>
#endif /* BUILD_UNIT_TESTS */

namespace Program {
#if defined(BUILD_UNIT_TESTS)
int mainRunUnitTests(int argc, char* argv[]);
#endif /* BUILD_UNIT_TESTS */
int mainProgram(int argc, char* argv[]);
} /* namespace Program */

int
main(int argc, char* argv[])
{
#if defined(BUILD_UNIT_TESTS)
    char const* self{std::getenv("SELF")};
    if (!self)
        return Program::mainProgram(argc, argv);
    else if (!std::strcmp(self, "RunUnitTests"))
        return Program::mainRunUnitTests(argc, argv);
#endif /* BUILD_UNIT_TESTS */

    return Program::mainProgram(argc, argv);
}
