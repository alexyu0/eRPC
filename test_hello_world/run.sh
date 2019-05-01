#!/bin/bash

echo "\`run.sh\` script using" $(pwd) "as path"

g++ -fPIC -std=c++11 -c client.cc \
  -I ../src -L ../build \
  -lm -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true
g++ client.o -shared -o libclient.so

g++ -fPIC -std=c++11 -c server.cc \
  -I ../src -L ../build \
  -lm -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true
g++ server.o -shared -o libserver.so

sudo cp myclient.h /usr/include/
sudo cp myclient.h /usr/local/include/
sudo cp myserver.h /usr/include/
sudo cp myserver.h /usr/local/include/
sudo cp libclient.so /usr/lib/
sudo cp libclient.so /usr/local/lib/
sudo cp libserver.so /usr/lib/
sudo cp libserver.so /usr/local/lib/

g++ -o test_client test_client.c \
  -I../src -L../build -L$(pwd) \
  -Wl,-rpath=$(pwd) \
  -lclient -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true

g++ -o test_server test_server.c \
  -I../src -L../build -L$(pwd) \
  -Wl,-rpath=$(pwd) \
  -lserver -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true

