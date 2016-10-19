cmake_minimum_required(VERSION 2.8.8)
project(protobuf C CXX)
include(ExternalProject)

ExternalProject_Add(protobuf
  GIT_REPOSITORY "git@github.com:google/protobuf.git"
  GIT_TAG "v3.1.0"

  UPDATE_COMMAND ""
  PATCH_COMMAND ""

  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build"

  CONFIGURE_COMMAND
      ./configure

  # Unfortunately, we have to build in the source directory.
  BUILD_IN_SOURCE 1

  BUILD_COMMAND
      ${MAKE}

  INSTALL_COMMAND
      sudo make install

  TEST_COMMAND ""
)

ExternalProject_Add_Step(protobuf autogen
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build"
	COMMAND
	    pwd
	COMMAND
	    ./autogen.sh
	DEPENDERS configure
	DEPENDEES download
	)

ExternalProject_Add_Step(protobuf ldconfig
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/protobuf"
	COMMAND
            sudo ldconfig # refresh shared library cache.
	DEPENDEES install
	DEPENDERS test
	)
