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
// This defines a cache to collect arbitrary data associated with tcp
// connections.  It is intended to be used to cache tcpinfo and related data,
// allow it to be updated (swapping the new for old data for a connection),
// and iterate with a visitor over any stale connections.

#include <unordered_map>

extern "C" {
#include <linux/inet_diag.h>
#include <linux/netlink.h>
}

namespace mlab {

struct Record {
  int round;
  int protocol;
  std::string msg;
};

class ConnectionTracker {
 public:
  using ConnectionMap = std::unordered_map<size_t, Record>;

  // Locate connection record, update round, and swap its data.
  // Returns true if this is a new connection.
  bool UpdateRecord(size_t key, int protocol, std::string* data);

  // Starting from nlhmsg, and sockid, compute hash, and UpdateRecord.
  // Returns true if this is a new connection.
  bool UpdateFromNLMsg(int family, int protocol,
                       const struct ::inet_diag_sockid id,
                       const struct ::nlmsghdr* nlh);

  // Visit all records that were not updated on this round.
  void VisitMissingRecords(void (*visitor)(const Record& record));

  void increment_round() { ++round_; }  // Don't care about wrapping.
  size_t size() const { return connections_.size(); }

 private:
  ConnectionMap connections_;
  int round_ = 1;
};

extern ConnectionTracker g_tracker;
}  // namespace mlab
