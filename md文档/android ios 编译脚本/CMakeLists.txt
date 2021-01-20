cmake_minimum_required(VERSION 2.8)

#Android的set语句需要放到project()之前
#编译配置##########################################################
PROJECT(MOTP)

#指定头文件
include_directories(${PROJECT_BINARY_DIR}/../../../OTPEngine ${PROJECT_BINARY_DIR}/../../../OTPEngine/alg ${PROJECT_BINARY_DIR}/../../../OTPEngine/hash )

# 查找指定目录下的所有源文件，然后将结果存进指定变量名
aux_source_directory( ${PROJECT_BINARY_DIR}/../../../OTPEngine/alg SRC_LIST)
aux_source_directory( ${PROJECT_BINARY_DIR}/../../../OTPEngine/hash SRC_LIST)


set(CMAKE_VERBOSE_MAKEFILE on)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
	ADD_DEFINITIONS(-g)
endif()

#指定 release debug 及输出路径
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
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--retain-symbols-file=${PROJECT_BINARY_DIR}/../unixtool/export.sym -Wl,--version-script=${PROJECT_BINARY_DIR}/../unixtool/export.map")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fPIC")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fPIC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC")
elseif (CMAKE_SYSTEM_NAME MATCHES "Android")  
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/android_x64)
	else()
		set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/android_x86)
	endif()
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--retain-symbols-file=${PROJECT_BINARY_DIR}/../unixtool/export.sym -Wl,--version-script=${PROJECT_BINARY_DIR}/../unixtool/export.map")		
	
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")  
	#默认64位
	set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/osx)
	set(CMAKE_SHARED_LINKER_FLAGS "-framework Foundation -framework IOKit")	
	#set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fembed-bitcode")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fembed-bitcode")
	#set_xcode_property (CommonPlugin IPHONEOS_DEPLOYMENT_TARGET "8.3")
	#set_xcode_property (CommonPlugin CLANG_CXX_LANGUAGE_STANDARD "gnu++0x")
	#set_xcode_property (CommonPlugin CLANG_CXX_LIBRARY "libc++")
elseif (CMAKE_SYSTEM_NAME MATCHES "iOS")  
	set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/ios)
	#set(CMAKE_C_COMPILER "/Applications/Xcode.app/Contents/Developer/usr/bin/gcc")
	#set(CMAKE_CXX_COMPILER "/Applications/Xcode.app/Contents/Developer/usr/bin/gcc")
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -o2 -arch arm64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk")
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

#指定编译目标
#add_library(motp SHARED ${PROJECT_BINARY_DIR}/../../../OTPEngine/motpdll.c ${PROJECT_BINARY_DIR}/../../../OTPEngine/Util.c ${SRC_LIST})
#add_library(motp-static STATIC ${PROJECT_BINARY_DIR}/../../../OTPEngine/motpdll.c ${PROJECT_BINARY_DIR}/../../../OTPEngine/Util.c ${SRC_LIST})

if (CMAKE_SYSTEM_NAME MATCHES "iOS")  
	add_library(motp-static STATIC ${PROJECT_BINARY_DIR}/../../../OTPEngine/motpdll.c ${PROJECT_BINARY_DIR}/../../../OTPEngine/Util.c ${SRC_LIST})
else()	
	add_library(motp SHARED ${PROJECT_BINARY_DIR}/../../../OTPEngine/motpdll.c ${PROJECT_BINARY_DIR}/../../../OTPEngine/Util.c ${SRC_LIST})
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Android")
	target_link_libraries(motp m log)
endif()