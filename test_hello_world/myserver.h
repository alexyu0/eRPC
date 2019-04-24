/* myserver.h */
#ifdef __cplusplus
extern "C" {
#endif

#define EVENT_LOOP_DURATION 1000

typedef void* erpc_server_t;

erpc_server_t init_server();

void test_server(erpc_server_t s);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif


