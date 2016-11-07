# tcpinfo_lib
Wrapper library for collecting data through netlink.

NDT and Sidestream both will require functionality related to but not entirely provided by the netlink library.  This repository will hold primarily OO code that encapsulates calls to netlink library functions, and basic manipulation of the resulting instrumentation data, including protobuf generation.

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
│   └── iproute2
├── install-protobuf.sh
├── LICENSE
├── README.md
└── src
    ├── tcpinfo_lib.cc
    ├── tcpinfo_lib.h
    ├── tcpinfo.proto
    └── tcpinfo_proto_test.cc
```
