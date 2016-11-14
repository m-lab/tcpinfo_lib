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

// TODO(gfr) The include order matters.  If tcpinfo_lib.h is included later,
// we get obscure compiler errors processing tcpinfo.pb.h.
#include "tcpinfo_lib.h"

#include <string>

#include "gtest/gtest.h"
#include "connection_cache.h"

extern "C" {
#include <netinet/in.h>  // IPPROTO_TCP
#include <sys/socket.h>  // AF_INET*

#include "tcpinfo_c_adapter.h"
}

extern mlab::netlink::TCPInfoPoller g_poller_;

//   0      0       2620:0:1003:413:ac8f:7971:3973:b48e:38625
//   2607:f8b0:4006:80d::200e:https  timer:(keepalive 31sec 0), uid:148024,
//   ino:89694138, sk:0, <->, skmem:(r0,rb369280,t0,tb87552,f0,w0,o0,bl0) ts,
//   sack, cubic, wscale:7/7, rto:208, rtt:8.5/11, ato:40, mss:1398, cwnd:19,
//   ssthresh:18, send 25.0M,bps, lastsnd:239268, lastrcv:239268, lastack:13972,
//   rcv_rtt:36, rcv_space:28800
std::string raw1(
    "\x10\x01\x00\x00\x14\x00\x02\x00\x40\xE2\x01\x00\xA8\x4B\x00\x00\x0A\x01"
    "\x02\x00\x96\xE1\x01\xBB\x26\x20\x00\x00\x10\x03\x04\x13\xAC\x8F\x79\x71"
    "\x39\x73\xB4\x8E\x26\x07\xF8\xB0\x40\x06\x08\x0D\x00\x00\x00\x00\x00\x00"
    "\x20\x0E\x00\x00\x00\x00\x00\x1F\xF8\x35\x00\x88\xFF\xFF\x6C\x79\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x38\x42\x02\x00\xBA\x9F\x58\x05\x05\x00"
    "\x08\x00\x00\x00\x00\x00\x14\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x24\x00\x07\x00\x00\x00\x00\x00\x80\xA2"
    "\x05\x00\x00\x00\x00\x00\x00\x56\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x6C\x00\x02\x00\x01\x00\x00\x00\x00\x07"
    "\x77\x00\x80\x2C\x03\x00\x40\x9C\x00\x00\x76\x05\x00\x00\x4A\x02\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\xA4\xA6\x03\x00\x00\x00\x00\x00\xA4\xA6\x03\x00\x94\x36\x00\x00"
    "\xDC\x05\x00\x00\xD8\x7D\x00\x00\x34\x21\x00\x00\xF8\x2A\x00\x00\x12\x00"
    "\x00\x00\x13\x00\x00\x00\x94\x05\x00\x00\x03\x00\x00\x00\xA0\x8C\x00\x00"
    "\x80\x70\x00\x00\x00\x00\x00\x00\x0A\x00\x04\x00\x63\x75\x62\x69\x63\x00"
    "\x00\x00",
    0x110);

namespace mlab {
// Simple visitor that just counts the visited records.
int visit_count = 0;
bool saw_msg1 = false;

void visitor(int protocol, const std::string& old_msg,
             const std::string& new_msg) {
  saw_msg1 |= (old_msg == raw1);
  visit_count++;
}

namespace netlink {

// When running without ESTABLISHED state, this generally produces
// no results, so it is useless.
TEST(TCPInfoLib, DISABLED_Basic) {
  visit_count = 0;
  TCPInfoPoller poller;
  poller.PollOnce();  // This should populate the poller's tracker object.

  // Not quite intended use, but useful testing hack.
  g_poller_.GetTracker()->VisitMissingRecords(visitor);
  g_poller_.GetTracker()->increment_round();

  // CAUTION: This may be flaky since it depends on number of
  // TCP connections on the machine.
  EXPECT_GT(visit_count, 2);
}

TEST(TCPInfoLib, CAdapters) {
  visit_count = 0;
  auto* hdr = (struct nlmsghdr*)raw1.c_str();
  struct inet_diag_arg diag_arg = {
    .protocol = IPPROTO_TCP,
  };

  update_record(nullptr, hdr, (void *)&diag_arg);
  g_poller_.GetTracker()->increment_round();
  // Not quite intended use, but useful testing hack.
  g_poller_.GetTracker()->VisitMissingRecords(visitor);
  EXPECT_EQ(visit_count, 1);
}

TEST(TCPInfoLib, ToString) {
  EndPoint ep4;
  ep4.set_ip("\x01\x02\x03\x04");
  ep4.set_port(1234);
  EXPECT_EQ(ToString(ep4), "1.2.3.4:1234");

  EndPoint ep6;
  ep6.set_ip("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10");
  ep6.set_port(4321);
  EXPECT_EQ(ToString(ep6), "[102:304:506:708:90a:b0c:d0e:f10]:4321");
}

TEST(TCPInfoLib, OnClose) {
  visit_count = 0;
  saw_msg1 = false;

  // Insert some artificial items into the cache.
  auto* hdr = (struct nlmsghdr*)raw1.c_str();
  struct inet_diag_arg diag_arg = {
    .protocol = IPPROTO_TCP,
  };
  update_record(nullptr, hdr, (void *)&diag_arg);
  g_poller_.GetTracker()->increment_round();

  // Now set up the poller, and configure to run visitor on close.
  g_poller_.OnClose(visitor);

  // When we run PollOnce, it should iterate over cache entries, and call
  // visitor for each old item that isn't updated.  The visitor should see
  // raw, so saw_msg1 should be true.
  g_poller_.PollOnce();
  EXPECT_EQ(visit_count, 1);
  EXPECT_TRUE(saw_msg1);
}

}  // namespace netlink
}  // namespace mlab