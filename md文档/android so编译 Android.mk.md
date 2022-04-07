[下载ndk](https://developer.android.google.cn/ndk/downloads?hl=zh-cn)
https://github.com/android/ndk/wiki/Unsupported-Downloads




ndk-build.cmd  NDK_PROJECT_PATH=%cd% APP_BUILD_SCRIPT=jni/so.mk -j4

ndk-build.cmd -j4
ndk-build.cmd  clean

# 一、创建jni 子目录放置源码
编译时默认找jni目录下源码进行编译，所以需要在工程目录下新建jni子目录

# 二、创建Android.mk
```ini
示例：
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)  

LOCAL_MODULE    := lesson2 #so库引用名称  编译时会自动添加lib前缀，最后的结果为libkey.so
LOCAL_SRC_FILES := Lesson2.cpp MyTest.cpp module.c# 需要编译的源码

LOCAL_LDLIBS    := -llog -ldl -lmediandk -lGLESv2
LOCAL_CPPFLAGS += -O3
LOCAL_CFLAGS += -fvisibility=hidden
include $(BUILD_SHARED_LIBRARY) #编译动态库  

```
```-fvisibility=hidden
g++ main.cpp libA.so libB.so -o main

假如 libA.so libB.so中有全局同名的函数，main.cpp调用了这个函数，
GLOBAL的符号即全局的符号，同名的全局符号会被认为是同一个符号，由于main先加载了libA，得到了libA中的subfunc符号，再加载libB时，就把libB中的subfunc忽略了。
既然有GLOBAL符号，那自然会有LOCAL符号，LOCAL的符号只在当前lib可见，全局不可见。
如何将符号变成LOCAL的呢，最直接的就是加上visibility为hidden的标志，修改后的libA.cpp：
__attribute__ ((visibility ("hidden"))) int subfunc(int a, int b) {
    return a + b;
}

指定需要导出的函数为visibility ("default")，编译时使用-fvisibility=hidden,则只有指定的函数为GLOBAL的符号
__attribute__ ((visibility ("default"))) int funcA(int a, int b) {return subfunc(a, b);}

```


# 编译完成后会在jni同级目录下生成libs和obj两个文件夹，
libs目录下存放的是编译好的so库，obj目录下存放的是.o链接库, 如果有需要，使用以下命令可以清除obj目录下文件：
ndk-build.cmd  clean
同级目录 jni libs obj



# 默认情况下使用Andorid.mk作为编译脚本，如果想指定编译脚本文件可以添加APP_BUILD_SCRIPT参数进行指定：
ndk-build.cmd  NDK_PROJECT_PATH=G:\project\3K3_GM\4.Development\middleware\prj_android APP_BUILD_SCRIPT=new_android.mk  

# 默认情况下编译只生成armeabi架构的so库，如果要生成x86以及其他cpu架构的so，需要在Android.mk文件所在目录下新建Application.mk文件，并加入以下配置：
我们一般在Application.mk中指定所需要的平台版本和编译后so的架构：
APP_PLATFORM := android-18  #指编译时链接的动态库和编译工具链平台 G:\android-ndk-r20b-w\platforms\android-21  
APP_ABI := armeabi armeabi-v7a x86
或者编译所有架构
APP_ABI := all # 注意必须小写
## 默认情况下使用Andorid.mk同级目录下找Application.mk 如果想指定Application.mk文件路径，可以添加NDK_APPLICATION_MK进行指定：
ndk-build.cmd  NDK_PROJECT_PATH=G:\project\3K3_GM\4.Development\middleware\prj_android NDK_APPLICATION_MK=new_application.mk



# 三、安卓项目引用库
android工程在引用so库时,必须放在对应架构目录下,否则打包apk时会忽略该文件的打包, 比如指定jni目录为libs ,引用的so是x86架构的,那么必须在libs目录下再新建一个名为x86的文件夹,然后将so放进去, 否则so库不打包
 



## 引入库
LOCAL_SHARED_LIBRARIES和LOCAL_STATIC_LIBRARIES是引入系统的，LOCAL_LDFLAGS是引用第三方的。
LOCAL_SHARED_LIBRARIES += 动态库
LOCAL_STATIC_LIBRARIES += 静态库
LOCAL_LDFLAGS := 静态动态都可以

LOCAL_LDFLAGS := -L(路径) -l(模块名)  这是动态的引入方法
LOCAL_LDFLAGS := (.a文件所在的全路径)  这是静态的引入方法
