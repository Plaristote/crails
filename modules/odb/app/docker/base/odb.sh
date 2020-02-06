#!/bin/sh -ex

ODB_MAJOR_VERSION=2
ODB_MINOR_VERSION=4
ODB_PATCH_VERSION=0
ODB_SHORT_VERSION="$ODB_MAJOR_VERSION.$ODB_MINOR_VERSION"
ODB_VERSION="$ODB_MAJOR_VERSION.$ODB_MINOR_VERSION.$ODB_PATCH_VERSION"

apt-get update
apt-get install -y gcc-5-plugin-dev

CUTL_VERSION_MAJOR=1
CUTL_VERSION_MINOR=10
CUTL_VERSION_PATCH=0
CUTL_VERSION="$CUTL_VERSION_MAJOR.$CUTL_VERSION_MINOR.$CUTL_VERSION_PATCH"

wget http://www.codesynthesis.com/download/libcutl/$CUTL_VERSION_MAJOR.$CUTL_VERSION_MINOR/libcutl-$CUTL_VERSION.tar.gz
tar -xvzf libcutl-$CUTL_VERSION.tar.gz
cd libcutl-$CUTL_VERSION
./configure
make
make install
cd ..

for package_name in odb libodb libodb-pgsql ; do
  wget http://www.codesynthesis.com/download/odb/$ODB_SHORT_VERSION/$package_name-$ODB_VERSION.tar.gz
  tar -xzvf $package_name-$ODB_VERSION.tar.gz
  cd $package_name-$ODB_VERSION
  ./configure
  make
  make install
  cd ..
done

