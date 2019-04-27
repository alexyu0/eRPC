#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "myserver.h"
#ifdef __cplusplus
}
#endif

void req_handler(erpc::ReqHandle *req_handle, void *) {
  /*
  auto &resp = req_handle->pre_resp_msgbuf;
  rpc->resize_msg_buffer(&resp, kMsgSize);
  sprintf(reinterpret_cast<char *>(resp.buf), "hello");

  rpc->enqueue_response(req_handle, &resp);
  */
  return;
}


#ifdef __cplusplus
extern "C" {
#endif
erpc_server_t init_server() {
  printf("Entering init_server\n");
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
  printf("Created server uri\n");
  erpc::Nexus nexus(server_uri, 0, 0);
  printf("Initialized nexus\n");
  nexus.register_req_func(kReqType, req_handler);
  printf("Registered function\n");
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
  return;
  erpc::Rpc<erpc::CTransport>* server = (erpc::Rpc<erpc::CTransport>*)s;
  assert(server != nullptr);
  printf("Succesfully casted server\n");
  return;
}

#ifdef __cplusplus
} // closing extern "C" bracket
#endif

