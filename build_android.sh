#!/bin/sh
if [ ! -d "build" ];then
    mkdir build
else
    rm -rf build/CMakeCache.txt
fi

cd build
 cmake \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_HOME/ndk/21.0.6113669/build/cmake/android.toolchain.cmake \
    -DCMAKE_MAKE_PROGRAM= $NDK_ROOT\
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_NATIVE_API_LEVEL=26 \
    -DCMAKE_BUILD_TYPE=Debug \
    .. \

make -j4
cd -