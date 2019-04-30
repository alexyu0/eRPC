g++ -fPIC -std=c++11 -c client.cc \
  -I ../src -L ../build \
  -lm -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true
g++ client.o -shared -o libclient.so

#g++ -fPIC -std=c++11 -c server.cc \
#  -I ../src -L ../build \
#  -lm -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true
#g++ server.o -shared -o libserver.so

g++ -o test_client test_client.c \
  -I../src -L../build -L/users/lizziyin/myeRPC/eRPC/test_hello_world \
  -Wl,-rpath=/users/lizziyin/myeRPC/eRPC/test_hello_world \
  -lclient -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true

#g++ -o test_server test_server.c \
#  -I../src -L../build -L/users/lizziyin/myeRPC/eRPC/test_hello_world \
#  -Wl,-rpath=/users/lizziyin/myeRPC/eRPC/test_hello_world \
#  -lserver -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true

sudo cp libclient.so /usr/lib/
sudo cp libclient.so /usr/local/lib/
sudo cp libserver.so /usr/lib/
sudo cp libserver.so /usr/local/lib/
