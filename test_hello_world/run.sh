g++ -fPIC -std=c++11 -c client.cc \
  -I ../src -L ../build \
  -lm -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true
g++ client.o -shared -o libclient.so

g++ -fPIC -std=c++11 -c server.cc \
  -I ../src -L ../build \
  -lm -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true

g++ -o test_client test_client.c \
  -I../src -L../build \
  -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true

