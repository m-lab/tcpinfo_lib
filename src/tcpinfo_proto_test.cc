// Placeholder for more interesting tests.

#include "tcpinfo.pb.h"
#include "gtest/gtest.h"

namespace mlab {
namespace netlink {

TEST(tcpinfo_proto, has_some_fields) {
  TCPDiagnosticsProto proto;

  proto.set_diag_protocol("foobar");
  EXPECT_EQ(proto.diag_protocol(), "foobar");
}

}
}
