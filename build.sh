#!/bin/sh
if [ ! -d "build" ];then
    mkdir build
else
    rm -rf build/CMakeCache.txt
fi

cd build
cmake \
-G "Visual Studio 15 Win64" \
.. 
cd -