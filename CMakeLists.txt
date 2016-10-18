cmake_minimum_required (VERSION 2.8.6)

############### Source Tree Overview ######################
# ├── build
# ├── CMakeLists.txt
# ├── ext
# │   ├── gtest
# │   │   └── CMakeLists.txt
# │   ├── iproute2
# │   │   └── CMakeLists.txt
# │   └── protobuf
# │       └── CMakeLists.txt
# ├── install-protobuf.sh
# ├── LICENSE
# ├── README.md
# └── src
#     ├── tcpinfo.proto
#     └── tcpinfo_proto_test.cc

# Note that these must precede the ENABLE_LANGUAGE statements.
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
ENABLE_LANGUAGE(C)
ENABLE_LANGUAGE(CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

# This allows the external project to push and pop environment.
cmake_policy(SET CMP0011 OLD)
include(ExternalProject)


################### project settings ########################
set(PROJECT_NAME_STR tcpinfo_lib)
project (${PROJECT_NAME_STR} C CXX)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

################### iproute submodule ########################
add_subdirectory(${PROJECT_SOURCE_DIR}/ext/iproute2)

################### protobuf compiler ########################
find_package(Threads REQUIRED)
find_package(Protobuf)
if (NOT PROTOBUF_FOUND)
	add_subdirectory(${PROJECT_SOURCE_DIR}/ext/protobuf)
endif()

include_directories(${PROTOBUF_INCLUDE_DIRS})

# Include path for proto files.
include_directories(${CMAKE_CURRENT_BINARY_DIR})

################### gtest submodule ########################
add_subdirectory(${PROJECT_SOURCE_DIR}/ext/gtest)
enable_testing()

#############################################################
################## Project specific stuff ###################
#############################################################

################### tcpinfo library  ########################

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${SRC_DIR}/tcpinfo.proto)
add_library(${PROJECT_NAME}
	${PROTO_HDRS} ${PROTO_SRCS})

################### unit tests ########################
set(PROJECT_TEST_NAME ${PROJECT_NAME_STR}_test)
include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
add_executable(${PROJECT_TEST_NAME} ${SRC_DIR}/empty_test.cc)
add_dependencies(${PROJECT_TEST_NAME} googletest)

target_link_libraries(${PROJECT_TEST_NAME}
    ${GTEST_LIBS_DIR}/libgtest.a
    ${GTEST_LIBS_DIR}/libgtest_main.a
    tcpinfo_lib
    )

target_link_libraries(${PROJECT_TEST_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_TEST_NAME} ${PROTOBUF_LIBRARIES})
add_test(test1 ${PROJECT_TEST_NAME})  # Add project test to ctest targets.

