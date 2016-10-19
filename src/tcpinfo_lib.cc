
#include "tcpinfo_lib.h"

namespace mlab {
namespace netlink {

TCPDiagnosticsProto TCPInfoParser::ParseNLMsg(
    const struct nlmsghdr* msg) const {
  return std::move(TCPDiagnosticsProto());
}

void TCPInfoParser::NLMsgToProto(const struct nlmsghdr* msg,
                                 TCPDiagnosticsProto* proto) const {
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
                               Handler on_close, Handler on_change) {}

ConnectionFilter::Token TCPInfoPoller::AddFilter(ConnectionFilter filter) {}

void TCPInfoPoller::OnClose(Handler& handler) {}
void TCPInfoPoller::OnChange(Handler& handler) {}

} // namespace netlink
} // namespace mlab

