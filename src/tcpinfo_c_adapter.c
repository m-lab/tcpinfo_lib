/*
 * This file contains code that calls C functions defined in the netlink
 * and related libraries, and iproute2 code.  It is challenging to
 * implement in C++ because of linkage issues.
 *
 * Derived from iproute2 ss.c.  Forked from net-next in Sept 2016.
 * Much of the content has been left close in form to the original
 * to make it clearer what the relationships are with the ss.c code.
 * Most of the ss.c code is not needed, and therefore stripped out.
 *
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version
 *    2 of the License, or (at your option) any later version.
 *
 * Original ss.c Author: Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 */

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

#define MAGIC_SEQ 123456

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
  req.r.idiag_states = 1 << SS_ESTABLISHED;
  // DO NOT SUBMIT
//  req.r.idiag_states = 0;
  /****************************************************************
  * With these additional flags, we occasionally last state we see
  * is usually LAST_ACK or TIME_WAIT, and occasionally we see one of
  * the FIN_WAIT states.  The LAST_ACK and FIN_WAIT1 states have full
  * information, but FIN_WAIT2 and TIME_WAIT have only the inet_diag_msg.
  *
  * Incidentally, dropping ESTABLISHED only raises the polling rate
  * from about 1000/sec to about 1600/sec.
  *****************************************************************/
  req.r.idiag_states |= 1 << SS_SYN_SENT;
  req.r.idiag_states |= 1 << SS_SYN_RECV;
  req.r.idiag_states |= 1 << SS_CLOSE_WAIT;
  req.r.idiag_states |= 1 << SS_CLOSING;
  req.r.idiag_states |= 1 << SS_CLOSE;
  req.r.idiag_states |= 1 << SS_LAST_ACK;
  req.r.idiag_states |= 1 << SS_FIN_WAIT1;
  req.r.idiag_states |= 1 << SS_FIN_WAIT2;
//  req.r.idiag_states |= 1 << SS_TIME_WAIT;

  // show_mem
  req.r.idiag_ext |= (1 << (INET_DIAG_MEMINFO - 1));
  req.r.idiag_ext |= (1 << (INET_DIAG_SKMEMINFO - 1));

  // show_tcpinfo
  req.r.idiag_ext |= (1 << (INET_DIAG_INFO - 1));
  req.r.idiag_ext |= (1 << (INET_DIAG_VEGASINFO - 1));
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
