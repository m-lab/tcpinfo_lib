// Copyright 2016 measurement-lab
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/****************************************************************************
* This file contains declarations that are used by both .c files and .cc
* files.  When included in .cc files, it must be enclosed in an extern "C"
* block.
*
* fetch_tcpinfo(...) is implemented in the tcpinfo_c_adapter.c for easy
* access to all the C libraries.
* update_record(...) is implemented in tcpinfo_lib.cc, because it requires
* access to C++ functions.
****************************************************************************/

#ifndef TCPINFO_C_ADAPTER_H_
#define TCPINFO_C_ADAPTER_H_

#include <linux/inet_diag.h>
#include <linux/netlink.h>
#include <sys/socket.h>

#include "libnetlink.h"

struct inet_diag_arg {
  int protocol;
};

// Fetch all tcp socket connections.  Implemention in tcpinfo_c_adapter.c
// `callback` has rtnl_filter_t signature, and is passed to rtnl_dump_filter.
int fetch_tcpinfo(rtnl_filter_t callback);

// rtnl_filter_t function to handle each result, passed into fetch_tcpinfo()
// function. Implementation in tcpinfo_lib.cc
int update_record(const struct sockaddr_nl *addr,
                  struct nlmsghdr *nlh, void *arg);

#endif  // TCPINFO_C_ADAPTER_H_
