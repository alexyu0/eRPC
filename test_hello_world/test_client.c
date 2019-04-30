#include "stdio.h"
#include "myclient.h"

int main () {
  printf("hello\n");
  void* b = init_client();
  set_message(b);
  printf("world\n");
  return 0;
}
