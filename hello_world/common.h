#include <stdio.h>
#include "rpc.h"

static const std::string kServerHostname = "128.110.153.140";
static const std::string kClientHostname = "128.110.153.148";

static constexpr uint16_t kUDPPort = 31850;
static constexpr uint8_t kReqType = 2;
static constexpr size_t kMsgSize = 16;
