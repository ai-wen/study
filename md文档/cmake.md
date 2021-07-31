
# CMakeLists.txt

``` shell
CPack 生成安装包
在顶层的 CMakeLists.txt 文件尾部添加下面几行：
# 构建一个 CPack 安装包
include (InstallRequiredSystemLibraries) #导入 InstallRequiredSystemLibraries 模块，以便之后导入 CPack 模块；
#设置一些 CPack 相关变量，包括版权信息和版本信息，其中版本信息用了上一节定义的版本号；
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR "${Demo_VERSION_MAJOR}") 
set (CPACK_PACKAGE_VERSION_MINOR "${Demo_VERSION_MINOR}")
include (CPack) #导入 CPack 模块。

cpack -C CPackConfig.cmake #生成二进制安装包：
cpack -C CPackSourceConfig.cmake #生成源码安装包



```
```cpp
cmake_minimum_required(VERSION 3.1)  # CMake 最低版本要求

#Android的set语句需要放到project()之前

PROJECT(hello)                       # 项目名称

#添加版本号
set (Demo_VERSION_MAJOR 1)
set (Demo_VERSION_MINOR 0)
config.h.in 文件，添加两个预定义变量：
// the configured options and settings for Tutorial
#define Demo_VERSION_MAJOR @Demo_VERSION_MAJOR@
#define Demo_VERSION_MINOR @Demo_VERSION_MINOR@
printf("%s Version %d.%d\n",argv[0],Demo_VERSION_MAJOR,Demo_VERSION_MINOR);

# gdb调试
set(CMAKE_BUILD_TYPE "Debug/Release") 
set(CMAKE_VERBOSE_MAKEFILE on)          #make VERBOSE=1
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)            #_DEBUG宏定义
endif()
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


# add_subdirectory(math)  #math 目录下存在CMakeLists.txt

# 指定头文件路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_SOURCE_DIR}../include) # CMAKE_CURRENT_SOURCE_DIR 是内置宏， 表示当前 CMakeLists.txt 所在目录

# 将当前目录所有文件添加到变量 PROGRAM_SOURCE 中
aux_source_directory(. PROGRAM_SOURCE)  

#添加外部链接库查找目录
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
# 添加链接的外部库,可以是静态库也可以是动态库
link_libraries(stdc++ dl)

如果是链接指定目录指定某个库，则可以用以下方式：
target_link_libraries(hello ../mylib1.a
                      hello ../mylib2.so)
对于同一个工程中构建的库，则可以用以下方式，无需指定具体的库的位置：
target_link_libraries(hello mylib1 mylib2)

自定义编译选项
# 加入一个配置头文件，用于处理 CMake 对源码的设置
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )
option (USE_MYMATH "Use provided math implementation" ON)
# 是否加入 MathFunctions 库
if (USE_MYMATH)
  include_directories ("${PROJECT_SOURCE_DIR}/math")
  add_subdirectory (math)  
  set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)
endif (USE_MYMATH)
```
config.h.in 文件，内容如下
#cmakedefine USE_MYMATH

可以使用 ccmake 命令（也可以使用 cmake -i 命令，该命令会提供一个会话式的交互式配置界面
```

自定义编译选项
add_compile_options(-std=c++11) 
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--retain-symbols-file=${PROJECT_BINARY_DIR}/../export.sym -Wl,--version-script=${PROJECT_BINARY_DIR}/../export.map")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fPIC")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fPIC")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++11 -O2 -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC")

#生成可执行文件 静态库 动态库
add_executable(hello ${PROGRAM_SOURCE}) # 指定目标可执行文件 hello 的源代码文件为 PROGRAM_SOURCE
add_library(mylib STATIC ${SRC})
add_library(mylib SHARED ${SRC})

以 .cmake 为文件后缀的通用CMake模块
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake") #设置模块放置的目录
include(common) # 包含 common 模块，无需文件后缀名

执行外部命令
set(runcmd ${CMAKE_CURRENT_SOURCE_DIR}/build.sh)
exec_program("bash ${CMAKE_CURRENT_SOURCE_DIR}/${runcmd}")



安装和测试 make install 和 make test
# 指定 MathFunctions 库的安装路径
install (TARGETS MathFunctions DESTINATION bin)
install (FILES MathFunctions.h DESTINATION include)
libMathFunctions.o 文件将会被复制到/usr/local/bin 中，而 MathFunctions.h 会被复制到 /usr/local/include
修改 CMAKE_INSTALL_PREFIX 变量的值来指定这些文件应该拷贝到哪个根目录

# 启用测试
enable_testing()
# 测试程序是否成功运行
add_test (test_run Demo 5 2)
```

``` bash
#!/bin/bash
set -e

if [ ! -d ./build ];then
      mkdir build
fi
rm -rf build/* 
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

#ndk-r20b 支持编译arm64-v8a架构的库

export NDK_HOME=/mnt/hgfs/G/android-ndk-r20b
if [ "$2" = "-d" ];then                                   
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=$NDK_HOME/build/cmake/android.toolchain.cmake -DANDROID_NDK=$NDK_HOME \
      -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-16 \
      -DANDROID_TOOLCHAIN=clang -DANDROID_STL=c++_shared ../
else
      cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$NDK_HOME/build/cmake/android.toolchain.cmake -DANDROID_NDK=$NDK_HOME \
      -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-16 \
      -DANDROID_TOOLCHAIN=clang  -DANDROID_STL=c++_shared ../    
fi
      
fi      

```
```
cmake_minimum_required(VERSION 2.8)

#Android的set语句需要放到project()之前

PROJECT(OTPENGINE)

include_directories(${PROJECT_BINARY_DIR}/../../../OTPEngine ${PROJECT_BINARY_DIR}/../../../OTPEngine/alg ${PROJECT_BINARY_DIR}/../../../OTPEngine/hash ${PROJECT_BINARY_DIR}/../../../OTPEngine/log)

aux_source_directory( ${PROJECT_BINARY_DIR}/../../../OTPEngine SRC_LIST)
aux_source_directory( ${PROJECT_BINARY_DIR}/../../../OTPEngine/alg SRC_LIST)
aux_source_directory( ${PROJECT_BINARY_DIR}/../../../OTPEngine/hash SRC_LIST)
aux_source_directory( ${PROJECT_BINARY_DIR}/../../../OTPEngine/log SRC_LIST)

set(CMAKE_VERBOSE_MAKEFILE on)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Windows")    # for Windows
	if(CMAKE_SYSTEM_VERSION MATCHES 10.0)       # for Win10        
    elseif(CMAKE_SYSTEM_VERSION MATCHES 8.1)   # for Win8.1         
    endif()
	if(CMAKE_CL_64)
		set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/win_x64)
	else(CMAKE_CL_64)
		set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/win_x86)
	endif(CMAKE_CL_64)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")   
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/linux_x64)
	else()
		set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/linux_x86)
	endif()

	#指定 cflag cxxflag link 参数
	#add_compile_options(-std=c++11) 
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--retain-symbols-file=${PROJECT_BINARY_DIR}/../export.sym -Wl,--version-script=${PROJECT_BINARY_DIR}/../export.map")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
	#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fPIC")
	#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fPIC")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC")
	#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC")
elseif (CMAKE_SYSTEM_NAME MATCHES "Android")  
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/android_x64)
	else()
		set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/android_x86)
	endif()
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--retain-symbols-file=${PROJECT_BINARY_DIR}/../export.sym -Wl,--version-script=${PROJECT_BINARY_DIR}/../export.map")

elseif (CMAKE_SYSTEM_NAME MATCHES "Apple")  
	#默认64位
	set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/osx)
	set(CMAKE_SHARED_LINKER_FLAGS "-framework Foundation -framework IOKit")

	#set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fembed-bitcode")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fembed-bitcode")
	#set_xcode_property (CommonPlugin IPHONEOS_DEPLOYMENT_TARGET "8.3")
	#set_xcode_property (CommonPlugin CLANG_CXX_LANGUAGE_STANDARD "gnu++0x")
	#set_xcode_property (CommonPlugin CLANG_CXX_LIBRARY "libc++")
endif()


MESSAGE(STATUS "--------------------------------------------" )
MESSAGE(STATUS "CMAKE_SYSTEM_NAME = " ${CMAKE_SYSTEM_NAME})
MESSAGE(STATUS "CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})
#MESSAGE(STATUS "src = " ${SRC_LIST})
MESSAGE(STATUS "outdir = " ${LIBRARY_OUTPUT_PATH})
MESSAGE(STATUS "--------------------------------------------" )
MESSAGE(STATUS "CMAKE_C_FLAGS = " ${CMAKE_C_FLAGS})
MESSAGE(STATUS "CMAKE_C_FLAGS_DEBUG = " ${CMAKE_C_FLAGS_DEBUG})
MESSAGE(STATUS "CMAKE_C_FLAGS_RELEASE = " ${CMAKE_C_FLAGS_RELEASE})
MESSAGE(STATUS "CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
MESSAGE(STATUS "CMAKE_CXX_FLAGS_DEBUG = " ${CMAKE_CXX_FLAGS_DEBUG})
MESSAGE(STATUS "CMAKE_CXX_FLAGS_RELEASE = " ${CMAKE_CXX_FLAGS_RELEASE})
MESSAGE(STATUS "CMAKE_SHARED_LINKER_FLAGS = " ${CMAKE_SHARED_LINKER_FLAGS})
MESSAGE(STATUS "--------------------------------------------" )
MESSAGE(STATUS "The BINARY dir =" ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "The SOURCE dir =" ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "CMAKE_HOST_SYSTEM = " ${CMAKE_HOST_SYSTEM})
MESSAGE(STATUS "CMAKE_HOST_SYSTEM_NAME = " ${CMAKE_HOST_SYSTEM_NAME})
MESSAGE(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR = " ${CMAKE_HOST_SYSTEM_PROCESSOR})
MESSAGE(STATUS "CMAKE_HOST_SYSTEM_VERSION = " ${CMAKE_HOST_SYSTEM_VERSION})
MESSAGE(STATUS "CMAKE_HOST_UNIX = " ${CMAKE_HOST_UNIX})
MESSAGE(STATUS "CMAKE_HOST_WIN32 = " ${CMAKE_HOST_WIN32})
MESSAGE(STATUS "CMAKE_SYSTEM_NAME = " ${CMAKE_SYSTEM_NAME})
MESSAGE(STATUS "CMAKE_SYSTEM_PROCESSOR = " ${CMAKE_SYSTEM_PROCESSOR})
MESSAGE(STATUS "CMAKE_SYSTEM_VERSION = " ${CMAKE_SYSTEM_VERSION})
MESSAGE(STATUS "CMAKE_SIZEOF_VOID_P = " ${CMAKE_SIZEOF_VOID_P})
MESSAGE(STATUS "--------------------------------------------" )

add_library(mOTPEngine SHARED ${SRC_LIST})
target_link_libraries(mOTPEngine stdc++ dl)

```