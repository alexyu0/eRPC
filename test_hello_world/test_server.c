#include "stdio.h"
#include "myserver.h"

int main () {
  printf("hello\n");
  erpc_server_t server = init_server();
  printf("Got server\n");
  test_server(server);
  printf("world\n");

  return 0;
}
