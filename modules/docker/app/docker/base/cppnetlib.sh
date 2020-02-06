#!/bin/sh

cppnetlib_version="0.11.2"
cppnetlib_release="final"
cppnetlib_filename="cpp-netlib-${cppnetlib_version}-${cppnetlib_release}.tar.gz"

wget http://downloads.cpp-netlib.org/${cppnetlib_version}/${cppnetlib_filename}
tar -zxvf ${cppnetlib_filename} && rm ${cppnetlib_filename}

cd cpp-netlib-${cppnetlib_version}-${cppnetlib_release}
cmake . && make && make install

cd ..