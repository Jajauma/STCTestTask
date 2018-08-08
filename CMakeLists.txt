cmake_minimum_required(VERSION 3.0.0)
project(STCTestTask CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_UNIT_TESTS "Build unit tests" OFF)

# Use -DCMAKE_PREFIX_PATH=... to set custom search path
find_package(OpenCV 3.0.0 REQUIRED COMPONENTS imgcodecs)

set(sources
    Main.cpp
    MainProgram.cpp)

if (BUILD_UNIT_TESTS)
    set (sources ${sources}
        Program/MainRunUnitTests.cpp)
endif()

set(mainTarget integral_image)
add_executable(${mainTarget} ${sources})
target_link_libraries(${mainTarget}
    PRIVATE opencv_imgcodecs)

if (BUILD_UNIT_TESTS)
    find_package(GTest REQUIRED)
    target_compile_definitions(${mainTarget}
        PRIVATE BUILD_UNIT_TESTS)
    target_link_libraries(${mainTarget}
        PRIVATE GTest::GTest)
endif()