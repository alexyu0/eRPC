#ifndef ERPC_SSLOT_H
#define ERPC_SSLOT_H

#include "msg_buffer.h"
#include "ops.h"

namespace ERpc {

class Session; /* Forward declaration */

/**
 * @brief Session slot metadata maintained about an Rpc
 *
 * This slot structure is used by both server and client sessions.
 *
 * The validity/existence of a request or response in a slot is inferred from
 * \p rx_msgbuf or \p tx_msgbuf. Doing so avoids maintaining additional
 * boolean fields (such as \em is_req_received and \em is_resp_generated).
 *
 * If either \p rx_msgbuf or \p tx_msgbuf is valid outside a function,
 * its packet header must contain the request type and number.
 */
class SSlot {
 public:
  // Members that are valid for both server and client
  uint8_t pad[64];       ///< Padding to prevent false sharing
  Session *session;      ///< Pointer to the session that this sslot belongs to
  size_t index;          ///< Index of this sslot in the session's sslot_arr
  MsgBuffer rx_msgbuf;   ///< The RX MsgBuffer, valid if \p buf is not NULL
  MsgBuffer *tx_msgbuf;  ///< The TX MsgBuffer, valid if it is not NULL

  // Client-only
  erpc_cont_func_t cont_func;  ///< Continuation function for the request
  size_t tag;                  ///< Tag of the request

  // Server-only
  ReqFuncType req_func_type;  ///< The type of the request function
  MsgBuffer pre_resp_msgbuf;  ///< Prealloc MsgBuffer to store app response
  MsgBuffer dyn_resp_msgbuf;  ///< Dynamic MsgBuffer to store app response
  bool prealloc_used;         ///< Did the app use \p pre_resp_msgbuf

  /// Return a string representation of this session slot
  std::string to_string() const {
    if (rx_msgbuf.buf == nullptr && tx_msgbuf == nullptr) {
      return "[Invalid]";
    }

    /*
     * Sanity check: If the RX and TX MsgBuffers are both valid, they should
     * contain identical request number and type.
     */
    if (rx_msgbuf.buf != nullptr && tx_msgbuf != nullptr) {
      assert(rx_msgbuf.get_req_num() == tx_msgbuf->get_req_num());
      assert(rx_msgbuf.get_req_type() == tx_msgbuf->get_req_type());
    }

    /* Extract the request number and type from either RX or TX MsgBuffer */
    std::string req_num_string, req_type_string;
    if (rx_msgbuf.buf != nullptr) {
      req_num_string = std::to_string(rx_msgbuf.get_req_num());
      req_type_string = std::to_string(rx_msgbuf.get_req_type());
    }

    if (tx_msgbuf != nullptr && rx_msgbuf.buf == nullptr) {
      req_num_string = std::to_string(tx_msgbuf->get_req_num());
      req_type_string = std::to_string(tx_msgbuf->get_req_type());
    }

    std::ostringstream ret;
    ret << "[req num" << req_num_string << ", "
        << "req type " << req_type_string << ", "
        << "rx_msgbuf " << rx_msgbuf.to_string() << ", "
        << "tx_msgbuf "
        << (tx_msgbuf == nullptr ? "0x0" : tx_msgbuf->to_string()) << "]";
    return ret.str();
  }
};
}

#endif  // ERPC_SSLOT_H
