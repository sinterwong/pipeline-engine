#include "serialModule.h"
// #include "frameMessage.pb.h"

namespace module {
serialModule::serialModule(Backend *ptr, const std::string &initName,
                           const std::string &initType,
                           const std::vector<std::string> &recv,
                           const std::vector<std::string> &send)
    : Module(ptr, initName, initType, recv, send) {}

void serialModule::forward(std::vector<forwardMessage> message) {
  // tutorial::FrameMessage buf;
  //    char ch = getchar();
  //    tutorial::FrameMessage buf;
  //    bool flag = false;
  //    if (ch == 's' || ch == 'S')
  //    {
  //        buf.set_control("stop");
  //        flag = true;
  //    } else if (ch == 'i' || ch == 'I')
  //    {
  //        buf.set_control("init");
  //        flag = true;
  //    }
  //    autoSend(buf.SerializeAsString());
}
FlowEngineModuleRegister(serialModule, Backend *, std::string const &,
                         std::string const &, std::vector<std::string> const &,
                         std::vector<std::string> const &);
} // namespace module