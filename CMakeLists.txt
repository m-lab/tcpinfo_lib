cmake_minimum_required (VERSION 2.8.6)
set(PROJECT_NAME_STR tcpinfo_lib)
# Note that these must preceed the ENABLE_LANGUAGE statements.
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

ENABLE_LANGUAGE(C)
ENABLE_LANGUAGE(CXX)

set(CMAKE_CXX_FLAGS "-std=c++11")

# This allows the external project to push and pop environment.
cmake_policy(SET CMP0011 OLD)
include(ExternalProject)

project (${PROJECT_NAME_STR} C CXX)

find_package(Threads REQUIRED)

if (CMAKE_COMPILER_IS_GNUXX)
  add_definitions(-Wall -ansi -Wno-deprecated -pthread)
endif()

set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)

################### gtest submodule ########################
#set(PROJECT_LIB_NAME ${PROJECT_NAME_STR}_impl)

#include_directories(${COMMON_INCLUDES})
#file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
#add_library(${PROJECT_LIB_NAME} ${SRC_FILES})

add_subdirectory(${EXT_PROJECTS_DIR}/gtest)
enable_testing()
set(PROJECT_TEST_NAME ${PROJECT_NAME_STR}_test)
include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})

file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/src/*_test.cc)
MESSAGE("PROJECT SOURCE DIR: " ${PROJECT_SOURCE_DIR})
MESSAGE("TEST FILES: " ${TEST_SRC_FILES})
add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} googletest)


target_link_libraries(${PROJECT_TEST_NAME}
    ${GTEST_LIBS_DIR}/libgtest.a
    ${GTEST_LIBS_DIR}/libgtest_main.a
    )

target_link_libraries(${PROJECT_TEST_NAME} ${CMAKE_THREAD_LIBS_INIT})

add_test(test1 ${PROJECT_TEST_NAME})
################### iproute submodule ########################
add_subdirectory(${EXT_PROJECTS_DIR}/iproute2)

################### protobuf compiler ########################
# add_subdirectory(${EXT_PROJECTS_DIR})

