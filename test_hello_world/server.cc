#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "myserver.h"
#ifdef __cplusplus
}
#endif

void req_handler(erpc::ReqHandle *req_handle, void *) {
  return;
  /*
  auto &resp = req_handle->pre_resp_msgbuf;
  rpc->resize_msg_buffer(&resp, kMsgSize);
  sprintf(reinterpret_cast<char *>(resp.buf), "hello");

  rpc->enqueue_response(req_handle, &resp);
  */
}

#ifdef __cplusplus
extern "C" {
#endif
erpc_server_t init_server() {
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
  erpc::Nexus nexus(server_uri, 0, 0);
  printf("Inidiatlized nexus\n");
  nexus.register_req_func(kReqType, req_handler);
  auto *rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, nullptr);
  printf("Initialized server\n");
  return (void *)rpc;
}
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

void test_server(erpc_server_t s) {
  erpc::Rpc<erpc::CTransport>* server = (erpc::Rpc<erpc::CTransport>*)s;
  assert(server != nullptr);
  printf("Succesfully casted server\n");
  return;
}

#ifdef __cplusplus
} // closing extern "C" bracket
#endif

