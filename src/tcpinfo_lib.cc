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

namespace mlab {
namespace netlink {

TCPDiagnosticsProto TCPInfoParser::ParseNLMsg(
    const struct nlmsghdr* msg, Protocol protocol) const {
  TCPDiagnosticsProto proto;
  NLMsgToProto(msg, protocol, &proto);
  return proto;
}

void TCPInfoParser::NLMsgToProto(const struct nlmsghdr* nlh, Protocol protocol,
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

