#!/bin/sh -ex

imagemagick_version="6.7.7-10"
wget "http://www.imagemagick.org/download/releases/ImageMagick-$imagemagick_version.tar.xz"
tar xvfJ ImageMagick-$imagemagick_version.tar.xz
cd ImageMagick-$imagemagick_version
./configure
make && make install

rm /usr/local/lib/libMagick{Core,Wand}.so.5
