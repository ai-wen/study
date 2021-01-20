#!/bin/bash
set -e

if [ -d ./build/output ];then
     cp -rf build/output ./
fi

if [ -d ./build ];then
      rm -rf build
      mkdir build
fi

if [ ! -d ./build ];then
      mkdir build
fi
cd build

if [ "$1" = "" ];then 
      cmake -DCMAKE_BUILD_TYPE=Release ../
fi
if [ "$1" = "-d" ];then 
      cmake -DCMAKE_BUILD_TYPE=Debug ../ 
fi



if [ "$1" = "and" ];then 

#CMAKE_TOOLCHAIN _FILE是工具链文件，这个参数非常重要，这里面还配置了很多其它参数。
#ANDROID_NDK是ndk的根目录
#ANDROID_ABI 是cpu架构
#ANDROID_PLATFORM是支持的最低android平台
#ANDROID_TOOLCHAIN是C/C++编译器，可选Clang和gcc，官网推荐clang
# cmake -DCMAKE_TOOLCHAIN_FILE=/mnt/hgfs/G/android-ndk-r20b/build/cmake/android.toolchain.cmake -DANDROID_NDK=/mnt/hgfs/G/android-ndk-r20b -DANDROID_ABI=armeabi -DANDROID_PLATFORM=android-21   -DANDROID_TOOLCHAIN=clang -DANDROID_STL=c++_shared  ../

#4 种 ABI默认输出（"armeabi-v7a" , "arm64-v8a", "x86", "x86_64"）
#"armeabi", "armeabi-v7a" , "arm64-v8a", "x86", "x86_64", "mips", "mips64"

# ndk（v17）已不在支持mips、armeabi等CPU架构，只支持armeabi-v7a, arm64-v8a, x86, x86_64 

#ndk-r20b 支持编译arm64-v8a架构的库
#-DANDROID_STL=c++_shared
#/mnt/hgfs/G/android-ndk-r10e  /mnt/hgfs/G/android-ndk-r20b
export NDK_HOME=/mnt/hgfs/G/android-ndk-r20b
if [ "$2" = "-d" ];then 
      echo "----------------------------cmake debug----------------------------"                                          
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=$NDK_HOME/build/cmake/android.toolchain.cmake -DANDROID_NDK=$NDK_HOME \
      -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-16 \
      -DANDROID_TOOLCHAIN=clang ../
else
      echo "----------------------------cmake release----------------------------"
      cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$NDK_HOME/build/cmake/android.toolchain.cmake -DANDROID_NDK=$NDK_HOME \
      -DANDROID_ABI=armeabi -DANDROID_PLATFORM=android-16 \
      -DANDROID_TOOLCHAIN=clang ../    
fi
            
fi     



if [ "$1" = "ios" ];then 
      #export CC=/Applications/Xcode.app/Contents/Developer/usr/bin/gcc
      #export CXX=/Applications/Xcode.app/Contents/Developer/usr/bin/g++     
      #if [ "$2" = "-d" ];then
      #      /Applications/CMake.app/Contents/bin/cmake -DCMAKE_BUILD_TYPE=Debug ../
      #else
      #      /Applications/CMake.app/Contents/bin/cmake -DCMAKE_BUILD_TYPE=Release ../
      #fi
      #Set -DPLATFORM to "SIMULATOR" to build for iOS simulator 32 bit (i386) DEPRECATED
      #Set -DPLATFORM to "SIMULATOR64" (example above) to build for iOS simulator 64 bit (x86_64)
      #Set -DPLATFORM to "OS" to build for Device (armv7, armv7s, arm64)
      #Set -DPLATFORM to "OS64" to build for Device (arm64)
      #Set -DPLATFORM to "OS64COMBINED" to build for Device & Simulator (FAT lib) (arm64, x86_64)
      #Set -DPLATFORM to "TVOS" to build for tvOS (arm64)
      #Set -DPLATFORM to "TVOSCOMBINED" to build for tvOS & Simulator (arm64, x86_64)
      #Set -DPLATFORM to "SIMULATOR_TVOS" to build for tvOS Simulator (x86_64)
      #Set -DPLATFORM to "WATCHOS" to build for watchOS (armv7k, arm64_32)
      #Set -DPLATFORM to "WATCHOSCOMBINED" to build for watchOS & Simulator (armv7k, arm64_32, i386)
      #Set -DPLATFORM to "SIMULATOR_WATCHOS" to build for watchOS Simulator (i386)

      if [ "$2" = "-d" ];then
            /Applications/CMake.app/Contents/bin/cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=./iostool/ios.toolchain.cmake  -DPLATFORM=SIMULATOR64 -G Xcode ../
            /Applications/CMake.app/Contents/bin/cmake --build . --config Debug
      else
            /Applications/CMake.app/Contents/bin/cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=./iostool/ios.toolchain.cmake  -DPLATFORM=SIMULATOR64 -G Xcode ../
            /Applications/CMake.app/Contents/bin/cmake --build . --config Release
      fi
else
      make
fi  
