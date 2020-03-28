#!/bin/bash -ex

export CHEERP_SRC=/tmp/compile-cheerp

mkdir -p $CHEERP_SRC
cd $CHEERP_SRC
								# LAST SEEN WORKING WITH TAG:
git clone https://github.com/leaningtech/cheerp-llvm		# 58650eeadc8b22cf81419ce08ffa1ee7ad21c292
cd  cheerp-llvm/tools/
git clone https://github.com/leaningtech/cheerp-clang clang	# 224dff3a1c14d2f5697e0ed1565ad019f8430442
cd $CHEERP_SRC
git clone https://github.com/leaningtech/cheerp-utils		# 63b8c37c5faead3483322ba321a3ee4ec7e3498f
git clone https://github.com/leaningtech/cheerp-newlib		# 875ad4f30b1ade8f005366e76cb66a7dbb411987
git clone https://github.com/leaningtech/cheerp-libcxx		# 1fadaf39e57b555c699d0deb2277e94f6594651c
git clone https://github.com/leaningtech/cheerp-libcxxabi	# 9ae50ecf5a7a4ab24113dff37a389cace8b535e1
git clone https://github.com/leaningtech/cheerp-libs		# 9eb512dd1f6c430dfdf43749dabb554fb6edabc5

cd $CHEERP_SRC/cheerp-llvm
mkdir build
cd build
cmake -C ../CheerpCmakeConf.cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
make install

cd $CHEERP_SRC/cheerp-utils
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt/cheerp ..
make install

cd $CHEERP_SRC/cheerp-newlib/newlib
mkdir build
cd build
../configure --host=cheerp-genericjs --with-cxx-headers=$CHEERP_SRC/cheerp-libcxx/include --prefix=/opt/cheerp --enable-newlib-io-long-long --enable-newlib-iconv --enable-newlib-iconv-encodings=utf-16,utf-8,ucs_2 --enable-newlib-mb --enable-newlib-nano-formatted-io
make
make install
CHEERP_PREFIX=/opt/cheerp ../build-bc-libs.sh genericjs

cd $CHEERP_SRC/cheerp-libcxx
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt/cheerp -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/opt/cheerp/share/cmake/Modules/CheerpToolchain.cmake -DLIBCXX_ENABLE_SHARED=OFF -DLIBCXX_ENABLE_ASSERTIONS=OFF -DLIBCXX_CXX_ABI_INCLUDE_PATHS=$CHEERP_SRC/cheerp-libcxxabi/include -DLIBCXX_CXX_ABI=libcxxabi ..
make
make install

cd $CHEERP_SRC/cheerp-libcxxabi
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt/cheerp -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/opt/cheerp/share/cmake/Modules/CheerpToolchain.cmake -DLIBCXXABI_ENABLE_SHARED=OFF -DLIBCXXABI_ENABLE_ASSERTIONS=OFF -DLIBCXXABI_LIBCXX_PATH=$CHEERP_SRC/cheerp-libcxx/ -DLIBCXXABI_LIBCXX_INCLUDES=$CHEERP_SRC/cheerp-libcxx/include -DLIBCXXABI_ENABLE_THREADS=0 -DLLVM_CONFIG=/opt/cheerp/bin/llvm-config ..
make
make install

cd $CHEERP_SRC/cheerp-libs
make -C stdlibs install_genericjs INSTALL_PREFIX=/opt/cheerp CHEERP_PREFIX=/opt/cheerp
