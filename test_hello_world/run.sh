#!/bin/bash

echo "\`run.sh\` script using" $(pwd) "as path"

cd ..; ./build_stuff.sh; cd test_hello_world

sudo cp myclient.h /usr/local/include/
sudo cp myserver.h /usr/local/include/
sudo cp myclient.h /usr/include/
sudo cp myserver.h /usr/include/
sudo cp ../build/liberpc.a /usr/local/lib/
sudo cp ../build/liberpc.a /usr/lib/

g++ -fPIC -std=c++11 -c client.cc \
  -I ../src \
  -lm -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true
g++ client.o -shared -o libclient.so

g++ -fPIC -std=c++11 -c server.cc \
  -I ../src \
  -lm -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true
g++ server.o -shared -o libserver.so

sudo cp libclient.so /usr/local/lib/
sudo cp libserver.so /usr/local/lib/
sudo cp libclient.so /usr/lib/
sudo cp libserver.so /usr/lib/

g++ -o test_client test_client.c \
  -I../src \
  -lclient -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true

g++ -o test_server test_server.c \
  -I../src \
  -lserver -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true

