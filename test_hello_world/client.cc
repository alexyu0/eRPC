#include "common.h"
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "myclient.h"
#ifdef __cplusplus
}
#endif


erpc::Rpc<erpc::CTransport> *rpc;
erpc::MsgBuffer req;
erpc::MsgBuffer resp;

void cont_func(void *, void *) { printf("%s\n", resp.buf); }

void sm_handler(int, erpc::SmEventType, erpc::SmErrType, void *) {}

struct ERPC_blob {
  erpc::Nexus *my_nex;
  erpc::Rpc<erpc::CTransport> *my_rpc;
  int session_num;
};



#ifdef __cplusplus
extern "C" {
#endif
erpc_client_t init_client() {
  struct ERPC_blob* myblob = new ERPC_blob();

  std::string client_uri = kClientHostname + ":" + std::to_string(kUDPPort);
  erpc::Nexus *n = new erpc::Nexus(client_uri, 0, 0);
  erpc::Rpc<erpc::CTransport> *rpc = new erpc::Rpc<erpc::CTransport>(n, nullptr, 0, sm_handler);
  printf("ERPC: Connecting to server\n");
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
  int session_num = rpc->create_session(server_uri, 0);
  while (!rpc->is_connected(session_num)) {
    //session_num = rpc->create_session(server_uri, 0);
    rpc->run_event_loop_once();
  }
  myblob->session_num = session_num;
  printf("ERPC: Connected to eRPC server\n");

  myblob->my_nex = n;
  myblob->my_rpc = rpc;
  printf("ERPC: Initialized client\n");
  return ((void *)myblob);
}
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
void set_message(erpc_client_t myblob, const char *s, size_t len) {
  if (myblob == nullptr) {
    printf("erpc blob is null!");
    return;
  }
  printf("ERPC: Sending message of len %zd\n", len);

  erpc::Rpc<erpc::CTransport> *rpc = ((ERPC_blob*)myblob)->my_rpc;
  int session_num = ((ERPC_blob*)myblob)->session_num;

  /*
  printf("ERPC: Connecting to server\n");
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
  int session_num = rpc->create_session(server_uri, 0);
  while (!rpc->is_connected(session_num)) {
    rpc->run_event_loop_once();
    //printf("waiting for connection\n");
  }
  printf("ERPC: Connected to eRPC server\n");
  */

  req = rpc->alloc_msg_buffer_or_die(len);
  resp = rpc->alloc_msg_buffer_or_die(kMsgSize);

  //req.buf = (unsigned char *)s;
  memcpy(req.buf, s, len);
  rpc->enqueue_request(session_num, kReqType, &req, &resp, cont_func, nullptr);
  rpc->run_event_loop(100);
  printf("ERPC: Message sent in client!\n");
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
void delete_client (erpc_client_t myblob) {
  if (myblob != nullptr) {
    ERPC_blob* b = (ERPC_blob*)myblob;
    delete (b->my_rpc);
    delete (b->my_nex);
    delete b;
  }

}

#ifdef __cplusplus
}
#endif
