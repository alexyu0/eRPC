/* myserver.h */
#ifdef __cplusplus
extern "C" {
#endif

#define EVENT_LOOP_DURATION 1000

typedef void* erpc_server_t;

erpc_server_t init_server();

int get_message(char* buf);

/**
 * Runs the event loop by calling server->run_event_loop(duration)
 * If duration == 0, instead calls run_event_loop_once()
 */
void run_event_loop(erpc_server_t s, size_t duration);

/******************
 * Test Functions *
 ******************/

void test_server(erpc_server_t s);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif


