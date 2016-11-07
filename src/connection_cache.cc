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

#include "connection_cache.h"


#include <fstream>
#include <string>
#include <unistd.h>
#include <unordered_map>

extern "C" {
#include <linux/inet_diag.h>
#include <sys/socket.h>
}

namespace mlab {

bool ConnectionTracker::UpdateRecord(size_t key, int protocol,
                                     std::string* data) {
  const auto& it = connections_.find(key);
  if (it == connections_.end()) {
    // TODO - optimize this.
    auto& entry = connections_[key];
    entry.round = round_;
    entry.protocol = protocol;  // Should we compare against previous?
    entry.msg.swap(*data);
    return true;
  } else {
    // TODO(gfr) In addition to stashing the data, this should also parse
    // and compute various stats, e.g. min rtt.
    if (it->second.round == round_) {
      fprintf(stderr, "!!!!Double update. %d %lu\n", round_, key);
    }
    it->second.round = round_;
    it->second.protocol = protocol;  // Should we compare against previous?
    it->second.msg.swap(*data);
    return false;
  }
}

// Iterate through the map, find any items that are from previous
// round, and take action on them.
void ConnectionTracker::VisitMissingRecords(
    void (*visitor)(const Record& record)) {
  for (auto it = connections_.begin(); it != connections_.end();) {
    if (it->second.round != round_) {
      visitor(it->second);
      it = connections_.erase(it);
    } else {
      ++it;
    }
  }
}

namespace {
// We want simple hash function for connection map, and there isn't a
// good one in standard libraries.  This one is derived from boost.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// This will be used for small numbers of connection (< 1000), and
// consequences of a collision are modest.  Since the raw data also
// contains the full connection info, we can detect collisions later
// if we care.
inline void hash_combine(std::size_t& seed) {}
template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  hash_combine(seed, rest...);
}

// Returns zero if endpoints are on same IP address.
size_t HashConnection(const struct inet_diag_sockid id, int family) {
  size_t key = id.idiag_sport;
  hash_combine(key, id.idiag_dport);
  // We don't need to track sockets where the remote endpoint is localhost.
  bool endpoints_are_same = true;
  // TODO - are there other possible lengths we need to worry about?
  int num_words = (family == AF_INET) ? 1 : 4;
  for (int word = 0; word < num_words; ++word) {
    endpoints_are_same &= (id.idiag_src[word] == id.idiag_dst[word]);
    hash_combine(key, id.idiag_src[word], id.idiag_dst[word]);
  }
  return endpoints_are_same ? 0 : key;
}
}  // anonymous namespace

bool ConnectionTracker::UpdateFromNLMsg(int family, int protocol,
                                        const struct inet_diag_sockid id,
                                        const struct nlmsghdr* nlh) {
  size_t key = HashConnection(id, family);
  if (key == 0) return false;
  std::string data(reinterpret_cast<const char*>(nlh), nlh->nlmsg_len);
  return UpdateRecord(key, protocol, &data);
}

}  // namespace mlab
