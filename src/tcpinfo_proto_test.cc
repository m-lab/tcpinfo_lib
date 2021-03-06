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

// Placeholder for more interesting tests.

#include "gtest/gtest.h"
#include "tcpinfo.pb.h"
#include "tcpinfo_lib.h"

namespace mlab {
namespace netlink {
// These samples (raw{1,2,3,4}) are collected from stripped down ss.c
// that also dumps the raw data.  Would be good to have more variability.

//   0      0       2620:0:1003:413:ac8f:7971:3973:b48e:38625
//   2607:f8b0:4006:80d::200e:https  timer:(keepalive 31sec 0), uid:148024,
//   ino:89694138, sk:0, <->, skmem:(r0,rb369280,t0,tb87552,f0,w0,o0,bl0) ts,
//   sack, cubic, wscale:7/7, rto:208, rtt:8.5/11, ato:40, mss:1398, cwnd:19,
//   ssthresh:18, send 25.0M,bps, lastsnd:239268, lastrcv:239268, lastack:13972,
//   rcv_rtt:36, rcv_space:28800
//   PLUS fake rtattr containing bbrinfo.
std::string raw1(
    "\x28\x01\x00\x00\x14\x00\x02\x00\x40\xE2\x01\x00\xA8\x4B\x00\x00"
    "\x0A\x01\x02\x00\x96\xE1\x01\xBB\x26\x20\x00\x00\x10\x03\x04\x13"
    "\xAC\x8F\x79\x71\x39\x73\xB4\x8E\x26\x07\xF8\xB0\x40\x06\x08\x0D"
    "\x00\x00\x00\x00\x00\x00\x20\x0E\x00\x00\x00\x00\x00\x1F\xF8\x35"
    "\x00\x88\xFF\xFF\x6C\x79\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x38\x42\x02\x00\xBA\x9F\x58\x05\x05\x00\x08\x00\x00\x00\x00\x00"
    "\x14\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x24\x00\x07\x00\x00\x00\x00\x00\x80\xA2\x05\x00"
    "\x00\x00\x00\x00\x00\x56\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x6C\x00\x02\x00\x01\x00\x00\x00"
    "\x00\x07\x77\x00\x80\x2C\x03\x00\x40\x9C\x00\x00\x76\x05\x00\x00"
    "\x4A\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\xA4\xA6\x03\x00\x00\x00\x00\x00"
    "\xA4\xA6\x03\x00\x94\x36\x00\x00\xDC\x05\x00\x00\xD8\x7D\x00\x00"
    "\x34\x21\x00\x00\xF8\x2A\x00\x00\x12\x00\x00\x00\x13\x00\x00\x00"
    "\x94\x05\x00\x00\x03\x00\x00\x00\xA0\x8C\x00\x00\x80\x70\x00\x00"
    "\x00\x00\x00\x00\x0A\x00\x04\x00\x63\x75\x62\x69\x63\x00\x00\x00"
    "\x18\x00\x10\x00"  // len 24, type 16
    "\x12\x23\x34\x45\x01\x00\x00\x00"  // bw high and low
    "\x9a\xab\x04\x00"  // min_rtt uSec
    "\xde\xef\x01\x00"  // pacing gain << 8
    "\x23\x34\x02\x00",  // cwnd gain << 8
    0x128);

//   0      0       2620:0:1003:413:ac8f:7971:3973:b48e:54550
//   2607:f8b0:400d:c03::bd:https  timer:(keepalive 30sec 0), uid:148024,
//   ino:89698212, sk:0, <->, skmem:(r0,rb369280,t0,tb46080,f0,w0,o0,bl0) ts,
//   sack, cubic, wscale:7/7, rto:220, rtt:21.5/15, ato:40, mss:1348, cwnd:10,
//   send 5.0M,bps, lastsnd:239528, lastrcv:239528, lastack:14264, rcv_rtt:16,
//   rcv_space:28800
std::string raw2(
    "\x10\x01\x00\x00\x14\x00\x02\x00\x40\xE2\x01\x00\xA8\x4B\x00\x00\x0A\x01"
    "\x02\x00\xD5\x16\x01\xBB\x26\x20\x00\x00\x10\x03\x04\x13\xAC\x8F\x79\x71"
    "\x39\x73\xB4\x8E\x26\x07\xF8\xB0\x40\x0D\x0C\x03\x00\x00\x00\x00\x00\x00"
    "\x00\xBD\x00\x00\x00\x00\x40\x97\x84\x45\x01\x88\xFF\xFF\x3C\x78\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x38\x42\x02\x00\xA4\xAF\x58\x05\x05\x00"
    "\x08\x00\x00\x00\x00\x00\x14\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x24\x00\x07\x00\x00\x00\x00\x00\x80\xA2"
    "\x05\x00\x00\x00\x00\x00\x00\xB4\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x6C\x00\x02\x00\x01\x00\x00\x00\x00\x07"
    "\x77\x00\x60\x5B\x03\x00\x40\x9C\x00\x00\x44\x05\x00\x00\xB8\x04\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\xA8\xA7\x03\x00\x00\x00\x00\x00\xA8\xA7\x03\x00\xB8\x37\x00\x00"
    "\xDC\x05\x00\x00\x04\xA2\x00\x00\xFC\x53\x00\x00\x98\x3A\x00\x00\xFF\xFF"
    "\xFF\x7F\x0A\x00\x00\x00\x94\x05\x00\x00\x03\x00\x00\x00\x80\x3E\x00\x00"
    "\x80\x70\x00\x00\x00\x00\x00\x00\x0A\x00\x04\x00\x63\x75\x62\x69\x63\x00"
    "\x00\x00",
    0x110);

//   0      0       2620:0:1003:413:ac8f:7971:3973:b48e:42341
//   2607:f8b0:400d:c02::5f:https  timer:(keepalive 30sec 0), uid:148024,
//   ino:89698213, sk:0, <->, skmem:(r0,rb369280,t0,tb46080,f0,w0,o0,bl0) ts,
//   sack, cubic, wscale:7/7, rto:220, rtt:21.5/15, ato:40, mss:1348, cwnd:10,
//   send 5.0M,bps, lastsnd:239432, lastrcv:239432, lastack:14132, rcv_rtt:16,
//   rcv_space:28800
std::string raw3(
    "\x10\x01\x00\x00\x14\x00\x02\x00\x40\xE2\x01\x00\xA8\x4B\x00\x00\x0A\x01"
    "\x02\x00\xA5\x65\x01\xBB\x26\x20\x00\x00\x10\x03\x04\x13\xAC\x8F\x79\x71"
    "\x39\x73\xB4\x8E\x26\x07\xF8\xB0\x40\x0D\x0C\x02\x00\x00\x00\x00\x00\x00"
    "\x00\x5F\x00\x00\x00\x00\x80\xAE\x84\x45\x01\x88\xFF\xFF\xC0\x78\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x38\x42\x02\x00\xA5\xAF\x58\x05\x05\x00"
    "\x08\x00\x00\x00\x00\x00\x14\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x24\x00\x07\x00\x00\x00\x00\x00\x80\xA2"
    "\x05\x00\x00\x00\x00\x00\x00\xB4\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x6C\x00\x02\x00\x01\x00\x00\x00\x00\x07"
    "\x77\x00\x60\x5B\x03\x00\x40\x9C\x00\x00\x44\x05\x00\x00\xB8\x04\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x48\xA7\x03\x00\x00\x00\x00\x00\x48\xA7\x03\x00\x34\x37\x00\x00"
    "\xDC\x05\x00\x00\xAA\xAA\x00\x00\xFC\x53\x00\x00\x98\x3A\x00\x00\xFF\xFF"
    "\xFF\x7F\x0A\x00\x00\x00\x94\x05\x00\x00\x03\x00\x00\x00\x80\x3E\x00\x00"
    "\x80\x70\x00\x00\x00\x00\x00\x00\x0A\x00\x04\x00\x63\x75\x62\x69\x63\x00"
    "\x00\x00",
    0x110);

//   0      0       2620:0:1003:413:ac8f:7971:3973:b48e:48537
//   2607:f8b0:400d:c06::5f:https  ino:89693107, sk:0, <->,
//   skmem:(r0,rb369280,t0,tb46080,f0,w0,o0,bl0) ts, sack, cubic, wscale:7/7,
//   rto:216, rtt:16/11, ato:40, mss:1348, cwnd:10, send 6.7M,bps,
//   lastsnd:179168, lastrcv:29100, lastack:29100, rcv_rtt:16, rcv_space:28800
std::string raw4(
    "\x10\x01\x00\x00\x14\x00\x02\x00\x40\xE2\x01\x00\xA8\x4B\x00\x00\x0A\x01"
    "\x00\x00\xBD\x99\x01\xBB\x26\x20\x00\x00\x10\x03\x04\x13\xAC\x8F\x79\x71"
    "\x39\x73\xB4\x8E\x26\x07\xF8\xB0\x40\x0D\x0C\x06\x00\x00\x00\x00\x00\x00"
    "\x00\x5F\x00\x00\x00\x00\xC0\x87\xEE\x20\x01\x88\xFF\xFF\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xB3\x9B\x58\x05\x05\x00"
    "\x08\x00\x00\x00\x00\x00\x14\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x24\x00\x07\x00\x00\x00\x00\x00\x80\xA2"
    "\x05\x00\x00\x00\x00\x00\x00\xB4\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x6C\x00\x02\x00\x01\x00\x00\x00\x00\x07"
    "\x77\x00\xC0\x4B\x03\x00\x40\x9C\x00\x00\x44\x05\x00\x00\xB8\x04\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\xE0\xBB\x02\x00\x00\x00\x00\x00\xAC\x71\x00\x00\xAC\x71\x00\x00"
    "\xDC\x05\x00\x00\x20\xA1\x00\x00\x80\x3E\x00\x00\xF8\x2A\x00\x00\xFF\xFF"
    "\xFF\x7F\x0A\x00\x00\x00\x94\x05\x00\x00\x03\x00\x00\x00\x80\x3E\x00\x00"
    "\x80\x70\x00\x00\x00\x00\x00\x00\x0A\x00\x04\x00\x63\x75\x62\x69\x63\x00"
    "\x00\x00", 0x110);

std::string raw5(
    "\x0c\x01\x00\x00\x14\x00\x02\x00\x40\xe2\x01\x00\x57\x31\x00\x00"
    "\x0a\x02\x01\x00\xdd\xe4\x13\xa8\x26\x20\x00\x00\x10\x03\x04\x13"
    "\xf5\xee\x00\xcf\x00\xbb\x77\x3b\x26\x07\xf8\xb0\x40\x0d\x0c\x0a"
    "\x00\x00\x00\x00\x00\x00\x00\x81\x00\x00\x00\x00\x00\xbe\xdb\xff"
    "\x07\x88\xff\xff\xe4\x03\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00"
    "\x00\x00\x00\x00\x1d\xd8\x4a\x01\x05\x00\x08\x00\x00\x00\x00\x00"
    "\x14\x00\x01\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x0b\x00\x00"
    "\x00\x00\x00\x00\x24\x00\x07\x00\x00\x00\x00\x00\x54\x55\x01\x00"
    "\x00\x00\x00\x00\x00\x40\x00\x00\x00\x0b\x00\x00\x00\x05\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x6c\x00\x02\x00\x02\x00\x00\x00"
    "\x00\x00\x00\x00\x40\x42\x0f\x00\x00\x00\x00\x00\xb8\x04\x00\x00"
    "\x58\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x08\xac\x57\x18\x00\x00\x00\x00"
    "\x08\xac\x57\x18\x08\xac\x57\x18\xdc\x05\x00\x00\x80\x70\x00\x00"
    "\x00\x00\x00\x00\x90\xd0\x03\x00\xff\xff\xff\x7f\x0a\x00\x00\x00"
    "\xa0\x05\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x05\x00\x04\x00\x00\x00\x00\x00", 0x10c); 
std::string raw6("\x0c\x01\x00\x00"
    "\x14\x00\x02\x00\x40\xe2\x01\x00\x57\x31\x00\x00\x0a\x02\x01\x00"
    "\xdd\xe6\x13\xa8\x26\x20\x00\x00\x10\x03\x04\x13\xf5\xee\x00\xcf"
    "\x00\xbb\x77\x3b\x26\x07\xf8\xb0\x40\x0d\x0c\x0a\x00\x00\x00\x00"
    "\x00\x00\x00\x81\x00\x00\x00\x00\xc0\x26\xd9\x18\x01\x88\xff\xff"
    "\xe8\x03\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00"
    "\x8c\xc5\x4a\x01\x05\x00\x08\x00\x00\x00\x00\x00\x14\x00\x01\x00"
    "\x00\x00\x00\x00\x00\x05\x00\x00\x00\x0b\x00\x00\x00\x00\x00\x00"
    "\x24\x00\x07\x00\x00\x00\x00\x00\x54\x55\x01\x00\x00\x00\x00\x00"
    "\x00\x40\x00\x00\x00\x0b\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x6c\x00\x02\x00\x02\x00\x00\x00\x00\x00\x00\x00"
    "\x40\x42\x0f\x00\x00\x00\x00\x00\xb8\x04\x00\x00\x58\x00\x00\x00"
    "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x7c\xad\x57\x18\x00\x00\x00\x00\x7c\xad\x57\x18"
    "\x7c\xad\x57\x18\xdc\x05\x00\x00\x80\x70\x00\x00\x00\x00\x00\x00"
    "\x90\xd0\x03\x00\xff\xff\xff\x7f\x0a\x00\x00\x00\xa0\x05\x00\x00"
    "\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x05\x00\x04\x00\x00\x00\x00\x00", 0x10c);
std::string raw7("\x10\x01\x00\x00\x14\x00\x02\x00"
    "\x40\xe2\x01\x00\x57\x31\x00\x00\x0a\x01\x00\x00\xdd\xe4\x13\xa8"
    "\x26\x20\x00\x00\x10\x03\x04\x13\xf5\xee\x00\xcf\x00\xbb\x77\x3b"
    "\x26\x07\xf8\xb0\x40\x0d\x0c\x0a\x00\x00\x00\x00\x00\x00\x00\x81"
    "\x00\x00\x00\x00\x00\xbe\xdb\xff\x07\x88\xff\xff\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x1d\xd8\x4a\x01"
    "\x05\x00\x08\x00\x00\x00\x00\x00\x14\x00\x01\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x24\x00\x07\x00"
    "\x00\x00\x00\x00\x80\xa2\x05\x00\x00\x00\x00\x00\x00\xb4\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x6c\x00\x02\x00\x01\x00\x00\x00\x00\x07\x77\x00\xc0\x4b\x03\x00"
    "\x40\x9c\x00\x00\x44\x05\x00\x00\x18\x02\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\xf0\x05\x00\x00\x00\x00\x00\x00\xf0\x05\x00\x00\xe0\x05\x00\x00"
    "\xdc\x05\x00\x00\x80\x70\x00\x00\x80\x3e\x00\x00\x58\x1b\x00\x00"
    "\xff\xff\xff\x7f\x0a\x00\x00\x00\x94\x05\x00\x00\x03\x00\x00\x00"
    "\x00\x00\x00\x00\x80\x70\x00\x00\x00\x00\x00\x00\x0a\x00\x04\x00"
    "\x63\x75\x62\x69\x63\x00\x00\x00", 0x110);
std::string raw8("\x10\x01\x00\x00\x14\x00\x02\x00"
    "\x40\xe2\x01\x00\x57\x31\x00\x00\x0a\x09\x01\x00\xdd\xe4\x13\xa8"
    "\x26\x20\x00\x00\x10\x03\x04\x13\xf5\xee\x00\xcf\x00\xbb\x77\x3b"
    "\x26\x07\xf8\xb0\x40\x0d\x0c\x0a\x00\x00\x00\x00\x00\x00\x00\x81"
    "\x00\x00\x00\x00\x00\xbe\xdb\xff\x07\x88\xff\xff\x20\x00\x00\x00"
    "\x00\x00\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x05\x00\x08\x00\x03\x00\x00\x00\x14\x00\x01\x00\x00\x00\x00\x00"
    "\x00\x0e\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x24\x00\x07\x00"
    "\x00\x00\x00\x00\x80\xa2\x05\x00\x00\x00\x00\x00\x00\xb4\x00\x00"
    "\x00\x02\x00\x00\x00\x0e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x6c\x00\x02\x00\x09\x00\x00\x00\x00\x07\x77\x00\xc0\x4b\x03\x00"
    "\x40\x9c\x00\x00\x44\x05\x00\x00\x18\x02\x00\x00\x02\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x04\x00\x00\x00"
    "\xdc\x05\x00\x00\xb0\x74\x00\x00\x80\x3e\x00\x00\x58\x1b\x00\x00"
    "\xff\xff\xff\x7f\x0a\x00\x00\x00\x94\x05\x00\x00\x03\x00\x00\x00"
    "\x00\x00\x00\x00\x80\x70\x00\x00\x00\x00\x00\x00\x0a\x00\x04\x00"
    "\x63\x75\x62\x69\x63\x00\x00\x00", 0x110);
std::string raw9(
    "\x0c\x01\x00\x00\x14\x00\x02\x00"
    "\x40\xe2\x01\x00\x57\x31\x00\x00\x02\x02\x01\x00\x98\x09\x13\xa8"
    "\x64\x65\xe4\xaa\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x4a\x7d\x1d\x81\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\xdb\x1f\x7d\x01\x88\xff\xff\xe8\x03\x00\x00"
    "\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\xf5\xbe\x4a\x01"
    "\x05\x00\x08\x00\x00\x00\x00\x00\x14\x00\x01\x00\x00\x00\x00\x00"
    "\x00\x05\x00\x00\x00\x0b\x00\x00\x00\x05\x00\x00\x24\x00\x07\x00"
    "\x00\x00\x00\x00\x54\x55\x01\x00\x00\x05\x00\x00\x00\x40\x00\x00"
    "\x00\x0b\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x6c\x00\x02\x00\x02\x00\x00\x00\x00\x00\x00\x00\x40\x42\x0f\x00"
    "\x00\x00\x00\x00\x0c\x02\x00\x00\x58\x00\x00\x00\x01\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x04\xc1\x57\x18\x00\x00\x00\x00\x04\xc1\x57\x18\x04\xc1\x57\x18"
    "\xdc\x05\x00\x00\x10\x72\x00\x00\x00\x00\x00\x00\x90\xd0\x03\x00"
    "\xff\xff\xff\x7f\x0a\x00\x00\x00\xb4\x05\x00\x00\x03\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x04\x00"
    "\x00\x00\x00\x00", 0x10c);
std::string raw10(
    "\x0c\x01\x00\x00\x14\x00\x02\x00\x40\xe2\x01\x00"
    "\x57\x31\x00\x00\x0a\x02\x01\x00\xdd\xe9\x13\xa8\x26\x20\x00\x00"
    "\x10\x03\x04\x13\xf5\xee\x00\xcf\x00\xbb\x77\x3b\x26\x07\xf8\xb0"
    "\x40\x0d\x0c\x0a\x00\x00\x00\x00\x00\x00\x00\x81\x00\x00\x00\x00"
    "\xc0\x07\xa3\xfb\x07\x88\xff\xff\xe8\x03\x00\x00\x00\x00\x00\x00"
    "\x01\x00\x00\x00\x00\x00\x00\x00\x3f\x8e\x49\x01\x05\x00\x08\x00"
    "\x00\x00\x00\x00\x14\x00\x01\x00\x00\x00\x00\x00\x00\x05\x00\x00"
    "\x00\x0b\x00\x00\x00\x00\x00\x00\x24\x00\x07\x00\x00\x00\x00\x00"
    "\x54\x55\x01\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x0b\x00\x00"
    "\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x6c\x00\x02\x00"
    "\x02\x00\x00\x00\x00\x00\x00\x00\x40\x42\x0f\x00\x00\x00\x00\x00"
    "\xb8\x04\x00\x00\x58\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x3c\xc6\x57\x18"  // 000600
    "\x00\x00\x00\x00\x3c\xc6\x57\x18\x3c\xc6\x57\x18\xdc\x05\x00\x00"
    "\x80\x70\x00\x00\x00\x00\x00\x00\x90\xd0\x03\x00\xff\xff\xff\x7f"
    "\x0a\x00\x00\x00\xa0\x05\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x04\x00\x00\x00\x00\x00", 0x110);
std::string raw11(
    "\x10\x01\x00\x00\x14\x00\x02\x00\x40\xe2\x01\x00\x57\x31\x00\x00"
    "\x02\x01\x00\x00\x98\x09\x13\xa8\x64\x65\xe4\xaa\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x4a\x7d\x1d\x81\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xdb\x1f\x7d"
    "\x01\x88\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\xf5\xbe\x4a\x01\x05\x00\x08\x00\x00\x00\x00\x00"
    "\x14\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x24\x00\x07\x00\x00\x00\x00\x00\x00\xaf\x05\x00"
    "\x00\x00\x00\x00\x00\xb4\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x6c\x00\x02\x00\x01\x00\x00\x00"
    "\x00\x07\x77\x00\x20\x3c\x03\x00\x40\x9c\x00\x00\x58\x05\x00\x00"
    "\x18\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\xf0\x05\x00\x00\x00\x00\x00\x00"
    "\xf0\x05\x00\x00\xe4\x05\x00\x00\xdc\x05\x00\x00\x10\x72\x00\x00"
    "\xd4\x30\x00\x00\x88\x13\x00\x00\xff\xff\xff\x7f\x0a\x00\x00\x00"
    "\xa8\x05\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x10\x72\x00\x00"
    "\x00\x00\x00\x00\x0a\x00\x04\x00\x63\x75\x62\x69\x63\x00\x00\x00", 0x110);
std::string raw12(
    "\x10\x01\x00\x00\x14\x00\x02\x00\x40\xe2\x01\x00\x57\x31\x00\x00"
    "\x02\x09\x01\x00\x98\x09\x13\xa8\x64\x65\xe4\xaa\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x4a\x7d\x1d\x81\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xdb\x1f\x7d"
    "\x01\x88\xff\xff\xd4\x00\x00\x00\x00\x00\x00\x00\x20\x00\x00\x00"
    "\x00\x00\x00\x00\xf5\xbe\x4a\x01\x05\x00\x08\x00\x03\x00\x00\x00"
    "\x14\x00\x01\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x02\x00\x00"
    "\x00\x0e\x00\x00\x24\x00\x07\x00\x00\x00\x00\x00\x00\xaf\x05\x00"
    "\x00\x0e\x00\x00\x00\xb4\x00\x00\x00\x02\x00\x00\x00\x0e\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x6c\x00\x02\x00\x09\x00\x00\x00"
    "\x00\x07\x77\x00\x20\x3c\x03\x00\x40\x9c\x00\x00\x58\x05\x00\x00"
    "\x18\x02\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\xdc\x05\x00\x00\x40\x76\x00\x00"
    "\xd4\x30\x00\x00\x88\x13\x00\x00\xff\xff\xff\x7f\x0a\x00\x00\x00"
    "\xa8\x05\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x10\x72\x00\x00"
    "\x00\x00\x00\x00\x0a\x00\x04\x00\x63\x75\x62\x69\x63\x00\x00\x00", 0x110);
std::string raw13(
    "\x10\x01\x00\x00\x14\x00\x02\x00\x40\xe2\x01\x00\x57\x31\x00\x00"
    "\x0a\x01\x00\x00\xdd\xe9\x13\xa8\x26\x20\x00\x00\x10\x03\x04\x13"
    "\xf5\xee\x00\xcf\x00\xbb\x77\x3b\x26\x07\xf8\xb0\x40\x0d\x0c\x0a"
    "\x00\x00\x00\x00\x00\x00\x00\x81\x00\x00\x00\x00\xc0\x07\xa3\xfb"
    "\x07\x88\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x3f\x8e\x49\x01\x05\x00\x08\x00\x00\x00\x00\x00"
    "\x14\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x24\x00\x07\x00\x00\x00\x00\x00\x80\xa2\x05\x00"
    "\x00\x00\x00\x00\x00\xb4\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x6c\x00\x02\x00\x01\x00\x00\x00"
    "\x00\x07\x77\x00\x20\x3c\x03\x00\x40\x9c\x00\x00\x44\x05\x00\x00"
    "\x18\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\xf0\x05\x00\x00\x00\x00\x00\x00"
    "\xf0\x05\x00\x00\xe4\x05\x00\x00\xdc\x05\x00\x00\x80\x70\x00\x00"
    "\xd4\x30\x00\x00\x88\x13\x00\x00\xff\xff\xff\x7f\x0a\x00\x00\x00"
    "\x94\x05\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x80\x70\x00\x00"
    "\x00\x00\x00\x00\x0a\x00\x04\x00\x63\x75\x62\x69\x63\x00\x00\x00", 0x110);
std::string raw14(
    "\x10\x01\x00\x00\x14\x00\x02\x00\x40\xe2\x01\x00\x57\x31\x00\x00"
    "\x0a\x09\x01\x00\xdd\xe9\x13\xa8\x26\x20\x00\x00\x10\x03\x04\x13"
    "\xf5\xee\x00\xcf\x00\xbb\x77\x3b\x26\x07\xf8\xb0\x40\x0d\x0c\x0a"
    "\x00\x00\x00\x00\x00\x00\x00\x81\x00\x00\x00\x00\xc0\x07\xa3\xfb"
    "\x07\x88\xff\xff\x18\x00\x00\x00\x00\x00\x00\x00\x20\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x08\x00\x03\x00\x00\x00"
    "\x14\x00\x01\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x02\x00\x00"
    "\x00\x00\x00\x00\x24\x00\x07\x00\x00\x00\x00\x00\x80\xa2\x05\x00"
    "\x00\x00\x00\x00\x00\xb4\x00\x00\x00\x02\x00\x00\x00\x0e\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x6c\x00\x02\x00\x09\x00\x00\x00"
    "\x00\x07\x77\x00\x20\x3c\x03\x00\x40\x9c\x00\x00\x44\x05\x00\x00"
    "\x18\x02\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\xdc\x05\x00\x00\xb0\x74\x00\x00"
    "\xd4\x30\x00\x00\x88\x13\x00\x00\xff\xff\xff\x7f\x0a\x00\x00\x00"
    "\x94\x05\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x80\x70\x00\x00"
    "\x00\x00\x00\x00\x0a\x00\x04\x00\x63\x75\x62\x69\x63\x00\x00\x00", 0x110);
std::string raw15(
    "\x0c\x01\x00\x00\x14\x00\x02\x00\x40\xe2\x01\x00\x57\x31\x00\x00"
    "\x0a\x02\x01\x00\xdd\xeb\x13\xa8\x26\x20\x00\x00\x10\x03\x04\x13"
    "\xf5\xee\x00\xcf\x00\xbb\x77\x3b\x26\x07\xf8\xb0\x40\x0d\x0c\x0a"
    "\x00\x00\x00\x00\x00\x00\x00\x81\x00\x00\x00\x00\xc0\x64\x2d\x52"
    "\x07\x88\xff\xff\xe8\x03\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00"
    "\x00\x00\x00\x00\xe5\xd2\x4a\x01\x05\x00\x08\x00\x00\x00\x00\x00"
    "\x14\x00\x01\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x0b\x00\x00"
    "\x00\x00\x00\x00\x24\x00\x07\x00\x00\x00\x00\x00\x54\x55\x01\x00"
    "\x00\x00\x00\x00\x00\x40\x00\x00\x00\x0b\x00\x00\x00\x05\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x6c\x00\x02\x00\x02\x00\x00\x00"
    "\x00\x00\x00\x00\x40\x42\x0f\x00\x00\x00\x00\x00\xb8\x04\x00\x00"
    "\x58\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\xcf\x57\x18\x00\x00\x00\x00"
    "\x00\xcf\x57\x18\x00\xcf\x57\x18\xdc\x05\x00\x00\x80\x70\x00\x00"
    "\x00\x00\x00\x00\x90\xd0\x03\x00\xff\xff\xff\x7f\x0a\x00\x00\x00"
    "\xa0\x05\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x05\x00\x04\x00\x00\x00\x00\x00", 0x10c);
std::string raw16("\x10\x01\x00\x00"
    "\x14\x00\x02\x00\x40\xe2\x01\x00\x57\x31\x00\x00\x0a\x01\x00\x00"
    "\xdd\xeb\x13\xa8\x26\x20\x00\x00\x10\x03\x04\x13\xf5\xee\x00\xcf"
    "\x00\xbb\x77\x3b\x26\x07\xf8\xb0\x40\x0d\x0c\x0a\x00\x00\x00\x00"
    "\x00\x00\x00\x81\x00\x00\x00\x00\xc0\x64\x2d\x52\x07\x88\xff\xff"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\xe5\xd2\x4a\x01\x05\x00\x08\x00\x00\x00\x00\x00\x14\x00\x01\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x24\x00\x07\x00\x00\x00\x00\x00\x80\xa2\x05\x00\x00\x00\x00\x00"
    "\x00\xb4\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x6c\x00\x02\x00\x01\x00\x00\x00\x00\x07\x77\x00"
    "\xc0\x4b\x03\x00\x40\x9c\x00\x00\x44\x05\x00\x00\x18\x02\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\xf4\x05\x00\x00\x00\x00\x00\x00\xf4\x05\x00\x00"
    "\xe4\x05\x00\x00\xdc\x05\x00\x00\x80\x70\x00\x00\x80\x3e\x00\x00"
    "\x88\x13\x00\x00\xff\xff\xff\x7f\x0a\x00\x00\x00\x94\x05\x00\x00"
    "\x03\x00\x00\x00\x00\x00\x00\x00\x80\x70\x00\x00\x00\x00\x00\x00"
    "\x0a\x00\x04\x00\x63\x75\x62\x69\x63\x00\x00\x00", 0x110);
std::string raw17("\x10\x01\x00\x00"
    "\x14\x00\x02\x00\x40\xe2\x01\x00\x57\x31\x00\x00\x0a\x09\x01\x00"
    "\xdd\xeb\x13\xa8\x26\x20\x00\x00\x10\x03\x04\x13\xf5\xee\x00\xcf"
    "\x00\xbb\x77\x3b\x26\x07\xf8\xb0\x40\x0d\x0c\x0a\x00\x00\x00\x00"
    "\x00\x00\x00\x81\x00\x00\x00\x00\xc0\x64\x2d\x52\x07\x88\xff\xff"
    "\x20\x00\x00\x00\x00\x00\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x05\x00\x08\x00\x03\x00\x00\x00\x14\x00\x01\x00"
    "\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00"
    "\x24\x00\x07\x00\x00\x00\x00\x00\x80\xa2\x05\x00\x00\x00\x00\x00"
    "\x00\xb4\x00\x00\x00\x02\x00\x00\x00\x0e\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x6c\x00\x02\x00\x09\x00\x00\x00\x00\x07\x77\x00"
    "\xc0\x4b\x03\x00\x40\x9c\x00\x00\x44\x05\x00\x00\x18\x02\x00\x00"
    "\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\xdc\x05\x00\x00\xb0\x74\x00\x00\x80\x3e\x00\x00"
    "\x88\x13\x00\x00\xff\xff\xff\x7f\x0a\x00\x00\x00\x94\x05\x00\x00"
    "\x03\x00\x00\x00\x00\x00\x00\x00\x80\x70\x00\x00\x00\x00\x00\x00"
    "\x0a\x00\x04\x00\x63\x75\x62\x69\x63\x00\x00\x00", 0x110);
std::string raw18("\x0c\x01\x00\x00"
    "\x14\x00\x02\x00\x40\xe2\x01\x00\x57\x31\x00\x00\x0a\x02\x01\x00"
    "\xdd\xcb\x01\xbb\x26\x20\x00\x00\x10\x03\x04\x13\xf5\xee\x00\xcf"
    "\x00\xbb\x77\x3b\x26\x07\xf8\xb0\x40\x0d\x0c\x04\x00\x00\x00\x00"
    "\x00\x00\x00\x81\x00\x00\x00\x00\xc0\x87\x47\xfb\x07\x88\xff\xff"
    "\xe8\x03\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00"
    "\x78\x9f\x49\x01\x05\x00\x08\x00\x00\x00\x00\x00\x14\x00\x01\x00"
    "\x00\x00\x00\x00\x00\x05\x00\x00\x00\x0b\x00\x00\x00\x00\x00\x00"
    "\x24\x00\x07\x00\x00\x00\x00\x00\x54\x55\x01\x00\x00\x00\x00\x00"
    "\x00\x40\x00\x00\x00\x0b\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x6c\x00\x02\x00\x02\x00\x00\x00\x00\x00\x00\x00"
    "\x40\x42\x0f\x00\x00\x00\x00\x00\xb8\x04\x00\x00\x58\x00\x00\x00"
    "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\xa0\xdc\x57\x18\x00\x00\x00\x00\xa0\xdc\x57\x18"
    "\xa0\xdc\x57\x18\xdc\x05\x00\x00\x80\x70\x00\x00\x00\x00\x00\x00"
    "\x90\xd0\x03\x00\xff\xff\xff\x7f\x0a\x00\x00\x00\xa0\x05\x00\x00"
    "\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x05\x00\x04\x00\x00\x00\x00\x00", 0x10c);
std::string raw19(
    "\x10\x01\x00\x00\x14\x00\x02\x00"
    "\x40\xe2\x01\x00\x57\x31\x00\x00\x0a\x01\x00\x00\xdd\xcb\x01\xbb"
    "\x26\x20\x00\x00\x10\x03\x04\x13\xf5\xee\x00\xcf\x00\xbb\x77\x3b"
    "\x26\x07\xf8\xb0\x40\x0d\x0c\x04\x00\x00\x00\x00\x00\x00\x00\x81"
    "\x00\x00\x00\x00\xc0\x87\x47\xfb\x07\x88\xff\xff\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x78\x9f\x49\x01"
    "\x05\x00\x08\x00\x00\x00\x00\x00\x14\x00\x01\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x24\x00\x07\x00"
    "\x00\x00\x00\x00\x80\xa2\x05\x00\x00\x00\x00\x00\x00\xb4\x00\x00"
    "\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x6c\x00\x02\x00\x01\x00\x00\x00\x00\x07\x77\x00\xc0\x4b\x03\x00"
    "\x40\x9c\x00\x00\x44\x05\x00\x00\x50\x03\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x44\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x34\x00\x00\x00"
    "\xdc\x05\x00\x00\x24\xaa\x00\x00\x2c\x4c\x00\x00\x28\x23\x00\x00"
    "\xff\xff\xff\x7f\x0a\x00\x00\x00\x94\x05\x00\x00\x03\x00\x00\x00"
    "\xc0\xda\x00\x00\x80\x70\x00\x00\x00\x00\x00\x00\x0a\x00\x04\x00"
    "\x63\x75\x62\x69\x63\x00\x00\x00", 0x110);

TEST(Parser, IPToString) {
  InetDiagMsgProto p4;
  p4.set_family(InetDiagMsgProto_AddressFamily_INET);
  auto* sock_id = p4.mutable_sock_id();
  auto* src = sock_id->mutable_source();
  src->set_port(1234);
  src->set_ip("abcd", 4);
  EXPECT_EQ(ToString(p4.sock_id().source()), "97.98.99.100:1234");

  InetDiagMsgProto p6;
  p6.set_family(InetDiagMsgProto_AddressFamily_INET6);
  auto* sock_id6 = p6.mutable_sock_id();
  auto* src6 = sock_id6->mutable_source();
  src6->set_port(5678);
  src6->set_ip("abcdefghijklmnop", 16);
  EXPECT_EQ(ToString(p6.sock_id().source()),
            "[6162:6364:6566:6768:696a:6b6c:6d6e:6f70]:5678");
}

TEST(Parser, LotsOfFields) {
  TCPInfoParser parser;
  auto* hdr = (const struct nlmsghdr*)raw1.data();
  auto proto = parser.ParseNLMsg(hdr, IPPROTO_TCP);
  ASSERT_EQ(proto.diag_protocol(), IPPROTO_TCP);
  EXPECT_TRUE(proto.has_inet_diag_msg());
  EXPECT_EQ(proto.inet_diag_msg().expires(), 31084);
  EXPECT_EQ(proto.inet_diag_msg().uid(), 148024);
  EXPECT_EQ(proto.inet_diag_msg().inode(), 89694138);

  ASSERT_TRUE(proto.has_tcp_info());
  EXPECT_EQ(proto.tcp_info().state(), TCPState::ESTABLISHED);
  EXPECT_EQ(proto.tcp_info().snd_wscale(), 7);
  EXPECT_EQ(proto.tcp_info().rcv_wscale(), 7);

  EXPECT_EQ(proto.tcp_info().rto(), 208000);
  EXPECT_EQ(proto.tcp_info().ato(), 40000);
  EXPECT_EQ(proto.tcp_info().snd_mss(), 1398);
  EXPECT_EQ(proto.tcp_info().rcv_mss(), 586);
  EXPECT_EQ(proto.tcp_info().last_data_sent(), 239268);
  EXPECT_EQ(proto.tcp_info().last_ack_sent(), 0);
  EXPECT_EQ(proto.tcp_info().last_data_recv(), 239268);
  EXPECT_EQ(proto.tcp_info().last_ack_recv(), 13972);
  EXPECT_EQ(proto.tcp_info().pmtu(), 1500);
  EXPECT_EQ(proto.tcp_info().rcv_ssthresh(), 32216);
  EXPECT_EQ(proto.tcp_info().rtt(), 8500);
  EXPECT_EQ(proto.tcp_info().rttvar(), 11000);
  // Bored now.

  EXPECT_EQ(proto.congestion_algorithm(), "cubic");

  ASSERT_TRUE(proto.has_socket_mem());
  EXPECT_EQ(proto.socket_mem().rcvbuf(), 369280);
  EXPECT_EQ(proto.socket_mem().sndbuf(), 87552);

  ASSERT_TRUE(proto.has_bbr_info());
  EXPECT_EQ(proto.bbr_info().bw(), 5456012050ll);
  EXPECT_EQ(proto.bbr_info().min_rtt(), 306074);
  EXPECT_EQ(proto.bbr_info().pacing_gain(), 126942);
  EXPECT_EQ(proto.bbr_info().cwnd_gain(), 144419);

  // Check proper parsing of endpoints.
  EXPECT_EQ(ToString(proto.inet_diag_msg().sock_id().source()),
            "[2620:0:1003:413:ac8f:7971:3973:b48e]:38625");
  // Don't really care if this changes, but useful to detect
  // when it does.
  EXPECT_EQ(proto.ByteSize(), 205);
  fprintf(stderr, "%s\n", proto.DebugString().c_str());
  fprintf(stderr, "%d\n", proto.ByteSize());
}

TEST(Parser, MoreSamples) {
  TCPInfoParser parser;
  auto* hdr = (const struct nlmsghdr*)raw2.data();
  auto proto = parser.ParseNLMsg(hdr, IPPROTO_TCP);
  ASSERT_TRUE(proto.has_tcp_info());
  ASSERT_TRUE(proto.has_socket_mem());

  for (auto& msg : {raw3, raw4, raw5, raw6, raw7, raw8, raw9}) {
    hdr = (const struct nlmsghdr*)msg.data();
    proto = parser.ParseNLMsg(hdr, IPPROTO_TCP);
    ASSERT_TRUE(proto.has_tcp_info());
    ASSERT_TRUE(proto.has_socket_mem());
  }

  for (auto& msg : {raw10, raw11, raw12, raw13}) {
    hdr = (const struct nlmsghdr*)msg.data();
    proto = parser.ParseNLMsg(hdr, IPPROTO_TCP);
    ASSERT_TRUE(proto.has_tcp_info());
    ASSERT_TRUE(proto.has_socket_mem());
  }

  for (auto& msg : {raw13, raw14, raw15, raw16, raw17, raw18, raw19}) {
    hdr = (const struct nlmsghdr*)msg.data();
    proto = parser.ParseNLMsg(hdr, IPPROTO_TCP);
    ASSERT_TRUE(proto.has_tcp_info());
    ASSERT_TRUE(proto.has_socket_mem());
  }
}

namespace {
int on_close_count = 0;
// On close should only be called with empty new messages.
bool new_msg_not_empty = false;

int on_new_state_count = 0;

void on_close(int protocol, const std::string& old_msg, const std::string& new_msg) {
  new_msg_not_empty |= !new_msg.empty();
  on_close_count++;
}

void on_new_state(int protocol, const std::string& old_msg, const std::string& new_msg) {
  on_new_state_count++;
}

void Print(const struct nlmsghdr* nlh) {
  const google::protobuf::EnumDescriptor* enum_desc =
          mlab::netlink::TCPState_descriptor();
  TCPInfoParser parser;
  auto proto = parser.ParseNLMsg(nlh, IPPROTO_TCP);
  fprintf(stderr, "%5d %s\n",
          proto.inet_diag_msg().sock_id().source().port(),
          enum_desc->value(
              proto.inet_diag_msg().state())
                  ->options().GetExtension(mlab::netlink::name).c_str());
}
}  // anonymous namespace

namespace test {
using namespace std::placeholders;
void VisitAndIncrement(TCPInfoPoller* poller) {
  poller->GetTracker()->VisitMissingRecords(
      std::bind(&TCPInfoPoller::on_close_wrapper, poller, _1, _2, _3));
  poller->GetTracker()->increment_round();
}
}  // namespace test

TEST(Poller, StashAndOnClose) {
  TCPInfoPoller p;
  p.OnClose(on_close, {mlab::netlink::TCPState::ESTABLISHED});
  p.OnNewState(on_new_state);  // always call for new states.

  {
    auto* nlh = (const struct nlmsghdr*)raw2.data();
    auto* msg = reinterpret_cast<struct inet_diag_msg*>(NLMSG_DATA(nlh));
    p.Stash(msg->idiag_family, IPPROTO_TCP, msg->id, nlh);
    Print(nlh);
  }
  {
    auto* nlh = (const struct nlmsghdr*)raw8.data();
    auto* msg = reinterpret_cast<struct inet_diag_msg*>(NLMSG_DATA(nlh));
    p.Stash(msg->idiag_family, IPPROTO_TCP, msg->id, nlh);
    Print(nlh);
  }
  {
    auto* nlh = (const struct nlmsghdr*)raw9.data();
    auto* msg = reinterpret_cast<struct inet_diag_msg*>(NLMSG_DATA(nlh));
    p.Stash(msg->idiag_family, IPPROTO_TCP, msg->id, nlh);
    Print(nlh);
  }
  // All three of these are new states.
  EXPECT_EQ(on_new_state_count, 3);

  // Visit stale records, and increment round;
  test::VisitAndIncrement(&p);
  EXPECT_EQ(on_close_count, 0);

  // Try another round.  Same message should NOT trigger on_new_state.
  {
    auto* nlh = (const struct nlmsghdr*)raw9.data();
    auto* msg = reinterpret_cast<struct inet_diag_msg*>(NLMSG_DATA(nlh));
    p.Stash(msg->idiag_family, IPPROTO_TCP, msg->id, nlh);
    Print(nlh);
  }
  EXPECT_EQ(on_new_state_count, 3);

  // Visit stale records, and increment round;
  // This one should cause the on_close_ to be invoked for two messages.
  test::VisitAndIncrement(&p);
  // We put in one ESTABLISHED, and one OTHER.  We should only see ESTABLISHED.
  EXPECT_EQ(on_close_count, 1);
  EXPECT_FALSE(new_msg_not_empty);  // OnClose should always have empty messages.
}

// TODO
// Test for BBR info parsing
//
}  // namespace netlink
}  // namespace mlab
