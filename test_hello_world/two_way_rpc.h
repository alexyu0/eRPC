/* two_way_rpc.h */
#ifdef __cplusplus
extern "C" {
#endif

typedef void* erpc_obj_t;

erpc_obj_t init_erpc_obj();

void set_message(erpc_client_t myblob, const char *s, size_t len);


/**
 * Get message received by server by copying it to buf and returning the
 * message size. If no message is received, then return 0
 */
int get_message(char** buf);

/**
 * Runs the event loop by calling server->run_event_loop(duration)
 * If duration == 0, instead calls run_event_loop_once()
 */
void run_event_loop(erpc_server_t s, size_t duration);


/**
 * Clean up all server objects
 */
void delete_server(erpc_server_t s);

/******************
 * Test Functions *
 ******************/

/**
 * Run server test suite
 */
void test_server(erpc_server_t s);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif


