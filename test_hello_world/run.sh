g++ -g -fPIC -std=c++11 -c client.cc -I ../src -L ../build -lerpc -lpthread -lnuma -ldl -libverbs -DINFINIBAND=true
g++ client.o -shared -o libclient.so
gcc -L/users/alexyu0/eRPC/test_hello_world -Wall -o test_client test_client.c -lclient -libverbs
