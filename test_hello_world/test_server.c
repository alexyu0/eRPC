#include "stdio.h"
#include "myserver.h"

int main () {
  printf("hello\n");
  erpc_server_t server = init_server();
  printf("world\n");

  printf("Running tests\n");
  test_server(server);
  printf("All tests passed!\n");

  run_event_loop(server, 0);
  printf("Ran event loop\n");

  delete_server(server);
  printf("Succesfully cleaned up server\n");

  return 0;
}
