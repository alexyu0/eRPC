#include "stdio.h"
#include "cstdlib"
#include "myserver.h"
#include <unistd.h>
#include "cstring"
#include "cassert"

int main () {
  printf("initializing server\n");
  erpc_server_t server = init_server();
  printf("Initialized server\n");


  char* msg = (char*)malloc(10);
  int len = 0;
  printf("Waiting for message\n");
  while (true) {
    run_event_loop(server, 0);
    len = get_message(&msg);

    if (len != 0) break;
  }

  printf("Received msg (%s) of len %d\n", msg, len);

  /*
  memset(msg, 0, 10);
  printf("Waiting for message\n");
  run_event_loop(server, 1000);
  len = get_message(&msg);
  assert(len == -1);
  printf("Received 'c' of len %d\n", len);
*/
  free(msg);
  return 0;
}
