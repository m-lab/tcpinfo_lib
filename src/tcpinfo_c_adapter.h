#ifndef TCPINFO_C_ADAPTER_H_
#define TCPINFO_C_ADAPTER_H_

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
*  fetch_tcpinfo() is called from C++, but implemented in C to provide clear
*  linkage to C library functions.
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
#include "libnetlink.h"  // For rtnl_filter_t

struct inet_diag_arg {
  int protocol;
};

// Fetch all tcp socket connections.  Implemention in tcpinfo_c_adapter.c
// `callback` has rtnl_filter_t signature, and is passed to rtnl_dump_filter.
int fetch_tcpinfo(rtnl_filter_t callback);

#ifdef __cplusplus
}
#endif
#endif  // TCPINFO_C_ADAPTER_H_
