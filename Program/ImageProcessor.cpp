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

template <typename IntensityT>
void
calcSATPointRecurrently(cv::Mat_<IntensityT>& m, int y, int x)
{
    assert(y >= 0 && y < m.rows);
    assert(x >= 0 && x < m.cols);

    if (x && y)
        m(y, x) += m(y - 1, x) + m(y, x - 1) - m(y - 1, x - 1);
    else if (y && !x)
        m(y, x) += m(y - 1, x);
    else if (!y && x)
        m(y, x) += m(y, x - 1);
}

template <typename IntensityT>
void
buildSATRecurrently(cv::Mat_<IntensityT>& m)
{
    for (int i = 0; i < m.rows; ++i)
        for (int row = i, col = 0; row >= 0 && col < m.cols; --row, ++col)
            calcSATPointRecurrently(m, row, col);
    for (int i = 1; i < m.cols; ++i)
        for (int row = m.rows - 1, col = i; row >= 0 && col < m.cols;
             --row, ++col)
            calcSATPointRecurrently(m, row, col);
}

using IntensityType      = float;
auto const IntensityCode = CV_32F;
} /* namespace */

struct ImageProcessor::Implementation
{
    std::vector<cv::Mat_<IntensityType>> imageChannels;
};

ImageProcessor::ImageProcessor(char const* imagePath)
    : impl{new Implementation}
{
    assert(imagePath != nullptr);

    cv::Mat image{cv::imread(imagePath, cv::IMREAD_UNCHANGED)};
    if (!image.data || !image.rows || !image.cols)
    {
        std::snprintf(MessageBuffer,
                      sizeof(MessageBuffer),
                      "Can't load image from '%s'",
                      imagePath);
        throw std::runtime_error{MessageBuffer};
    }

    image.convertTo(image, IntensityCode);
    cv::split(image, impl->imageChannels);
}

ImageProcessor::~ImageProcessor() = default;

void
ImageProcessor::run()
{
    for (auto& c : impl->imageChannels)
        buildSATRecurrently(c);
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

TEST(ImageProcessor, BuildSATRecurrently_10x1)
{
    cv::Mat_<int> data{cv::Size{10, 1}};
    data << 1, 1, 1, 1, 1, 1, 1, 1, 1, 1;

    cv::Mat_<int> const expected{cv::Size{10, 1}};
    expected << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10;

    EXPECT_NO_THROW(buildSATRecurrently(data));
    EXPECT_EQ(cv::countNonZero(data != expected), 0);
}

TEST(ImageProcessor, BuildSATRecurrently_1x10)
{
    cv::Mat_<int> data{cv::Size{1, 10}};
    data << 1, 1, 1, 1, 1, 1, 1, 1, 1, 1;

    cv::Mat_<int> const expected{cv::Size{1, 10}};
    expected << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10;

    EXPECT_NO_THROW(buildSATRecurrently(data));
    EXPECT_EQ(cv::countNonZero(data != expected), 0);
}

TEST(ImageProcessor, BuildSATRecurrently_2x3)
{
    cv::Mat_<int> data{cv::Size{2, 3}};
    data << 0, 1, 2, 3, 4, 5;

    cv::Mat_<int> const expected{cv::Size{2, 3}};
    expected << 0, 1, 2, 6, 6, 15;

    EXPECT_NO_THROW(buildSATRecurrently(data));
    EXPECT_EQ(cv::countNonZero(data != expected), 0);
}

TEST(ImageProcessor, BuildSATRecurrently_6x6)
{
    cv::Mat_<int> data{cv::Size{6, 6}};
    data << 31, 2, 4, 33, 5, 36, 12, 26, 9, 10, 29, 25, 13, 17, 21, 22, 20, 18,
        24, 23, 15, 16, 14, 19, 30, 8, 28, 27, 11, 7, 1, 35, 34, 3, 32, 6;

    cv::Mat_<int> const expected{cv::Size{6, 6}};
    expected << 31, 33, 37, 70, 75, 111, 43, 71, 84, 127, 161, 222, 56, 101,
        135, 200, 254, 333, 80, 148, 197, 278, 346, 444, 110, 186, 263, 371,
        450, 555, 111, 222, 333, 444, 555, 666;

    EXPECT_NO_THROW(buildSATRecurrently(data));
    EXPECT_EQ(cv::countNonZero(data != expected), 0);
}
#endif /* BUILD_UNIT_TESTS */
