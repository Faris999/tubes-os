#/usr/bin/bash

make build-run
cd out
./tc_gen $1
cd ..
make run