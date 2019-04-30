#include "stdio.h"
#include "myclient.h"

int main () {
  printf("hello\n");
  void* b = init_client();
  //set_message(b);
  printf("world\n");
  delete_client(b);
  printf("success\n");
  return 0;
}
