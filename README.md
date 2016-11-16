# tcpinfo_lib [![Test Status](https://travis-ci.org/m-lab/tcpinfo_lib.svg)](https://travis-ci.org/m-lab/tcpinfo_lib) [![Coverage Status](https://coveralls.io/repos/github/m-lab/tcpinfo_lib/badge.svg)](https://coveralls.io/github/m-lab/tcpinfo_lib)

Wrapper library for collecting data through netlink.
 
NDT and Sidestream both will require functionality related to but not entirely provided by the netlink library.  This repository will hold primarily OO code that encapsulates calls to netlink library functions, and basic manipulation of the resulting instrumentation data, including protobuf generation.

# Prerequisites
General requirements for building this repository
```
sudo apt-get install build-essential autoconf automake make cmake
sudo apt-get install clang g++ libtool curl unzip bison flex
```

# Building
```
git clone git@github.com:gfr10598/tcpinfo_lib
cd tcpinfo_lib
mkdir build
cd build
cmake .. && cmake --build . && ctest -V
```

# Dependencies
There are several dependencies that are handled through add_directory(...).  The
CMake files for these are under ext/, and they are invoked as needed.

The install_protobuf.sh script is used for building and installing google protobuf as needed.

# Travis-CI integration
.travis.yml provides config for building within travis-ci.  Currently only gcc
config is working, but will hopefully add clang config later.

# Source tree
```
├── .travis.yml
├── CMakeLists.txt
├── ext
│   ├── gtest
│   │   └── CMakeLists.txt
│   └── iproute2
│       └── CMakeLists.txt
├── install-protobuf.sh
├── LICENSE
├── README.md
└── src
    ├── connection_cache.cc
    ├── connection_cache.h
    ├── connection_cache_test.cc
    ├── tcpinfo_lib.cc
    ├── tcpinfo_lib.h
    ├── tcpinfo.proto
    └── tcpinfo_proto_test.cc
```
