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

#include "gtest/gtest.h"

#include <string>

#include "connection_cache.h"
extern "C" {
#include <netinet/in.h>  // IPPROTO_TCP
#include <sys/socket.h>  // AF_INET*
}

namespace mlab {
// Simple visitor that just counts the visited records.
int visit_count = 0;
void visitor(int protocol, const std::string& old_msg,
             const std::string& new_msg) { visit_count++; }

TEST(ConnectionTracker, Basic) {
  ConnectionTracker cache;
  std::string s1("string 1");
  std::string s2("string 2");
  cache.UpdateRecord(1, 99, &s1);
  EXPECT_TRUE(s1.empty());
  cache.UpdateRecord(2, 99, &s2);
  EXPECT_TRUE(s2.empty());
  cache.increment_round();

  // Now we update record 1, swapping its string value.
  // Visitor should then visit record 2.
  std::string swap("swap");
  cache.UpdateRecord(1, 99, &swap);
  EXPECT_EQ(swap, "string 1");
  cache.VisitMissingRecords(visitor);
  EXPECT_EQ(visit_count, 1);
  cache.increment_round();

  // Now we update record 1 again, swapping its string value.  Record 2 was
  // removed by visitor last time, so visitor should not change count.
  cache.UpdateRecord(1, 99, &swap);
  EXPECT_EQ(swap, "swap");
  cache.VisitMissingRecords(visitor);
  EXPECT_EQ(visit_count, 1);
  cache.increment_round();
}

TEST(ConnectionTracker, UpdateFromNLMsg) {
  visit_count = 0;
  ConnectionTracker cache;
  struct inet_diag_sockid id = {
      .idiag_sport = 1,
      .idiag_dport = 2,
      .idiag_src = {54321, 0, 0, 0},
      .idiag_dst = {12345, 0, 0, 0},
      .idiag_if = 0,
  };
  struct nlmsghdr hdr {
    .nlmsg_len = 16, .nlmsg_type = 0, .nlmsg_flags = 0, .nlmsg_seq = 123,
    .nlmsg_pid = 0,
  };
  EXPECT_TRUE(cache.UpdateFromNLMsg(AF_INET, IPPROTO_TCP, id, &hdr).empty());
  id.idiag_sport = 4;
  id.idiag_dport = 5;
  EXPECT_TRUE(cache.UpdateFromNLMsg(AF_INET, IPPROTO_TCP, id, &hdr).empty());
  cache.increment_round();
  cache.VisitMissingRecords(visitor);
  EXPECT_EQ(visit_count, 2);
}

}  // namespace mlab
