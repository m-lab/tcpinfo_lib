# tcpinfo_lib
Wrapper library for collecting data through netlink.

NDT and Sidestream both will require functionality related to but not entirely provided by the netlink library.  This repository will hold primarily OO code that encapsulates calls to netlink library functions, and basic manipulation of the resulting instrumentation data, including protobuf generation.

# Dependency on iproute2
Two CMake files are provided that include rules to download and build the
net-next branch of the iproute2 tools.  You will need cmake installed, and after
cloning the repository, do:
  cd build
  cmake ..
  make
When make finishes, there should be an iproute2 directory, and make should have
successfully build all targets under iproute2.
