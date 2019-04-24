g++ -fPIC -std=c++11 -c client.cc \
  -I ../src -L ../build \
  -lm -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true
g++ client.o -shared -o libclient.so

g++ -fPIC -std=c++11 -c server.cc \
  -I ../src -L ../build \
  -lm -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true

g++ -o test_client test_client.c \
  -I../src -L../build -L/users/alexyu0/eRPC/test_hello_world \
  -Wl,-rpath=/users/alexyu0/eRPC/test_hello_world \
  -lclient -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true

