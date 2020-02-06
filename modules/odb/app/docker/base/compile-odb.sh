#!/bin/sh -ex

ODB_MAJOR_VERSION=2
ODB_MINOR_VERSION=5
ODB_PATCH_VERSION=0
ODB_SHORT_VERSION="$ODB_MAJOR_VERSION.$ODB_MINOR_VERSION"
ODB_VERSION="$ODB_MAJOR_VERSION.$ODB_MINOR_VERSION.$ODB_PATCH_VERSION-b.3"

apt-get update
apt-get install -y gcc-7-plugin-dev libpq-dev libsqlite3-dev

CUTL_VERSION_MAJOR=1
CUTL_VERSION_MINOR=11
CUTL_VERSION_PATCH=0
CUTL_VERSION="$CUTL_VERSION_MAJOR.$CUTL_VERSION_MINOR.$CUTL_VERSION_PATCH"

wget http://www.codesynthesis.com/download/libcutl/$CUTL_VERSION_MAJOR.$CUTL_VERSION_MINOR/libcutl-$CUTL_VERSION.tar.gz
tar -xvzf libcutl-$CUTL_VERSION.tar.gz
cd libcutl-$CUTL_VERSION
./configure
make
make install
cd ..

for package_name in odb libodb libodb-pgsql libodb-sqlite; do
  # Use 2.5.0 pre-release for gcc-7 support
  wget https://codesynthesis.com/~boris/tmp/odb/pre-release/b.3/$package_name-$ODB_VERSION.tar.gz
  #wget http://www.codesynthesis.com/download/odb/$ODB_SHORT_VERSION/$package_name-$ODB_VERSION.tar.gz
  tar -xzvf $package_name-$ODB_VERSION.tar.gz
  cd $package_name-$ODB_VERSION
  ./configure
  make
  make install
  cd ..
done
