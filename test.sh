#/usr/bin/bash

make all 
python3 ./src/python/fill_map.py
cd out
./tc_gen $1
cd ..
make run