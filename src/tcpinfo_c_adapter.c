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
//
// This code is loosely modeled on misc/ss.c code from the iproute2
// library by Alexey Kuznetsov, circa September 2016.
//
#include "tcpinfo_c_adapter.h"

#include <arpa/inet.h>
#include <linux/sock_diag.h>
#include <unistd.h>

#include "libnetlink.h"

// Would rather use tcp_states.h, which is what idiag_states needs, but that
// seems to be only in the kernel headers.  linux/tcp.h in gobuntu doesn't
// have these either.  8-(
enum {
  SS_UNKNOWN,
  SS_ESTABLISHED,
  SS_SYN_SENT,
  SS_SYN_RECV,  // Excluded from SS_CONN
  SS_FIN_WAIT1,
  SS_FIN_WAIT2,
  SS_TIME_WAIT,  // Excluded from SS_CONN
  SS_CLOSE,      // Excluded from SS_CONN
  SS_CLOSE_WAIT,
  SS_LAST_ACK,
  SS_LISTEN,  // Excluded from SS_CONN
  SS_CLOSING,
  SS_MAX
};

// Magic number used for nlmsg_seq.  Doesn't really matter since we are
// running synchronously (???).
// TODO(gfr) For thoroughness, should this be a sequence instead of a constant?
// iproute2 ss.c just uses a constant.
#define MAGIC_SEQ 0x3A41B852

// family is expected to be PF_INET or PF_INET6
// protocol expected to be IPPROTO_TCP
static int sockdiag_send(int family, int fd, int protocol) {
  struct sockaddr_nl nladdr = {.nl_family = AF_NETLINK};
  struct {
    struct nlmsghdr nlh;
    struct inet_diag_req_v2 r;
  } req = {
      .nlh =
          {
              .nlmsg_type = SOCK_DIAG_BY_FAMILY,
              .nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST,
              .nlmsg_seq = MAGIC_SEQ,
              .nlmsg_len = sizeof(req),
          },
  };
  struct msghdr msg;
  struct iovec iov[3];  // TODO: Why 3 ??
  int iovlen = 1;

  memset(&req.r, 0, sizeof(req.r));
  req.r.sdiag_family = family;
  req.r.sdiag_protocol = protocol;

  // With these additional flags, the last state we see is usually LAST_ACK,
  // TIME_WAIT, or one of the FIN_WAIT states.  The LAST_ACK and FIN_WAIT1
  // states have full information, but FIN_WAIT2 and TIME_WAIT have only the
  // inet_diag_msg.
  // Since we don't ALWAYS see a final state when a connection ends, we request
  // and monitor ESTABLISHED records to avoid missing connections (though we may
  // still miss some).  Including ESTABLISHED drops the polling rate from about
  // 1600/sec to about 1000/sec on a modern 3 GHz machine.
  req.r.idiag_states = 1 << SS_ESTABLISHED;
  req.r.idiag_states |= 1 << SS_SYN_SENT;
  req.r.idiag_states |= 1 << SS_SYN_RECV;
  req.r.idiag_states |= 1 << SS_CLOSE_WAIT;
  req.r.idiag_states |= 1 << SS_CLOSING;
  req.r.idiag_states |= 1 << SS_CLOSE;
  req.r.idiag_states |= 1 << SS_LAST_ACK;
  req.r.idiag_states |= 1 << SS_FIN_WAIT1;
  req.r.idiag_states |= 1 << SS_FIN_WAIT2;
  req.r.idiag_states |= 1 << SS_TIME_WAIT;

  // show_mem
  req.r.idiag_ext |= (1 << (INET_DIAG_MEMINFO - 1));
  req.r.idiag_ext |= (1 << (INET_DIAG_SKMEMINFO - 1));

  // show_tcpinfo
  req.r.idiag_ext |= (1 << (INET_DIAG_INFO - 1));

  // congestion info
  req.r.idiag_ext |= (1 << (INET_DIAG_VEGASINFO - 1));
  req.r.idiag_ext |= (1 << (INET_DIAG_DCTCPINFO - 1));
  req.r.idiag_ext |= (1 << (INET_DIAG_BBRINFO - 1));
  req.r.idiag_ext |= (1 << (INET_DIAG_CONG - 1));

  iov[0] = (struct iovec){.iov_base = &req, .iov_len = sizeof(req)};

  msg = (struct msghdr){
      .msg_name = (void *)&nladdr,
      .msg_namelen = sizeof(nladdr),
      .msg_iov = iov,
      .msg_iovlen = iovlen,
  };

  if (sendmsg(fd, &msg, 0) < 0) {
    close(fd);
    return -1;
  }

  return 0;
}

// Returns error code from sockdiag_send or rtnl_dump_filter.
//   0: successful
// < 0: error
int fetch_tcpinfo(rtnl_filter_t callback) {
  int err = 0;
  struct rtnl_handle rth;
  struct inet_diag_arg arg = {.protocol = IPPROTO_TCP};
  if (rtnl_open_byproto(&rth, 0, NETLINK_SOCK_DIAG)) return -1;

  rth.dump = MAGIC_SEQ;
  rth.dump_fp = NULL;  // TODO(gfr) Do we want to support dump_fp?

  if ((err = sockdiag_send(PF_INET, rth.fd, IPPROTO_TCP))) goto Exit;
  if ((err = rtnl_dump_filter(&rth, callback, &arg))) goto Exit;
  if ((err = sockdiag_send(PF_INET6, rth.fd, IPPROTO_TCP))) goto Exit;
  if ((err = rtnl_dump_filter(&rth, callback, &arg))) goto Exit;

Exit:
  rtnl_close(&rth);
  return err;
}
