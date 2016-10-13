cmake_minimum_required (VERSION 2.8.6)
ENABLE_LANGUAGE(C)
ENABLE_LANGUAGE(CXX)

project (tcpinfo_lib)

# This allows the external project to push and pop environment.
cmake_policy(SET CMP0011 OLD)

include(ExternalProject)
include(CMakeLists-iproute2.cmake)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories("/usr/local/google/home/gfr/mlab/tcp_info/iproute2/include")

