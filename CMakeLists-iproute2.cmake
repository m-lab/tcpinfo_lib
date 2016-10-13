cmake_minimum_required (VERSION 2.6)

ExternalProject_Add(
  iproute2

  GIT_REPOSITORY "git://git.kernel.org/pub/scm/linux/kernel/git/shemminger/iproute2.git"
  GIT_TAG "net-next"

  UPDATE_COMMAND ""
  PATCH_COMMAND ""

  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/iproute2"

  CONFIGURE_COMMAND
      ./configure

  BUILD_IN_SOURCE 1

  BUILD_COMMAND
      ${MAKE}

  INSTALL_COMMAND ""

  TEST_COMMAND ""
)

# Required to fix xtables related build bug.
ExternalProject_Add_Step(iproute2 xtables-fix
	WORKING_DIRECTORY ${SOURCE_DIR}
	COMMENT
	    source dir is ${SOURCE_DIR}
	COMMAND
	    touch <SOURCE_DIR>/include/xtables-version.h
	DEPENDERS build
	DEPENDEES configure
	)
