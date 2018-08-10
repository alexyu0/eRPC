#include "client_tests.h"

void req_handler(ReqHandle *, void *);  // Forward declaration

/// Request handler for foreground testing
auto reg_info_vec_fg = {
    ReqFuncRegInfo(kTestReqType, req_handler, ReqFuncType::kForeground)};

/// Request handler for background testing
auto reg_info_vec_bg = {
    ReqFuncRegInfo(kTestReqType, req_handler, ReqFuncType::kBackground)};

/// Per-thread application context
class AppContext : public BasicAppContext {
 public:
  FastRand fastrand;  ///< Used for picking large message sizes
};

/// Configuration for controlling the test
size_t config_num_iters;         ///< The number of iterations
size_t config_num_sessions;      ///< Number of sessions created by client
size_t config_rpcs_per_session;  ///< Number of Rpcs per session per iteration
size_t config_num_bg_threads;    ///< Number of background threads

/// The common request handler for all subtests. Copies request to response
void req_handler(ReqHandle *req_handle, void *_context) {
  auto *context = static_cast<AppContext *>(_context);
  assert(!context->is_client);
  if (config_num_bg_threads > 0) assert(context->rpc->in_background());

  const MsgBuffer *req_msgbuf = req_handle->get_req_msgbuf();
  size_t resp_size = req_msgbuf->get_data_size();

  // MsgBuffer allocation is thread safe
  req_handle->dyn_resp_msgbuf =
      context->rpc->alloc_msg_buffer_or_die(resp_size);
  memcpy(req_handle->dyn_resp_msgbuf.buf, req_msgbuf->buf, resp_size);
  req_handle->prealloc_used = false;

  size_t user_alloc_tot = context->rpc->get_stat_user_alloc_tot();
  test_printf(
      "Server: Received request of length %zu. "
      "Rpc memory used = %zu bytes (%.3f MB)\n",
      resp_size, user_alloc_tot, 1.0 * user_alloc_tot / MB(1));

  context->rpc->enqueue_response(req_handle);
}

/// The common continuation function for all subtests. This checks that the
/// request buffer is identical to the response buffer, and increments the
/// number of responses in the context.
void cont_func(RespHandle *resp_handle, void *_context, size_t tag) {
  _unused(tag);

  const MsgBuffer *resp_msgbuf = resp_handle->get_resp_msgbuf();
  test_printf("Client: Received response of length %zu.\n",
              resp_msgbuf->get_data_size());

  for (size_t i = 0; i < resp_msgbuf->get_data_size(); i++) {
    assert(resp_msgbuf->buf[i] == static_cast<uint8_t>(tag));
  }

  auto *context = static_cast<AppContext *>(_context);
  assert(context->is_client);
  context->num_rpc_resps++;

  context->rpc->release_response(resp_handle);
}

/// The generic test function that issues \p config_rpcs_per_session Rpcs
/// on each of \p config_num_sessions sessions, for multiple iterations.
///
/// The second \p size_t argument exists only because the client thread function
/// template in client_tests.h requires it.
void generic_test_func(Nexus *nexus, size_t) {
  // Create the Rpc and connect the session
  AppContext context;
  client_connect_sessions(nexus, context, config_num_sessions,
                          basic_sm_handler);

  Rpc<CTransport> *rpc = context.rpc;
  int *session_num_arr = context.session_num_arr;

  // Pre-create MsgBuffers so we can test reuse and resizing
  size_t tot_reqs_per_iter = config_num_sessions * config_rpcs_per_session;
  std::vector<MsgBuffer> req_msgbufs(tot_reqs_per_iter);
  std::vector<MsgBuffer> resp_msgbufs(tot_reqs_per_iter);
  for (size_t req_i = 0; req_i < tot_reqs_per_iter; req_i++) {
    req_msgbufs[req_i] = rpc->alloc_msg_buffer_or_die(rpc->get_max_msg_size());
    resp_msgbufs[req_i] = rpc->alloc_msg_buffer_or_die(rpc->get_max_msg_size());
  }

  // The main request-issuing loop
  for (size_t iter = 0; iter < config_num_iters; iter++) {
    context.num_rpc_resps = 0;

    test_printf("Client: Iteration %zu.\n", iter);
    size_t iter_req_i = 0;  // Request MsgBuffer index in this iteration

    for (size_t sess_i = 0; sess_i < config_num_sessions; sess_i++) {
      for (size_t w_i = 0; w_i < config_rpcs_per_session; w_i++) {
        assert(iter_req_i < tot_reqs_per_iter);
        MsgBuffer &cur_req_msgbuf = req_msgbufs[iter_req_i];

        size_t req_size =
            get_rand_msg_size(&context.fastrand, rpc->get_max_data_per_pkt(),
                              rpc->get_max_msg_size());

        rpc->resize_msg_buffer(&cur_req_msgbuf, req_size);
        for (size_t i = 0; i < req_size; i++) {
          cur_req_msgbuf.buf[i] = static_cast<uint8_t>(iter_req_i);
        }

        rpc->enqueue_request(session_num_arr[sess_i], kTestReqType,
                             &cur_req_msgbuf, &resp_msgbufs[iter_req_i],
                             cont_func, iter_req_i);

        iter_req_i++;
      }
    }

    wait_for_rpc_resps_or_timeout(context, tot_reqs_per_iter, nexus->freq_ghz);
    assert(context.num_rpc_resps == tot_reqs_per_iter);
  }

  // Free the MsgBuffers
  for (auto req_msgbuf : req_msgbufs) rpc->free_msg_buffer(req_msgbuf);
  for (auto resp_msgbuf : resp_msgbufs) rpc->free_msg_buffer(resp_msgbuf);

  // Disconnect the sessions
  for (size_t sess_i = 0; sess_i < config_num_sessions; sess_i++) {
    rpc->destroy_session(session_num_arr[sess_i]);
  }

  rpc->run_event_loop(kTestEventLoopMs);

  // Free resources
  delete rpc;
  client_done = true;
}

void launch_helper() {
  auto &reg_info_vec =
      config_num_bg_threads == 0 ? reg_info_vec_fg : reg_info_vec_bg;
  launch_server_client_threads(config_num_sessions, config_num_bg_threads,
                               generic_test_func, reg_info_vec,
                               ConnectServers::kFalse, 0.0);
}

TEST(OneLargeRpc, Foreground) {
  config_num_iters = 1;
  config_num_sessions = 1;
  config_rpcs_per_session = 1;
  config_num_bg_threads = 0;
  launch_helper();
}

TEST(OneLargeRpc, Background) {
  config_num_iters = 1;
  config_num_sessions = 1;
  config_rpcs_per_session = 1;
  config_num_bg_threads = 1;
  launch_helper();
}

TEST(MultiLargeRpcOneSession, Foreground) {
  config_num_iters = 1;
  config_num_sessions = 1;
  config_rpcs_per_session = kSessionReqWindow;
  config_num_bg_threads = 0;
  launch_helper();
}

TEST(MultiLargeRpcOneSession, Background) {
  config_num_iters = 1;
  config_num_sessions = 1;
  config_rpcs_per_session = kSessionReqWindow;
  config_num_bg_threads = 1;
  launch_helper();
}

TEST(MultiLargeRpcMultiSession, Foreground) {
  assert(erpc::is_log_level_reasonable());
  config_num_iters = 2;
  config_num_sessions = 4;
  config_rpcs_per_session = kSessionReqWindow;
  config_num_bg_threads = 0;
  launch_helper();
}

TEST(MultiLargeRpcMultiSession, Background) {
  assert(erpc::is_log_level_reasonable());
  config_num_iters = 2;
  config_num_sessions = 4;
  config_rpcs_per_session = kSessionReqWindow;
  config_num_bg_threads = 1;
  launch_helper();
}

TEST(DISABLED_MemoryLeak, Foreground) {
  assert(erpc::is_log_level_reasonable());
  config_num_iters = 50;
  config_num_sessions = 4;
  config_rpcs_per_session = kSessionReqWindow;
  config_num_bg_threads = 0;
  launch_helper();
}

TEST(DISABLED_MemoryLeak, Background) {
  assert(erpc::is_log_level_reasonable());
  config_num_iters = 50;
  config_num_sessions = 4;
  config_rpcs_per_session = kSessionReqWindow;
  config_num_bg_threads = 1;
  launch_helper();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
