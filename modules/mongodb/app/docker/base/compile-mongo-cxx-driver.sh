#!/bin/sh

apt-get install -y scons

mongodb_version_major="2"
mongodb_version_minor="4"
mongodb_version="${mongodb_version_major}.${mongodb_version_minor}.14"

git clone https://github.com/mongodb/mongo-cxx-driver
cd mongo-cxx-driver
git checkout legacy
scons --ssl --prefix=/usr/local install
