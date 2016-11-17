# tcpinfo_lib [![Test Status](https://travis-ci.org/m-lab/tcpinfo_lib.svg)](https://travis-ci.org/m-lab/tcpinfo_lib) [![Coverage Status](https://coveralls.io/repos/github/m-lab/tcpinfo_lib/badge.svg)](https://coveralls.io/github/m-lab/tcpinfo_lib)

Wrapper library for collecting data through netlink.
 
NDT and Sidestream both will require functionality related to but not entirely provided by the netlink library.  This repository will hold primarily OO code that encapsulates calls to netlink library functions, and basic manipulation of the resulting instrumentation data, including protobuf generation.

# Prerequisites
General requirements for building this repository
```
sudo apt-get install build-essential autoconf automake make cmake
sudo apt-get install clang g++ libtool curl unzip bison flex
```

## clang
The build is configured to use clang.  If you want to use a different compiler,
export CXX_COMPILER and C_COMPILER.

# Building
```
git clone git@github.com:mlab/tcpinfo_lib
cd tcpinfo_lib
mkdir build
cd build
cmake .. && cmake --build . && ctest -V
```
When make finishes, there should be a directory tree under .../build that
contains all config, code, object files, libraries, and binaries.

# Travis-CI integration
.travis.yml provides config for building within travis-ci.  It includes a gcc
config that also produces coverage data for coveralls, and a clang config that
builds optimized code.

## protobuf compiler
CMake apparently uses pkg-config to discover the protobuf compiler.  But it
appears that when installed with apt-get install protobuf-compiler, (at least
on gobuntu) the pkg-config .pc file is not installed.
So, to make everything work smoothly, cmake .. will run install-protobuf.sh,
which will install it from github if pkg-config doesn't have a suitable config.

This requires sudo privileges, and you may be prompted for your password for
the install and ldconfig steps.

If you later want to revert your system, you will need to cd to the protobuf
directory, and:
```
sudo make uninstall
```
To restore the original protobuf-compiler (if you had one), you may also need
to:
```
sudo apt-get uninstall protobuf-compiler
sudo apt-get install protobuf-compiler
```

# Dependency on iproute2 and gtest
The ext/iproute2 and ext/gtest directories provide rules for downloading
and building the dependencies.  They are incorporated with add_subdirectory
and should be downloaded and built when needed.  They should show up under
build/ext/...

# Source tree
```
├── .travis.yml
├── CMakeLists.txt
├── ext
│   ├── gtest
│   │   └── CMakeLists.txt
│   └── iproute2
│       └── CMakeLists.txt
├── install-protobuf.sh
├── LICENSE
├── pre-commit
├── README.md
└── src
    ├── connection_cache.cc
    ├── connection_cache.h
    ├── connection_cache_test.cc
    ├── main.cc
    ├── README.md
    ├── tcpinfo_c_adapter.c
    ├── tcpinfo_c_adapter.h
    ├── tcpinfo_lib.cc
    ├── tcpinfo_lib.h
    ├── tcpinfo_lib_test.cc
    ├── tcpinfo.proto
    └── tcpinfo_proto_test.cc
```
