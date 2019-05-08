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
erpc::Rpc<erpc::CTransport> *rpc;

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
    void EnqueueMessage(const erpc::MsgBuffer *req) {
      /* Mesages have the general structure structure:
       * | type (char) | length (int) | data (char*) |
       */
      auto* msg = req->buf;
      auto len = req->get_data_size();
      //printf("ERPC: The message received had len: %d\n", len);
      auto* buffer = new char[len];
      memcpy(buffer, msg, len);
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
    int DequeueMessage(char** buffer_ptr) {
        std::lock_guard<std::mutex> l(latch_);
        if (msg_queue_.empty()) {
          //printf("dequeing empty msg queue\n");
          return 0;
        } else {
          auto& pair = msg_queue_.front();
          msg_queue_.pop_front();
          if (pair.first[0] == 'c') {
              printf("We received an EOF/STOP message. Returning -1\n");
              delete pair.first;
              return -1;
          }
          char *buffer = new char[pair.second];
          memcpy(buffer, pair.first, pair.second);
          *buffer_ptr = buffer;
          //printf("Returning msg: %s\n", buffer);
          delete pair.first;
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
  //printf("size %d\n", req->get_data_size());
  //printf("ERPC: We received a message! In req_handler\n");

  // Add message to queue
  assert(cntxt == context);
  context->EnqueueMessage(req);

  // send response
  auto &resp = req_handle->pre_resp_msgbuf;
  rpc->resize_msg_buffer(&resp, kMsgSize);
  sprintf(reinterpret_cast<char *>(resp.buf), "enqueue");
  rpc->enqueue_response(req_handle, &resp);

  return;
}


#ifdef __cplusplus
extern "C" {
#endif

int get_message(char** buf) {
  assert(context != nullptr);
  return context->DequeueMessage(buf);
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
erpc_server_t init_server(int instance_no) {
  //printf("Entering init_server\n");
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort + instance_no);
  //printf("Created server uri\n");
  nexus = new erpc::Nexus(server_uri, 0, 0);
  //printf("Initialized nexus\n");
  nexus->register_req_func(kReqType, req_handler);
  //printf("Registered function\n");

  // Initialize Context
  context = new eRPCContext();
  //printf("initialized context\n");

  rpc = new erpc::Rpc<erpc::CTransport>(nexus, (void*)context, instance_no, nullptr);
  //printf("Initialized server\n");
  return (void *)rpc;
}

#ifdef __cplusplus
}
#endif
/*
void test_erpc_context() {
  auto* ctxt = new eRPCContext();

  uint8_t* msg = (uint8_t*)"test";
  ctxt->EnqueueMessage(msg);
  char* buf = new char[10];
  int size = ctxt->DequeueMessage(&buf);
  printf("String: %s | Size: %d\n",buf, size);
  assert(size == 4);

  delete buf;
  delete ctxt;
}
*/
#ifdef __cplusplus
extern "C" {
#endif

void test_server(erpc_server_t s) {
  erpc::Rpc<erpc::CTransport>* server = (erpc::Rpc<erpc::CTransport>*)s;
  assert(server != nullptr);
  //test_erpc_context();
  printf("not doing anyting\n");
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


