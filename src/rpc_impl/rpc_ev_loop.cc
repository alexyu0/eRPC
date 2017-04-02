#include "rpc.h"

namespace ERpc {

template <class TTr>
inline void Rpc<TTr>::run_event_loop_one_st() {
  dpath_stat_inc(&dpath_stats.ev_loop_calls);

  if (kDatapathChecks) {
    if (!in_creator()) {
      erpc_dprintf(
          "eRPC Rpc %u: Error. Event loop invoked from a background thread.\n",
          app_tid);
      exit(-1);
    }

    if (in_event_loop) {
      erpc_dprintf("eRPC Rpc %u: Error. Re-entering event loop not allowed.\n",
                   app_tid);
      exit(-1);
    }

    in_event_loop = true;
  } else {
    assert(in_creator());
    // Event loop re-entrance tracking is disabled
  }

  // Handle session management events, if any
  if (unlikely(nexus_hook.sm_pkt_list.size > 0)) {
    handle_session_management_st();  // Callee grabs the hook lock
  }

  // Check if we need to retransmit any session management requests
  if (unlikely(mgmt_retry_queue.size() > 0)) {
    mgmt_retry();
  }

  process_comps_st();      // RX
  process_dpath_txq_st();  // TX

  if (kDatapathChecks) {
    in_event_loop = false;
  }
}

template <class TTr>
inline void Rpc<TTr>::run_event_loop_st() {
  assert(in_creator());
  while (true) {
    run_event_loop_one();
  }
}

template <class TTr>
inline void Rpc<TTr>::run_event_loop_timeout_st(size_t timeout_ms) {
  assert(in_creator());

  uint64_t start_tsc = rdtsc();
  while (true) {
    run_event_loop_one();

    double elapsed_ms = to_sec(rdtsc() - start_tsc, nexus->freq_ghz) * 1000;
    if (elapsed_ms > timeout_ms) {
      return;
    }
  }
}

}  // End ERpc