#include "ImageProcessor.hpp"

#if defined(BUILD_UNIT_TESTS)
#include <gtest/gtest.h>
#endif /* BUILD_UNIT_TESTS */

#include <opencv2/opencv.hpp>

#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <vector>

#if defined(BUILD_UNIT_TESTS)
#include <type_traits>
#endif /* BUILD_UNIT_TESTS */

using namespace Program;

namespace {
char MessageBuffer[4096];
} /* namespace */

struct ImageProcessor::Implementation
{
    std::vector<cv::Mat> imageChannels;
};

ImageProcessor::ImageProcessor(char const* imagePath)
    : impl{new Implementation}
{
    assert(imagePath != nullptr);

    cv::Mat image{cv::imread(imagePath, cv::IMREAD_UNCHANGED)};
    if (!image.data)
    {
        std::snprintf(MessageBuffer,
                      sizeof(MessageBuffer),
                      "Can't load image from '%s'",
                      imagePath);
        throw std::runtime_error{MessageBuffer};
    }

    image.convertTo(image, CV_32F);
    cv::split(image, impl->imageChannels);
}

ImageProcessor::~ImageProcessor() = default;

void
ImageProcessor::run()
{
}

std::ostream&
ImageProcessor::dumpResult(std::ostream& os) const
{
    return os;
}

#if defined(BUILD_UNIT_TESTS)
TEST(ImageProcessor, TypeTraits)
{
    EXPECT_FALSE(std::is_copy_constructible<ImageProcessor>::value);
    EXPECT_FALSE(std::is_copy_assignable<ImageProcessor>::value);
    EXPECT_TRUE(std::is_move_constructible<ImageProcessor>::value);
    EXPECT_TRUE(std::is_move_assignable<ImageProcessor>::value);
}
#endif /* BUILD_UNIT_TESTS */
