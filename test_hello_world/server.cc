#include "common.h"
#include <mutex>

#ifdef __cplusplus
extern "C" {
#endif
#include "myserver.h"
#ifdef __cplusplus
}
#endif

// From TPCC experiment, max message size was around 130k bytes. Setting this
// to 200k to be safe. Can be fine tuned if needed.
#define MAX_MESSAGE_SIZE 200000

class eRPCContext;
eRPCContext *context;
erpc::Nexus *nexus;


class eRPCContext {

  public:
    eRPCContext() = default;

    ~eRPCContext() {
      for (auto& p: msg_queue_) {
        delete p.first;
      }
    }

    /**
     * Adds an message to the message queue
     */
    void EnqueueMessage(uint8_t* msg) {
      /* Mesages have the general structure structure:
       * | type (char) | length (int) | data (char*) |
       */
      auto* buffer = new char[MAX_MESSAGE_SIZE];
      auto len = sprintf(buffer, "%s", msg);

      {
        std::lock_guard<std::mutex> l(latch_);
        msg_queue_.emplace_back(buffer, len);
      }
    }




    /**
     * Pops the first message from the message queue, and copies it to buffer.
     * Returns 0 if message queue is empty, else returns length of message
     * returned.
     */
    int DequeueMessage(char* buffer) {
        std::lock_guard<std::mutex> l(latch_);
        if (msg_queue_.empty()) {
          return 0;
        } else {
          auto& pair = msg_queue_.front();
          msg_queue_.pop_front();
          auto len = sprintf(buffer, "%s", pair.first);
          assert(len == pair.second);
          return pair.second;
        }
    }

  private:
    std::deque<std::pair<char* /* message */, int /* length */>> msg_queue_;
    std::mutex latch_;
};


void req_handler(erpc::ReqHandle *req_handle, void *cntxt) {

  // Get message (requests from the client/master)
  const erpc::MsgBuffer *req = req_handle->get_req_msgbuf();

  // Add message to queue
  assert(cntxt == context);
  context->EnqueueMessage(req->buf);

  return;
}


#ifdef __cplusplus
extern "C" {
#endif

int get_message(char* buf) {
  assert(context != nullptr);
  return context->DequeueMessage(buf);
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
erpc_server_t init_server() {
  printf("Entering init_server\n");
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);
  printf("Created server uri\n");
  nexus = new erpc::Nexus(server_uri, 0, 0);
  printf("Initialized nexus\n");
  nexus->register_req_func(kReqType, req_handler);
  printf("Registered function\n");

  // Initialize Context
  context = new eRPCContext();
  printf("initialized context\n");

  auto *rpc = new erpc::Rpc<erpc::CTransport>(nexus, (void*)context, 0, nullptr);
  printf("Initialized server\n");
  return (void *)rpc;
}

#ifdef __cplusplus
}
#endif

void test_erpc_context() {
  auto* ctxt = new eRPCContext();

  uint8_t* msg = (uint8_t*)"test";
  ctxt->EnqueueMessage(msg);
  char* buf = new char[10];
  int size = ctxt->DequeueMessage(buf);
  printf("String: %s | Size: %d\n",buf, size);
  assert(size == 4);

  delete buf;
  delete ctxt;
}

#ifdef __cplusplus
extern "C" {
#endif

void test_server(erpc_server_t s) {
  erpc::Rpc<erpc::CTransport>* server = (erpc::Rpc<erpc::CTransport>*)s;
  assert(server != nullptr);
  test_erpc_context();
  printf("Succesfully casted server\n");
  return;
}

#ifdef __cplusplus
} // closing extern "C" bracket
#endif


#ifdef __cplusplus
extern "C" {
#endif

void run_event_loop(erpc_server_t s, size_t duration) {
  assert(s != nullptr);
  erpc::Rpc<erpc::CTransport>* server = (erpc::Rpc<erpc::CTransport>*)s;
  assert(server != nullptr);
  if (duration == 0) {
    server->run_event_loop_once();
  } else {
    server->run_event_loop(duration);
  }
}

#ifdef __cplusplus
} // closing extern "C" bracket
#endif


#ifdef __cplusplus
extern "C" {
#endif

void delete_server(erpc_server_t s) {
  assert(s != nullptr);
  erpc::Rpc<erpc::CTransport>* server = (erpc::Rpc<erpc::CTransport>*)s;
  assert(server != nullptr);
  delete server;
  delete nexus;
  delete context;
}

#ifdef __cplusplus
} // closing extern "C" bracket
#endif


