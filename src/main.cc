/**********************************************************************
 * Demo polling implementation.
 *
 * This captures 10 minutes of connection data.
 * Raw nlmsg data compressed with bzip2 takes about 15 bytes/record.
 * Wire format protos compressed with bzip2 also takes about 15 bytes/record.
 * bzip2 compressed proto.ShortDebugString takes about 16 bytes/record.
 **********************************************************************/

#include <chrono>
#include <fstream>
#include <unistd.h>

#include "tcpinfo_lib.h"  // Poller

namespace {
// Dump just raw nlmsg, without protocol info.
void DumpNlMsg(const std::string& nlmsg) {
  std::ofstream out;
  out.open("nldata", std::ofstream::app | std::ofstream::binary);
  int len = nlmsg.size();
  for (int i = 0; i < len; ++i) {
    out.put(nlmsg.at(i));
  }
  out.close();
}

void DumpProto(const mlab::netlink::TCPDiagnosticsProto& proto) {
  std::ofstream out;
  out.open("protodata", std::ofstream::app | std::ofstream::binary);
  proto.SerializeToOstream(&out);
  out.close();

  // TODO(gfr) This should be optional.
  printf("%s\n", proto.ShortDebugString().c_str());
}

void DumpSummary(const mlab::netlink::TCPDiagnosticsProto& proto, std::string tag) {
  const google::protobuf::EnumDescriptor* enum_desc =
          mlab::netlink::TCPState_descriptor();
  fprintf(stderr, "%5d %s %s\n",
          proto.inet_diag_msg().sock_id().source().port(),
          tag.c_str(),
          enum_desc->value(
              proto.inet_diag_msg().state())
                  ->options().GetExtension(mlab::netlink::name).c_str());
}

void Output(const std::string& nlmsg, int protocol, std::string tag) {
  DumpNlMsg(nlmsg);

  auto proto = mlab::netlink::TCPInfoParser().ParseNLMsg(
      nlmsg, mlab::netlink::Protocol(protocol));

  DumpProto(proto);
  DumpSummary(proto, tag);
}

// Output, using '#' as tag for summary.
void on_close(int protocol, const std::string& old_msg,
              const std::string& new_msg) {
  Output(old_msg, protocol, "#");
}

// Output each new state we see, except for ESTABLISHED, which should only
// be output when it is the old state.
// PREREQ: old_msg and new_msg should have different TCPState values.
void on_new_state(int protocol, const std::string& old_msg,
                  const std::string& new_msg) {
  // Output old data when it's state is ESTABLISHED.
  if (!old_msg.empty()) {
    auto old_state = mlab::netlink::GetStateFromStr(old_msg);

    if (old_state == mlab::netlink::TCPState::ESTABLISHED) {
      Output(old_msg, protocol, "*");
    }
  }

  // For all states EXCEPT ESTABLISHED, output the state immediately.
  if (mlab::netlink::GetStateFromStr(new_msg)
      != mlab::netlink::TCPState::ESTABLISHED) {
    Output(new_msg, protocol, " ");
  }
}
} // anonymous namespace

extern mlab::netlink::TCPInfoPoller g_poller_;

int main(int argc, char* argv[]) {
  using std::chrono::steady_clock;
  using std::chrono::duration;

  auto start = steady_clock::now();
  auto one_minute = duration<int>(60);

  // NOTE: This fires only occasionally, on very short lived connections.
  // Usually, we see some other OnNewState before the connection closes.
  g_poller_.OnClose(on_close, {mlab::netlink::TCPState::ESTABLISHED});
  g_poller_.OnNewState(on_new_state);

  int count = 0;
  while (steady_clock::now() < start + 10*one_minute) {
    g_poller_.PollOnce();
    count++;
  }
  fprintf(stderr, "Rate was %d polls/sec\n", count / 600);
  return 0;
}
