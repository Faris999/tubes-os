#/usr/bin/bash

cd out
./tc_gen $1
cd ..
make run