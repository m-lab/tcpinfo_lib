cmake_minimum_required (VERSION 2.8.6)

# Note that these must precede the ENABLE_LANGUAGE statements.
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
ENABLE_LANGUAGE(C)
ENABLE_LANGUAGE(CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

# This allows the external project to push and pop environment.
cmake_policy(SET CMP0011 OLD)
include(ExternalProject)

################### threads library ########################
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

################### project settings ########################
project (tcpinfo_lib C CXX)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

################### iproute submodule ########################
add_subdirectory(${PROJECT_SOURCE_DIR}/ext/iproute2)
include_directories(${IPROUTE2_INCLUDE_DIRS})
link_directories(${IPROUTE2_LIBS_DIR})

################### protobuf compiler ########################
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

# Include path for proto files.
include_directories(${CMAKE_CURRENT_BINARY_DIR})

################### gtest submodule ########################
add_subdirectory(${PROJECT_SOURCE_DIR}/ext/gtest)
enable_testing()

include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})

#############################################################
################## Project specific stuff ###################
#############################################################

################### tcpinfo library  ########################

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
    ${SRC_DIR}/tcpinfo.proto)
add_library(tcpinfo_lib
    ${SRC_DIR}/tcpinfo_lib.cc ${SRC_DIR}/tcpinfo_lib.h
    ${PROTO_HDRS} ${PROTO_SRCS})
add_dependencies(tcpinfo_lib iproute2)

target_link_libraries(tcpinfo_lib
    ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES}
    ${IPROUTE2_LIBS_DIR}/libnetlink.a
    ${IPROUTE2_LIBS_DIR}/libutil.a)

   #================= unit tests ======================

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
add_executable(tcpinfo_lib_test
    ${SRC_DIR}/tcpinfo_proto_test.cc)
add_dependencies(tcpinfo_lib_test googletest)

target_link_libraries(tcpinfo_lib_test
    ${GTEST_LIBS_DIR}/libgtest.a
    ${GTEST_LIBS_DIR}/libgtest_main.a
    tcpinfo_lib)

add_test(tcpinfo_proto tcpinfo_lib_test)

################### connection_cache library  ########################
add_library(connection_cache_lib
    ${SRC_DIR}/connection_cache.cc ${SRC_DIR}/connection_cache.h)
target_link_libraries(connection_cache_lib Threads::Threads)

   #================= unit tests ======================
add_executable(connection_cache_test
    ${SRC_DIR}/connection_cache_test.cc)
add_dependencies(connection_cache_test googletest)

target_link_libraries(connection_cache_test
    connection_cache_lib
    ${CMAKE_THREAD_LIBS_INIT}
    ${GTEST_LIBS_DIR}/libgtest.a
    ${GTEST_LIBS_DIR}/libgtest_main.a)

add_test(connection_cache connection_cache_test)

