#!/bin/bash

cd $(dirname $(readlink -f $0)); CURRENT=$(pwd); cd -
MACH=$(echo $(basename ${0%.*}) | awk -F - '{print $2}')
test -z "$toolchain" && toolchain=$CURRENT/scripts/toolchain.cmake
test -z "$build_type" && build_type=release

rm -rf build; mkdir build && cd build \
    && cmake $CURRENT -DCMAKE_TOOLCHAIN_FILE=$toolchain -DCMAKE_BUILD_TYPE=$build_type -DMACHINE=$MACH -Wno-dev && make -j8
