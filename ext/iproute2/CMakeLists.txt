cmake_minimum_required (VERSION 2.6)

ExternalProject_Add(iproute2
  GIT_REPOSITORY "git://git.kernel.org/pub/scm/linux/kernel/git/shemminger/iproute2.git"
  GIT_TAG "net-next"

  UPDATE_COMMAND ""
  PATCH_COMMAND ""

  PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
  # SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build"

  CONFIGURE_COMMAND
      ./configure

  # Unfortunately, we have to build in the source directory, because of
  # ./configure
  BUILD_IN_SOURCE 1

  BUILD_COMMAND
      ${MAKE}

  INSTALL_COMMAND ""

  TEST_COMMAND ""
)

ExternalProject_Get_Property(iproute2 source_dir)

# Required to fix xtables related build bug.
ExternalProject_Add_Step(iproute2 xtables-fix
	WORKING_DIRECTORY ${source_dir}
	COMMENT
	    source dir is ${source_dir}
	COMMAND
	    touch <SOURCE_DIR>/include/xtables-version.h
	DEPENDERS build
	DEPENDEES configure
	)
