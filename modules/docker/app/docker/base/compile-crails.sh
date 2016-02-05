#!/bin/sh

git clone https://github.com/Plaristote/crails.git
cd crails

# Production/Synchronous
cmake -DDEVELOPER_MODE=OFF \
      -DUSE_MULTITHREAD=OFF
make clean && make && make install
rm CMakeCache.txt

# Production/Asynchronous
cmake -DDEVELOPER_MODE=OFF \
      -DUSE_MULTITHREAD=ON
make clean && make && make install
rm CMakeCache.txt

# Development/Synchronous
cmake -DDEVELOPER_MODE=ON \
      -DUSE_MULTITHREAD=OFF
make clean && make && make install
rm CMakeCache.txt

# Development/Asynchronous
cmake -DDEVELOPER_MODE=ON \
      -DUSE_MULTITHREAD=ON
make clean && make && make install
rm CMakeCache.txt
