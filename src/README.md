# Components
## tcpinfo.proto
... contains the data structures for all tcp_info and netlink related code.

## tcpinfo_lib
... contains the primary interface for interacting with the

## connection_cache
... keeps a cache of data for each connection seen so far.

## tcpinfo_c_adapter
... contains the low level code that makes calls to the netlink library and
iproute2 code.

## main.cc
... contains a simple demo polling program.

