cmake_minimum_required (VERSION 2.8.6)

# Note that these must precede the ENABLE_LANGUAGE statements.
ENABLE_LANGUAGE(C)
ENABLE_LANGUAGE(CXX)

# .travis.yml invokes CMake with CXX_COMPILER defined.  But on workstation,
# we want to define that ourselves to be clang (if available).
if (NOT DEFINED CXX_COMPILER)
  set(CXX_COMPILER "clang++")
  set(C_COMPILER "clang")
endif (NOT DEFINED CXX_COMPILER)

if (NOT DEFINED COVERAGE_FLAGS OR COVERAGE_FLAGS STREQUAL "")
# If we aren't instrumenting for coverage, then add optimization flags.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
else ()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_FLAGS}")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

MESSAGE(STATUS "CXX flags: " ${CMAKE_CXX_FLAGS})

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
execute_process(COMMAND ${PROJECT_SOURCE_DIR}/install-protobuf.sh)
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
    ${SRC_DIR}/tcpinfo_c_adapter.h ${SRC_DIR}/tcpinfo_c_adapter.c
    ${SRC_DIR}/gtest_prod.h ${PROTO_HDRS} ${PROTO_SRCS})
add_dependencies(tcpinfo_lib iproute2)

target_link_libraries(tcpinfo_lib
    connection_cache_lib
    ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES}
    ${IPROUTE2_LIBS_DIR}/libnetlink.a
    ${IPROUTE2_LIBS_DIR}/libutil.a)

   #================= unit tests ======================

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
add_executable(tcpinfo_proto_test
    ${SRC_DIR}/tcpinfo_proto_test.cc)
add_dependencies(tcpinfo_proto_test googletest)

target_link_libraries(tcpinfo_proto_test
    ${GTEST_LIBS_DIR}/libgtest.a
    ${GTEST_LIBS_DIR}/libgtest_main.a
    tcpinfo_lib)

add_test(tcpinfo_proto tcpinfo_proto_test)

add_executable(tcpinfo_lib_test
    ${SRC_DIR}/tcpinfo_lib_test.cc)
add_dependencies(tcpinfo_lib_test googletest)

target_link_libraries(tcpinfo_lib_test
    ${GTEST_LIBS_DIR}/libgtest.a
    ${GTEST_LIBS_DIR}/libgtest_main.a
    tcpinfo_lib)

add_test(tcpinfo_lib tcpinfo_lib_test)

################### connection_cache library  ########################
add_library(connection_cache_lib
    ${SRC_DIR}/connection_cache.cc ${SRC_DIR}/connection_cache.h)
target_link_libraries(connection_cache_lib pthread)

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

################### tcpinfo polling proto ########################
add_executable(poll
    ${SRC_DIR}/main.cc
    ${SRC_DIR}/tcpinfo_c_adapter.c)
add_dependencies(poll iproute2 tcpinfo_lib)
target_link_libraries(poll
    ${IPROUTE2_LIBS_DIR}/libnetlink.a
    ${IPROUTE2_LIBS_DIR}/libutil.a
    connection_cache_lib
    tcpinfo_lib)
