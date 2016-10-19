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

// Support for fetching tcp measurements through netlink library.
//
// This module will include several elements.
//  Poller - monitors connection data and calls handlers on events.
//  Parser - parses a single data record into protobuf format.
//  Logger* - support for writing data records out to log files.
//        * may be moved to separate module.

#ifndef MLAB_TCPINFO_LIB_H_
#define MLAB_TCPINFO_LIB_H_

#include "tcpinfo.pb.h"

#include <functional>
#include <linux/netlink.h>

namespace mlab {
namespace netlink {

// Key interface elements:
//  1. Poll all tcp connections, and maintain cache of measurements for each
//     connection.
//  2. On connection close, create a mlab.netlink.TCPDiagnosticsProto and
//     forward it to an arbitrary functor.
//  3. Create snapshot of all ESTABLISHED connections, and return as
//     vector<mlab.netlink.TCPDiagnosticsProto>.

// TCPInfoParser parses a single nlmsg struct, and produces an
// mlab.netlink.TCPDiagnosticsProto.  It is a class, so that we may later extend
// it to support filtering and other configured behavior.
class TCPInfoParser {
 public:
  // <msg> ownership NOT transfered.
  TCPDiagnosticsProto ParseNLMsg(const struct nlmsghdr* msg);

  // <msg> ownership NOT transfered.
  void NLMsgToProto(const struct nlmsghdr* msg, TCPDiagnosticsProto* proto);
};

// A connection filter checks whether a connection should be reported or not.
class ConnectionFilter {
 public:
  bool Accept(const mlab::netlink::InetSocketIDProto& socket);
  bool Accept(const struct nlmsghdr* msg);
};

// Instance that polls the status of TCP connections, and calls a handler on
// specified conditions.
// Trigger conditions include:
//   1. Connection closed (once per connection).
//   2. Change observed (once per polling cycle on active connections).
// Filtering:
//   All triggering may be restricted to 4-tuples that match the union of a set
//   of ConnectionFilters.

// Handler to be called when an event occurs.
// Message ownership is NOT transfered, and handler code should not retain
// references to the message after returning.
using Handler = std::function<void (struct nlmsghdr* msg)>;

// Token for later removing a filter.
struct FilterToken {};

class TCPInfoPoller {
 public:

  // Request netlink data once, run any triggered behaviors, and update the
  // data cache.
  void PollOnce();

  // Request netlink data in polling loop, running any triggered behaviors.
  void PollContinuously(uint polling_interval_msec = 100);

  void Break();  // Stop polling.

  // Clear the cache of per tuple data.
  void ClearCache();

  // Remove all whitelist filters.
  void ClearFilters();
  // Remove a single whitelist filter.
  bool ClearFilter(FilterToken token);

  // This adds a filter for a specific handler.  Each connection accepted by
  // this filter will be handled by on_close or on_change.  This does NOT
  // prevent other handlers from also triggering on the same connections.
  //
  // For example, if we want to collect all polled data on a specific tuple
  // for NDT, we could specify the filter for that tuple, and an on_change
  // handler that reports the desired NDT data.
  FilterToken AddFilter(ConnectionFilter filter, Handler on_close, Handler on_change);

  // This adds a filter that allows some connections to be reported by the
  // default OnClose or OnChange handler.  Reporting will trigger for any
  // connection that is accepted by ANY of the filters added through this
  // function.
  FilterToken AddFilter(ConnectionFilter filter);

  // Specify handlers that will be run on any events that do not match any of
  // the tuple filters.
  void OnClose(Handler& handler);
  void OnChange(Handler& handler);

};

} // namespace netlink
} // namespace mlab

#endif  // MLAB_TCPINFO_LIB_H_
