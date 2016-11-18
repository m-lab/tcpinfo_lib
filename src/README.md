# Components
## tcpinfo.proto
Data structures for all tcp_info and netlink related code.

## tcpinfo_lib
Utility library providing high level api to netlink services.

## connection_cache
Component for caching data for each active (non-local) connection.

## tcpinfo_c_adapter
Low level code that makes calls to the netlink library and iproute2 code.

## main.cc
Simple demo polling program illustrating use of the tcpinfo_lib library.

