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
* This module defines:
*   class ConnectionTracker
******************************************************************************/

#ifndef MLAB_CONNECTION_CACHE_H
#define MLAB_CONNECTION_CACHE_H

#include <unordered_map>
#include <functional>

extern "C" {
#include <linux/inet_diag.h>
#include <linux/netlink.h>
}

namespace mlab {

/*****************************************************************************
* This class provides a cache to collect arbitrary data associated with tcp
* connections.  It is intended to be used to cache tcpinfo and related data,
* allow it to be updated (swapping the new for old data for a connection),
* and iterate with a visitor over any stale connections.
*
* Thread compatible.
****************************************************************************/
class ConnectionTracker {
 public:
  struct Record {
    int round;  // Used to track which records have been updated.
    int protocol;
    std::string msg;  // String containing (possibly binary) cached data.
  };

  using ConnectionMap = std::unordered_map<size_t, Record>;

  // Locate connection record, update round, and swap its data.
  // Returns true if this is a new connection.
  // `data` is arbitrary (possibly binary) data to be cached under key.
  // It will typically by nlmsg data.
  bool UpdateRecord(size_t key, int protocol, std::string* data);

  // Compute key from socket <id>, and call UpdateRecord on non-local
  // connections.
  // Return previous record message contents, which will be empty if this
  // is a new connection, or "Ignoring local" if this is a local connection.
  std::string UpdateFromNLMsg(int family, int protocol,
                              const struct ::inet_diag_sockid id,
                              const struct ::nlmsghdr* nlh);

  // Iterate through the map, identifying all items that were not updated on
  // the previous round.  Execute provided function on such items, and remove
  // them from the cache.  Note: `new_msg` function arg will be empty, and is
  // only included for consistency with other visitor types.
  void VisitMissingRecords(
      std::function<void(int protocol, const std::string& old_msg,
                         const std::string& new_msg)> visitor);

  int Clear() {
    int count = size();
    connections_.clear();
    return count;
  }

  // Increment the round number, which is used to determine records that have
  // not been updated in the last round.
  void increment_round() { ++round_; }  // Don't care about wrapping.
  size_t size() const { return connections_.size(); }

 private:
  ConnectionMap connections_;
  int round_ = 1;
};
}  // namespace mlab

#endif // MLAB_CONNECTION_CACHE_H
