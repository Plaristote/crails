#!/bin/sh -ex

libmemcached_version="1.0.18"

wget "https://launchpad.net/libmemcached/1.0/1.0.18/+download/libmemcached-${libmemcached_version}.tar.gz"
tar -xzvf libmemcached-${libmemcached_version}.tar.gz
cd libmemcached-${libmemcached_version}
./configure
make && make install
