#ifdef __cplusplus
extern "C" {
#endif
#include "myclient.h"
#ifdef __cplusplus
}
#endif
#include "common.h"

/*
erpc::Rpc<erpc::CTransport> *rpc;
erpc::MsgBuffer req;
erpc::MsgBuffer resp;

void cont_func(void *, void *) { printf("%s\n", resp.buf); }

void sm_handler(int, erpc::SmEventType, erpc::SmErrType, void *) {}

struct ERPC_blob {
  erpc::Nexus *my_nex;
};
*/

#ifdef __cplusplus
extern "C" {
#endif
void *test_init_client() {
  std::string client_uri = kClientHostname + ":" + std::to_string(kUDPPort);
  erpc::Nexus *n = new erpc::Nexus(client_uri, 0, 0);
  return n;
}
#ifdef __cplusplus
}
#endif

/*

void* init_client() {
  std::string client_uri = kClientHostname + ":" + std::to_string(kUDPPort);
  erpc::Nexus nexus(client_uri, 0, 0);

  struct ERPC_blob* myblob = new ERPC_blob();
  myblob->my_nex = &nexus;
  return ((void *)myblob);
}

void main1() {
  std::string client_uri = kClientHostname + ":" + std::to_string(kUDPPort);
  erpc::Nexus nexus(client_uri, 0, 0);

  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, sm_handler);

  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
  int session_num = rpc->create_session(server_uri, 0);

  while (!rpc->is_connected(session_num)) rpc->run_event_loop_once();

  req = rpc->alloc_msg_buffer_or_die(kMsgSize);
  resp = rpc->alloc_msg_buffer_or_die(kMsgSize);

  rpc->enqueue_request(session_num, kReqType, &req, &resp, cont_func, nullptr);
  rpc->run_event_loop(100);

  delete rpc;
}
*/
