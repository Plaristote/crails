#!/bin/sh

git clone https://github.com/Plaristote/crails.git
cd crails
git checkout cedar14-compat
git checkout 7206a8998b60e0957a21f5260ce60e5ab1f64e2d

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
