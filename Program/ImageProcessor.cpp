#include "ImageProcessor.hpp"

#if defined(BUILD_UNIT_TESTS)
#include <gtest/gtest.h>
#endif /* BUILD_UNIT_TESTS */

#include <opencv2/opencv.hpp>

#include <cassert>
#include <cstdio>
#include <iterator>
#include <stdexcept>
#include <vector>

#if defined(BUILD_UNIT_TESTS)
#include <type_traits>
#endif /* BUILD_UNIT_TESTS */

using namespace Program;

namespace {
char MessageBuffer[4096];

/** \brief Calculate a point in the summed-area table
 *
 * Calculate a point in the summed-area table using recurrent formula
 * I(x,y)=i(x,y)+I(x,y-1)+I(x-1,y)-I(x-1,y-1)
 *
 * \sa https://en.wikipedia.org/wiki/Summed-area_table
 */
template <typename IntensityT>
void
calcSATPointRecurrently(cv::Mat_<IntensityT>& I, int y, int x)
{
    assert(y >= 0 && y < I.rows);
    assert(x >= 0 && x < I.cols);

    if (x && y)
        I(y, x) += I(y - 1, x) + I(y, x - 1) - I(y - 1, x - 1);
    else if (y && !x)
        I(y, x) += I(y - 1, x);
    else if (!y && x)
        I(y, x) += I(y, x - 1);
}

/** \brief Build a summed-area table
 *
 * Build a summed-area table by iterating over all diagonals of the matrix
 * starting from the topleft corner.
 */
template <typename IntensityT>
cv::Mat_<IntensityT>&
buildSAT(cv::Mat_<IntensityT>& I)
{
    for (int i = 0; i < I.rows; ++i)
        for (int y = i, x = 0; y >= 0 && x < I.cols; --y, ++x)
            calcSATPointRecurrently(I, y, x);
    for (int i = 1; i < I.cols; ++i)
        for (int y = I.rows - 1, x = i; y >= 0 && x < I.cols; --y, ++x)
            calcSATPointRecurrently(I, y, x);
    return I;
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
#pragma omp parallel for
    for (auto i = 0U; i < impl->imageChannels.size(); ++i)
        buildSAT(impl->imageChannels[i]);
}

std::ostream&
ImageProcessor::exportResults(std::ostream& os) const
{
    for (auto const& c : impl->imageChannels)
        for (auto it = c.begin(); it != c.end(); ++it)
        {
            auto const next = std::next(it);
            os << *it
               << (next == c.end() ? "\n\n"
                                   : (next.pos().y == it.pos().y ? " " : "\n"));
        }
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

TEST(ImageProcessor, BuildSAT_10x1)
{
    cv::Mat_<int> data{cv::Size{10, 1}};
    data << 1, 1, 1, 1, 1, 1, 1, 1, 1, 1;

    cv::Mat_<int> const expected{cv::Size{10, 1}};
    expected << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10;

    EXPECT_FALSE(cv::countNonZero(buildSAT(data) != expected));
}

TEST(ImageProcessor, BuildSAT_1x10)
{
    cv::Mat_<int> data{cv::Size{1, 10}};
    data << 1, 1, 1, 1, 1, 1, 1, 1, 1, 1;

    cv::Mat_<int> const expected{cv::Size{1, 10}};
    expected << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10;

    EXPECT_FALSE(cv::countNonZero(buildSAT(data) != expected));
}

TEST(ImageProcessor, BuildSAT_2x3)
{
    cv::Mat_<int> data{cv::Size{2, 3}};
    data << 0, 1, 2, 3, 4, 5;

    cv::Mat_<int> const expected{cv::Size{2, 3}};
    expected << 0, 1, 2, 6, 6, 15;

    EXPECT_FALSE(cv::countNonZero(buildSAT(data) != expected));
}

TEST(ImageProcessor, BuildSAT_6x6)
{
    cv::Mat_<int> data{cv::Size{6, 6}};
    data << 31, 2, 4, 33, 5, 36, 12, 26, 9, 10, 29, 25, 13, 17, 21, 22, 20, 18,
        24, 23, 15, 16, 14, 19, 30, 8, 28, 27, 11, 7, 1, 35, 34, 3, 32, 6;

    cv::Mat_<int> const expected{cv::Size{6, 6}};
    expected << 31, 33, 37, 70, 75, 111, 43, 71, 84, 127, 161, 222, 56, 101,
        135, 200, 254, 333, 80, 148, 197, 278, 346, 444, 110, 186, 263, 371,
        450, 555, 111, 222, 333, 444, 555, 666;

    EXPECT_FALSE(cv::countNonZero(buildSAT(data) != expected));
}
#endif /* BUILD_UNIT_TESTS */
