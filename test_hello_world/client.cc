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
};



#ifdef __cplusplus
extern "C" {
#endif
erpc_client_t init_client() {
  printf("in init client\n");
  //sleep(20);
  std::string client_uri = kClientHostname + ":" + std::to_string(kUDPPort);
  printf("hi\n");
  erpc::Nexus *n = new erpc::Nexus(client_uri, 0, 0);
  printf("hi\n");
  erpc::Rpc<erpc::CTransport> *rpc = new erpc::Rpc<erpc::CTransport>(n, nullptr, 0, sm_handler);


  printf("hi\n");
  struct ERPC_blob* myblob = new ERPC_blob();
  printf("hi\n");
  myblob->my_nex = n;
  printf("hi\n");
  myblob->my_rpc = rpc;
  printf("hi\n");
  return ((void *)myblob);
}
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
void set_message (erpc_client_t myblob, const char *s, size_t len) {
  if (myblob == nullptr) {
    printf("erpc blob is null!");
    return;
  }

  erpc::Rpc<erpc::CTransport> *rpc = ((ERPC_blob*)myblob)->my_rpc;
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
  int session_num = rpc->create_session(server_uri, 0);

  while (!rpc->is_connected(session_num)) rpc->run_event_loop_once();

  req = rpc->alloc_msg_buffer_or_die(len);
  resp = rpc->alloc_msg_buffer_or_die(kMsgSize);

  req.buf = (unsigned char *)s;
  rpc->enqueue_request(session_num, kReqType, &req, &resp, cont_func, nullptr);

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
