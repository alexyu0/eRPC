#include "stdio.h"
#include "cstdlib"
#include "cstring"
#include "myclient.h"

int main () {
  void* b = init_client();
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
