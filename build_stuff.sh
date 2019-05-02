#!/usr/bin/env bash
cmake . -DPERF=OFF -DTRANSPORT=infiniband -DROCE=on; make -j
find src -type f -regex ".*.h" -print0 | xargs -0 sudo cp -t /usr/local/include

