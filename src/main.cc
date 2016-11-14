/**********************************************************************
 * This captures 10 minutes of connection data.
 * Compressed with gzip, takes about 23 bytes / record.
 * Compressed with bzip2, takes about 17 bytes / record.  (15.5:1)
 **********************************************************************/

#include <chrono>
#include <fstream>
#include <unistd.h>

#include "tcpinfo_lib.h"  // Poller

namespace {
void output(const std::string& nlmsg, int protocol, std::string tag) {
  std::ofstream out;
  out.open("nldata", std::ofstream::app | std::ofstream::binary);
  int len = nlmsg.size();
  for (int i = 0; i < len; ++i) {
    out.put(nlmsg.at(i));
  }
  out.close();

  // TODO(gfr) This should be optional.
  auto* hdr = (const struct nlmsghdr*)nlmsg.c_str();
  mlab::netlink::TCPInfoParser parser;
  auto proto = parser.ParseNLMsg(hdr, mlab::netlink::Protocol(protocol));
  printf("%s\n", proto.ShortDebugString().c_str());
  const google::protobuf::EnumDescriptor* enum_desc =
          mlab::netlink::TCPState_descriptor();

  fprintf(stderr, "%5d %s %s\n",
          proto.inet_diag_msg().sock_id().source().port(),
          tag.c_str(),
          enum_desc->value(
              proto.inet_diag_msg().state())
                  ->options().GetExtension(mlab::netlink::name).c_str());
}

void output_hash(int protocol, const std::string& old_msg, const std::string& new_msg) {
  output(old_msg, protocol, "#");
}

void on_new_state(int protocol, const std::string& old_msg,
                  const std::string& new_msg) {
  // Always output old data when we see new state!
  if (!old_msg.empty()) {
    auto old_state = mlab::netlink::GetStateFromStr(old_msg);

    // Output old state if it is ESTABLISHED.
    if (old_state == mlab::netlink::TCPState::ESTABLISHED) {
      output(old_msg, protocol, "*");
    }
  }

  // For all states EXCEPT ESTABLISHED, output the state immediately.
  if (mlab::netlink::GetStateFromStr(new_msg)
      != mlab::netlink::TCPState::ESTABLISHED) {
    output(new_msg, protocol, " ");
  }
}
} // anonymous namespace

extern mlab::netlink::TCPInfoPoller g_poller_;

int main(int argc, char* argv[]) {
  using std::chrono::steady_clock;
  using std::chrono::duration;

  auto start = steady_clock::now();
  auto one_minute = duration<int>(60);

  // NOTE: This is rarely firing, because we almost always see some other
  // state as the connection is closing.
  g_poller_.OnClose(output_hash, {mlab::netlink::TCPState::ESTABLISHED});
  // This is handling all the output.
  g_poller_.OnNewState(on_new_state);

  int count = 0;
  while (steady_clock::now() < start + 10*one_minute) {
    g_poller_.PollOnce();
    count++;
  }
  fprintf(stderr, "Rate was %d polls/sec\n", count / 600);
  return 0;
}
