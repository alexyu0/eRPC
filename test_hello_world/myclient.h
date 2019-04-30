/* myErpcClient.h */
#ifdef __cplusplus
extern "C" {
#endif

void* init_client();
void set_message (void* myblob);
void delete_client(void* myblob);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif


