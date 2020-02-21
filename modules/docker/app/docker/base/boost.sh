#!/bin/sh

boost_version_major="1"
boost_version_minor="72"
boost_version_patch="0"
boost_directory="boost_${boost_version_major}_${boost_version_minor}_${boost_version_patch}"

wget http://sourceforge.net/projects/boost/files/boost/${boost_version_major}.${boost_version_minor}.${boost_version_patch}/boost_${boost_version_major}_${boost_version_minor}_${boost_version_patch}.tar.gz/download
tar -zxvf download && rm download

cd "${boost_directory}"
./bootstrap.sh \
  --with-toolset=gcc \
  --without-libraries=python
./b2 thread filesystem program_options system
./b2 install
