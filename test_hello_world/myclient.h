/* myErpcClient.h */
#ifdef __cplusplus
extern "C" {
#endif

typedef void* erpc_client_t;

erpc_client_t init_client(int instance_no);

void set_message(erpc_client_t myblob, const char *s, size_t len);

void delete_client(erpc_client_t myblob);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif


