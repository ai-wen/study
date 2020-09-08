#!/bin/bash

#libusb-1.0.23编译
#./configure CFLAGS=-fPIC --disable-udev --build=mips64el-linux
#./configure CFLAGS="-fPIC -g" --disable-udev --build=mips64el-linux
#./configure --host=运行平台 --build=编译平台  --target=目标平台

./configure --prefix=./bin CFLAGS="-fPIC -g" --enable-static --disable-udev --host=mips64el-linux --build=mips64el-linux --target=mips64el-linux  
