#!/bin/sh

CPUS=`grep "^processor" /proc/cpuinfo | wc -l`

git submodule update --init --depth=1 boringssl
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$CPUS
strip verifier
