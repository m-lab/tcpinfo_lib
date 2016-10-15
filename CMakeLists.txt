cmake_minimum_required (VERSION 2.8.6)

# Note that these must precede the ENABLE_LANGUAGE statements.
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
ENABLE_LANGUAGE(C)
ENABLE_LANGUAGE(CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(PROJECT_NAME_STR tcpinfo_lib)

# This allows the external project to push and pop environment.
cmake_policy(SET CMP0011 OLD)
include(ExternalProject)

project (${PROJECT_NAME_STR} C CXX)

find_package(Threads REQUIRED)

################### iproute submodule ########################
add_subdirectory(${PROJECT_SOURCE_DIR}/ext/iproute2)

################### protobuf compiler ########################
find_package(Protobuf)
if (PROTOBUF_FOUND)
	add_subdirectory(${PROJECT_SOURCE_DIR}/ext/protobuf)
endif()

if (CMAKE_COMPILER_IS_GNUXX)
  add_definitions(-Wall -ansi -Wno-deprecated -pthread)
endif()


################### gtest submodule ########################
add_subdirectory(${PROJECT_SOURCE_DIR}/ext/gtest)
enable_testing()
set(PROJECT_TEST_NAME ${PROJECT_NAME_STR}_test)
include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
add_executable(${PROJECT_TEST_NAME} ${SRC_DIR}/cpp_sorter_test.cc)
add_dependencies(${PROJECT_TEST_NAME} googletest)

target_link_libraries(${PROJECT_TEST_NAME}
    ${GTEST_LIBS_DIR}/libgtest.a
    ${GTEST_LIBS_DIR}/libgtest_main.a
    )

target_link_libraries(${PROJECT_TEST_NAME} ${CMAKE_THREAD_LIBS_INIT})
add_test(test1 ${PROJECT_TEST_NAME})  # Add project test to ctest targets.

