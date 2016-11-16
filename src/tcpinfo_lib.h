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

/*****************************************************************************
* Support for fetching tcp measurements through netlink library.
*   class ConnectionFilter - filters which connections are reported.
*   class TCPInfoPoller - polls, caches, and reports tcpinfo data.
*   class TCPInfoParser - parses raw tcpinfo data into protobufs.
******************************************************************************/

#ifndef MLAB_TCPINFO_LIB_H_
#define MLAB_TCPINFO_LIB_H_

#include "connection_cache.h"
#include "tcpinfo.pb.h"
#include "gtest/gtest_prod.h"

extern "C" {
#include <linux/inet_diag.h>  // Should come from iproute2 submodule.
#include <linux/netlink.h>
}

#include <functional>

namespace mlab {
namespace netlink {

// Key interface elements:
//  1. Poll all tcp connections, and maintain cache of measurements for each
//     connection.
//  2. On connection close, create a mlab.netlink.TCPDiagnosticsProto and
//     forward it to an arbitrary handler function.

// Utility function to create string representation for ipv4 or ipv6 endpoint.
std::string ToString(const EndPoint& ep);

// Utility function to extract the state from string containing nlmsg.
// If string is empty, returns INVALID.
TCPState GetStateFromStr(const std::string& data);

/*****************************************************************************
* Class to parse a single nlmsg struct, and produce a TCPDiagnosticsProto.
* It is a class, so that we may later extend it to support filtering and other
* configured behavior.
*****************************************************************************/
class TCPInfoParser {
 public:
  // `msg` ownership NOT transfered.
  // `protocol` fallback if it is not specified in msg.
  TCPDiagnosticsProto ParseNLMsg(const struct nlmsghdr* msg,
                                 Protocol protocol) const;

  // `protocol` fallback if it is not specified in msg.
  TCPDiagnosticsProto ParseNLMsg(const std::string& msg,
                                 Protocol protocol) const {
    auto* hdr = (const struct nlmsghdr*)msg.c_str();
    return ParseNLMsg(hdr, mlab::netlink::Protocol(protocol));
  }

  // `msg` ownership NOT transfered.
  // `protocol` fallback if it is not specified in msg.
  // `proto` output protobuf into which msg will be parsed.
  void NLMsgToProto(const struct nlmsghdr* msg, Protocol protocol,
                    TCPDiagnosticsProto* proto) const;
};

/*****************************************************************************
* A connection filter to check whether a connection should be reported or not.
*****************************************************************************/
class ConnectionFilter {
 public:
  // Token to be used for removing filters.
  struct Token {};

  bool Accept(const mlab::netlink::InetSocketIDProto& socket) const;
  bool Accept(const struct nlmsghdr* msg) const;
};

/*****************************************************************************
* Class to poll the status of TCP connections, and call a handler on
* specified conditions.
*
* Trigger conditions include:
*   1. Connection closed (once per connection).
*   2. Change observed (once per polling cycle on active connections).
* Filtering:
*   All triggering may be restricted to 4-tuples that match the union of a set
*   of ConnectionFilters.  This is not very efficient, however, as it is done
*   as post-processing step after fetching data for ALL TCP connections.
******************************************************************************/

class TCPInfoPoller {
 public:
  // Handler to be called when an event occurs.
  // Message ownership is NOT transfered, and handler code should not retain
  // references to the message after returning.
  // `old_nlmsg` std::string containing previous binary nlmsg data from cache.
  // `new_nlmsg` std::string containing new binary nlmsg data.
  using Handler = std::function<void(int protocol,
                                     const std::string& old_nlmsg,
                                     const std::string& new_nlmsg)>;


  // Request netlink data once, run any triggered behaviors, and update the
  // data cache.
  void PollOnce();

  // Request netlink data in polling loop, running any triggered behaviors.
  void PollContinuously(uint polling_interval_msec = 100);

  // Stop polling.
  void Break();

  // Clear the connection cache.
  // Returns the number of records in the cache.
  int ClearCache();

  // Remove all connection filters.
  void ClearFilters();
  // Remove a single connection filter.
  bool ClearFilter(ConnectionFilter::Token token);

  // Add a filter for a specific handler.  Each connection accepted by
  // this filter will be handled by on_close or on_change.  This does NOT
  // prevent other handlers from also triggering on the same connections.
  //
  // For example, if we want to collect all polled data on a specific tuple
  // for NDT, we could specify the filter for that tuple, and an on_change
  // handler that reports the desired NDT data.  In practice, this would be
  // very inefficient, and we will likely use getsockopt from the NDT process
  // instead.
  ConnectionFilter::Token AddFilter(ConnectionFilter filter, Handler on_close,
                                    Handler on_change);

  // Add a filter that allows some connections to be reported by the
  // default OnClose or OnChange handler.  Reporting will trigger for any
  // connection that is accepted by ANY of the filters added through this
  // function.  Connections that do not match any filters are ignored.
  //
  // If NO filters have been added, then ALL connections are handled by the
  // Handlers specified in OnClose and OnChange calls.
  ConnectionFilter::Token AddFilter(ConnectionFilter filter);

  // Specify handlers that will be run on any events that do not match any of
  // the tuple filters.
  // `states` is a list of states that should be reported.  If empty, all
  //          states will be reported.
  void OnClose(Handler handler, const std::vector<TCPState>& states = {});
  void OnNewState(Handler handler, const std::vector<TCPState>& states = {});
  void OnChange(Handler handler, const std::vector<TCPState>& states = {});

  // This function handles new data coming in.
  void Stash(int family, int protocol,
             const struct inet_diag_sockid id,
             const struct nlmsghdr* nlh);

 private:
  FRIEND_TEST(Poller, StashAndOnClose);

  // For testing purposes.
  ConnectionTracker* GetTracker() {
    return &tracker_;
  }

  ConnectionTracker tracker_;

  void on_close_wrapper(int protocol, const std::string& old_nlmsg,
                        const std::string& new_nlmsg);

  Handler on_close_;
  uint16_t on_close_states_;

  Handler on_change_;
  uint16_t on_change_states_;

  Handler on_new_state_;
  uint16_t on_new_state_states_;
};

}  // namespace netlink
}  // namespace mlab

#endif  // MLAB_TCPINFO_LIB_H_
