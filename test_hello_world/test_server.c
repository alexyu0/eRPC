#include "stdio.h"
#include "cstdlib"
#include "myserver.h"
#include <unistd.h>

int main () {
  printf("initializing server\n");
  erpc_server_t server = init_server();
  printf("Initialized server\n");


  char* msg = (char*)malloc(10);
  int len;
  printf("Waiting for message\n");
  while ((len = get_message(&msg)), len <= 0) {
    run_event_loop(server, 0);
  }

  printf("Received msg (%s) of len %d\n", msg, len);

  free(msg);
  return 0;
}
