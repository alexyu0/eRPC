#include "stdio.h"
#include "cstdlib"
#include "cstring"
#include "myclient.h"
#include "myserver.h"

int main () {
  //void* s = init_server(0);
  void* b = init_client(0);
  char* msg = (char*)malloc(10);
  int len = sprintf(msg, "c");
  set_message(b, msg, len);
  printf("Should have sent message: %s\n", msg);
  delete msg;
  /*
  msg = (char*)malloc(10);
  len = sprintf(msg, "c");
  printf("Size of msg 2: %d\n", len);
  set_message(b, msg, len);
  */
  return 0;
}
