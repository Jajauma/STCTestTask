#ifndef HEADER_B143DB74_9C7F_11E8_A678_74D435E5BEA6_INCLUDED
#define HEADER_B143DB74_9C7F_11E8_A678_74D435E5BEA6_INCLUDED

#include <memory>
#include <ostream>

namespace Program {
class ImageProcessor
{
public:
    explicit ImageProcessor(char const* imagePath);
    ImageProcessor(ImageProcessor&&) = default;
    ImageProcessor& operator=(ImageProcessor&&) = default;
    ~ImageProcessor();

    void run();
    std::ostream& exportResults(std::ostream& os) const;

private:
    struct Implementation;
    std::unique_ptr<Implementation> impl;
};
} /* namespace Program  */

#endif /* end of the header guard */
