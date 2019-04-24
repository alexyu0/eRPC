#!/usr/bin/env bash

make -j
objs=`find CMakeFiles/erpc.dir/src -type f -regex ".*.o" | tr '\n' ' '`
g++ $objs -shared -o liberpc.so

sudo cp liberpc.so /usr/lib
sudo cp liberpc.so /usr/local/lib

find src -type f -regex ".*.h" -print0 | xargs -0 sudo cp -t /usr/include
find src -type f -regex ".*.h" -print0 | xargs -0 sudo cp -t /usr/local/include

