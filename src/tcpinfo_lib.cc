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

#include "tcpinfo_lib.h"

#include "utils.h"

#include <linux/tcp.h>
#include <linux/sock_diag.h>
#include <linux/inet_diag.h>
#include <linux/unix_diag.h>

namespace mlab {
namespace netlink {
namespace {
// Extract a single 16 bit ipv6 field from byte array.
// <ip> byte array
// <n> offset
unsigned int extract(const std::string& ip, int n) {
  return (((unsigned char)(ip[n])) << 8) + (unsigned char)ip[n+1];
}

// Parse primary inet_diag_msg into protobuf.
// <r> Non-null pointer to message.
// <proto> Non-null pointer to protobuf.
void ParseInetDiagMsg(struct inet_diag_msg* r, InetDiagMsgProto* proto) {
  auto family = r->idiag_family;
  if (!InetDiagMsgProto_AddressFamily_IsValid(family)) {
    fprintf(stderr, "Invalid family: %d\n", family);
  } else {
    proto->set_family(InetDiagMsgProto::AddressFamily(family));
  }
  if (!TCPState_IsValid(r->idiag_state)) {
    fprintf(stderr, "Invalid state: %d\n", r->idiag_state);
  } else {
    proto->set_state(TCPState(r->idiag_state));
  }

  auto* sock_id = proto->mutable_sock_id();
  const uint32_t* cookie = r->id.idiag_cookie;
  sock_id->set_cookie(
      (((unsigned long long)cookie[1] << 31) << 1) | cookie[0]);
  sock_id->set_interface(r->id.idiag_if);

  auto* src = sock_id->mutable_source();
  src->set_port(ntohs(r->id.idiag_sport));
  src->set_ip(r->id.idiag_src, family == AF_INET ? 4 : 16);

  auto* dest = sock_id->mutable_destination();
  dest->set_port(ntohs(r->id.idiag_dport));
  dest->set_ip(r->id.idiag_dst, family == AF_INET ? 4 : 16);

  if (r->idiag_timer)
    proto->set_timer(r->idiag_timer);
  if (r->idiag_retrans)
    proto->set_retrans(r->idiag_retrans);

  proto->set_expires(r->idiag_expires);
  if (r->idiag_rqueue)
    proto->set_rqueue(r->idiag_rqueue);
  if (r->idiag_wqueue)
    proto->set_wqueue(r->idiag_wqueue);
  proto->set_uid(r->idiag_uid);
  proto->set_inode(r->idiag_inode);
}

// Parse rtattr message containing BBR info.
// <rta> Non-null pointer to message.
// <proto> Non-null pointer to protobuf.
void ParseBBRInfo(const struct rtattr* rta, BBRInfoProto* proto) {
  const auto* bbr = (const struct tcp_bbr_info*) rta;
  auto bw = (((unsigned long long)bbr->bbr_bw_hi) << 32) + bbr->bbr_bw_lo;
  if (bw > 0) proto->set_bw(bw);
  if (bbr->bbr_min_rtt > 0) proto->set_min_rtt(bbr->bbr_min_rtt);
  if (bbr->bbr_pacing_gain > 0) proto->set_pacing_gain(bbr->bbr_pacing_gain);
  if (bbr->bbr_cwnd_gain > 0) proto->set_cwnd_gain(bbr->bbr_cwnd_gain);
}

#define TCPI_HAS_OPT(info, opt) !!(info->tcpi_options & (opt))

// Parse rtattr message containing TCP info.
// <rta> Non-null pointer to message.
// <proto> Non-null pointer to protobuf.
void ParseTCPInfo(const struct rtattr* rta, TCPInfoProto* proto) {
  const struct tcp_info* info;
  int len = RTA_PAYLOAD(rta);

  /* workaround for older kernels with fewer fields */
  if (len < sizeof(*info)) {
    info = (const struct tcp_info*)alloca(sizeof(*info));
    memcpy((char *)info, (char*)RTA_DATA(rta), len);
    memset((char *)info + len, 0, sizeof(*info) - len);
  } else {
    info = (const struct tcp_info*)RTA_DATA(rta);
  }

#define SET_TCP_IF_NONZERO(field) \
  if (info->tcpi_##field) \
    proto->set_##field(info->tcpi_##field)
#define SET_TCP_FIELD(field) \
    proto->set_##field(info->tcpi_##field)

  proto->set_state(TCPState(info->tcpi_state));  // error checking?
  SET_TCP_FIELD(ca_state);
  SET_TCP_IF_NONZERO(retransmits);
  SET_TCP_IF_NONZERO(probes);
  SET_TCP_IF_NONZERO(backoff);
  SET_TCP_FIELD(options);
  // congestion?
  if (TCPI_HAS_OPT(info, TCPI_OPT_WSCALE)) {
    proto->set_snd_wscale(info->tcpi_snd_wscale);
    proto->set_rcv_wscale(info->tcpi_rcv_wscale);
  }
  // delivery_rate_app_limited ...
  //
  if (info->tcpi_rto && info->tcpi_rto != 3000000)
    proto->set_rto(info->tcpi_rto);
  SET_TCP_IF_NONZERO(ato);
  SET_TCP_IF_NONZERO(ato);
  SET_TCP_IF_NONZERO(snd_mss);
  SET_TCP_IF_NONZERO(rcv_mss);

  SET_TCP_IF_NONZERO(unacked);
  SET_TCP_IF_NONZERO(sacked);
  SET_TCP_IF_NONZERO(lost);
  SET_TCP_IF_NONZERO(retrans);
  SET_TCP_IF_NONZERO(fackets);  // ???

  SET_TCP_FIELD(last_data_sent);
  SET_TCP_FIELD(last_ack_sent);
  SET_TCP_FIELD(last_data_recv);
  SET_TCP_FIELD(last_ack_recv);

  SET_TCP_IF_NONZERO(pmtu);
  if (info->tcpi_rcv_ssthresh < 0xFFFF)
    proto->set_rcv_ssthresh(info->tcpi_rcv_ssthresh);
  SET_TCP_IF_NONZERO(rtt);
  SET_TCP_IF_NONZERO(rttvar);
  SET_TCP_IF_NONZERO(snd_ssthresh);
  SET_TCP_IF_NONZERO(snd_cwnd);
  SET_TCP_IF_NONZERO(advmss);
  SET_TCP_IF_NONZERO(reordering);

  SET_TCP_IF_NONZERO(rcv_rtt);
  SET_TCP_IF_NONZERO(rcv_space);
  SET_TCP_IF_NONZERO(total_retrans);

  SET_TCP_IF_NONZERO(pacing_rate);
  SET_TCP_IF_NONZERO(max_pacing_rate);
  SET_TCP_IF_NONZERO(bytes_acked);
  SET_TCP_IF_NONZERO(bytes_received);

  SET_TCP_IF_NONZERO(segs_out);
  SET_TCP_IF_NONZERO(segs_in);

  SET_TCP_IF_NONZERO(notsent_bytes);
  SET_TCP_IF_NONZERO(min_rtt);
  SET_TCP_IF_NONZERO(data_segs_in);
  SET_TCP_IF_NONZERO(data_segs_out);

  SET_TCP_IF_NONZERO(delivery_rate);
#undef SET_TCP_IF_NONZERO
#undef SET_TCP_FIELD
}

// Parse rtattr message containing mem info.
// <rta> Non-null pointer to message.
// <proto> Non-null pointer to protobuf.
void ParseMemInfo(const struct rtattr* rta,
                  SocketMemInfoProto* proto) {
  const auto* info = (const struct inet_diag_meminfo*)RTA_DATA(rta);
  proto->set_rmem_alloc(info->idiag_rmem);
  proto->set_wmem_alloc(info->idiag_wmem);
  proto->set_fwd_alloc(info->idiag_fmem);
  proto->set_tmem(info->idiag_tmem);
}

// Parse rtattr message containing SK mem info.
// <rta> Non-null pointer to message.
// <proto> Non-null pointer to protobuf.
void ParseSKMemInfo(const struct rtattr* rta,
                  SocketMemInfoProto* proto) {
  const __u32* info = (__u32*)RTA_DATA(rta);
#define SET_MEM_FIELD_IF_NONZERO(tag, field) \
  if (info[SK_MEMINFO_##tag] > 0) \
    proto->set_##field(info[SK_MEMINFO_##tag])
  SET_MEM_FIELD_IF_NONZERO(RMEM_ALLOC, rmem_alloc);
  SET_MEM_FIELD_IF_NONZERO(RCVBUF, rcvbuf);
  SET_MEM_FIELD_IF_NONZERO(WMEM_ALLOC, wmem_alloc);
  SET_MEM_FIELD_IF_NONZERO(SNDBUF, sndbuf);
  SET_MEM_FIELD_IF_NONZERO(FWD_ALLOC, fwd_alloc);
  SET_MEM_FIELD_IF_NONZERO(WMEM_QUEUED, wmem_queued);
  SET_MEM_FIELD_IF_NONZERO(OPTMEM, optmem);

  if (RTA_PAYLOAD(rta) < (SK_MEMINFO_BACKLOG + 1) * sizeof(__u32))
    return;
  SET_MEM_FIELD_IF_NONZERO(BACKLOG, backlog);

  if (RTA_PAYLOAD(rta) < (SK_MEMINFO_DROPS + 1) * sizeof(__u32))
    return;
  SET_MEM_FIELD_IF_NONZERO(DROPS, drops);
#undef SET_MEM_FIELD_IF_NONZERO
}
}  // anonymous namespace

// Create a string representation of an IP endpoint.
std::string ToString(const EndPoint& ep) {
  char result[64];
  const auto& ip = ep.ip();
  if (ip.size() == 4) {
    sprintf(result, "%u.%u.%u.%u:%u",
            (unsigned char)ip[0], (unsigned char)ip[1],
            (unsigned char)ip[2], (unsigned char)ip[3], ep.port());
  } else if (ip.size() == 16) {
    sprintf(
        result, "%x:%x:%x:%x:%x:%x:%x:%x:%u",
        extract(ip, 0), extract(ip, 2), extract(ip, 4), extract(ip, 6),
        extract(ip, 8), extract(ip, 10), extract(ip, 12), extract(ip, 14),
            ep.port());
  } else {
    // Should never happen.
    // TODO - should log an error.
    return "Unrecognized address size.";
  }
  return std::string(result);
}

TCPDiagnosticsProto TCPInfoParser::ParseNLMsg(
    const struct nlmsghdr* msg, Protocol protocol) const {
  TCPDiagnosticsProto proto;
  NLMsgToProto(msg, protocol, &proto);
  return proto;
}

void TCPInfoParser::NLMsgToProto(const struct nlmsghdr* nlh,
                                 Protocol protocol,
                                 TCPDiagnosticsProto* proto) const {
  struct inet_diag_msg *r = (struct inet_diag_msg*)NLMSG_DATA(nlh);

  ParseInetDiagMsg(r, proto->mutable_inet_diag_msg());

  // This may be overwritten below.
  proto->set_diag_protocol(protocol);

  auto* rta = (struct rtattr *)(r+1);
  int len = nlh->nlmsg_len - NLMSG_LENGTH(sizeof(*r));
  while (RTA_OK(rta, len)) {
    switch (rta->rta_type) {
      case INET_DIAG_PROTOCOL:
        fprintf(stderr, "####### Protocol message\n");  // DO NOT SUBMIT
        // TODO(gfr) Consider checking for equality, and LOG_FIRST_N.
        proto->set_diag_protocol(Protocol(*(__u8 *)RTA_DATA(rta)));
        break;
      case INET_DIAG_INFO:
        ParseTCPInfo(rta, proto->mutable_tcp_info());
        break;
      case INET_DIAG_CONG:
        proto->set_congestion_algorithm(rta_getattr_str(rta));
        break;
      case INET_DIAG_SHUTDOWN:
        proto->set_shutdown_mask(*(__u8 *)RTA_DATA(rta));
        break;
      case INET_DIAG_SKMEMINFO:
        proto->clear_socket_mem();
        ParseSKMemInfo(rta, proto->mutable_socket_mem());
        break;
      case INET_DIAG_MEMINFO:
        if (!proto->has_socket_mem()) {
          ParseMemInfo(rta, proto->mutable_socket_mem());
        }
      case INET_DIAG_BBRINFO:
        ParseBBRInfo(rta, proto->mutable_bbr_info());
        break;
      case INET_DIAG_VEGASINFO:
        fprintf(stderr, "Need to do vegas\n");
        break;
      case INET_DIAG_SKV6ONLY:
        // TODO(gfr) Do we need this?
        break;
      default:
        // TODO(gfr) - should LOG(WARNING) on missing cases.
        ;
    }
    rta = RTA_NEXT(rta,len);
  }
  if (len)
    fprintf(stderr, "!!!Deficit %d, rta_len=%d\n", len, rta->rta_len);
}

bool ConnectionFilter::Accept(
    const mlab::netlink::InetSocketIDProto& socket) const {
  return true;
}
bool ConnectionFilter::Accept(const struct nlmsghdr* msg) const {
  return true;
}

void TCPInfoPoller::PollOnce() {}
void TCPInfoPoller::PollContinuously(uint polling_interval_msec) {}

void TCPInfoPoller::Break() {}
void TCPInfoPoller::ClearCache() {}
void TCPInfoPoller::ClearFilters() {}
bool TCPInfoPoller::ClearFilter(ConnectionFilter::Token token) {
  return false;
}

ConnectionFilter::Token TCPInfoPoller::AddFilter(ConnectionFilter filter,
                               Handler on_close, Handler on_change) {
  return ConnectionFilter::Token();
}

ConnectionFilter::Token TCPInfoPoller::AddFilter(ConnectionFilter filter) {
  return ConnectionFilter::Token();
}

void TCPInfoPoller::OnClose(Handler handler) {}
void TCPInfoPoller::OnChange(Handler handler) {}

} // namespace netlink
} // namespace mlab

