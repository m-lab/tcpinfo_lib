cmake_minimum_required (VERSION 2.8.6)
project(gtest_builder C CXX)
include(ExternalProject)

ExternalProject_Add(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    CMAKE_ARGS
    -DBUILD_GTEST=ON

    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    INSTALL_COMMAND ""
    )

# Specify include dir
ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIRS
    ${source_dir}/googletest/include PARENT_SCOPE)

# Specify MainTest's link libraries
ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBS_DIR
    ${binary_dir}/googlemock/gtest PARENT_SCOPE)
