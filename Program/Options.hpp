#ifndef HEADER_08A54E3E_9C67_11E8_A678_74D435E5BEA6_INCLUDED
#define HEADER_08A54E3E_9C67_11E8_A678_74D435E5BEA6_INCLUDED

#include <vector>

namespace Program {
class Options
{
public:
    using ImagePaths = std::vector<char const*>;

    Options(int argc, char* argv[]);
    int getThreadCount() const { return threadCount; }
    ImagePaths const& getImagePaths() const { return imagePaths; }

private:
    int threadCount{0};
    ImagePaths imagePaths;
};
} /* namespace Program */

#endif /* end of the header guard */
