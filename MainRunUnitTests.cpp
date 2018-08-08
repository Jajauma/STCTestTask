#include <gtest/gtest.h>

namespace Program {
int
mainRunUnitTests(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
} /* namespace Program */
