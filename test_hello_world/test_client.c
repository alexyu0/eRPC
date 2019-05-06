#include "stdio.h"
#include "cstdlib"
#include "myclient.h"

int main () {
  void* b = init_client();
  char* msg = (char*)malloc(10);
  int len = sprintf(msg, "testmsg");
  set_message(b, msg, len);
  printf("Should have sent message\n");
  return 0;
}
