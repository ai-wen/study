# https://developers.google.cn/

# 1.源码下载;
Google采用Git对AOSP项目进行多仓库管理.
为了方便需要，对git语句进行了封装，也就形成了repo.

sudo apt install repo

大家在使用repo的时候，都会先执行一条repo init的命令，这条命令就是去下载manifest仓库的代码，里面包含一个manifest.xml的文件，内容就是各个子项目的git仓库路径。

```conf
打开manifest.xm文件，可以看到以下的结构：
<remote  name="aosp" fetch="https://android.googlesource.com" />
<project path="abi/cpp" name="platform/abi/cpp" groups="pdk" />
remote代表的是一个远程仓库，project代表的是仓库下的工程路径。
manifest.xml里会包含一个default标签：
<default revision="refs/tags/android-5.1.1_r13"
    remote="aosp"
    sync-j="4" />
指明了默认的远程仓库是aosp。非默认仓库的，需要在project标签里用remote字段指明。 
因此，这里如果要用清华的aosp镜像服务器，只要把的remote中的地址替换成清华大学的镜像服务器地址即可，即：
<remote  name="aosp" fetch="https://aosp.tuna.tsinghua.edu.cn" />
```
```conf
aosp项目下的源码，可能看到的manifest中remote格式是：
 <remote  name="aosp" fetch=".." />
看到fetch=".."可能会纳闷，实际上这里相当于声明了代码服务器路径是repo init时的地址加上../
比如repo init -u https://android.googlesource.com/platform/manifest
fetch=".."相当于 https://android.googlesource.com/platform/../
```

## Android 清华镜像使用帮助 https://mirror.tuna.tsinghua.edu.cn/help/AOSP/

### 传统初始化方法
repo init -u https://mirrors.tuna.tsinghua.edu.cn/git/AOSP/platform/manifest  
如果提示无法连接到 gerrit.googlesource.com，  
export REPO_URL='https://mirrors.tuna.tsinghua.edu.cn/git/git-repo'

如果需要某个特定的 Android 版本(列表)：  
repo init -u https://mirrors.tuna.tsinghua.edu.cn/git/AOSP/platform/manifest -b android-4.0.1_r1  

同步源码树（以后只需执行这条命令来同步）：  
repo sync

### 使用每月更新的初始化包
wget -c https://mirrors.tuna.tsinghua.edu.cn/aosp-monthly/aosp-latest.  tar # 下载初始化包  
tar xf aosp-latest.tar  
cd AOSP   # 解压得到的 AOSP 工程目录  
这时 ls 的话什么也看不到，因为只有一个隐藏的 .repo 目录  
repo sync # 正常同步一遍即可得到完整目录



# 2.构建编译环境;
Ubuntu 14.04  Android 6.0至AOSP master     OpenJDK 8
Ubuntu 12.04  Android 2.3.x至Android 5.x   2.3.x-4.4.x OpenJDK 6    5.x至 6.0  OpenJDK 7
Ubuntu 10.04  Android 1.5至Android 2.2.x   Oracle JDK 5

### Ubuntu 16.4 
sudo apt-get update
sudo apt-get install openjdk-8-jdk

sudo apt-get update
sudo apt-get install git-core gnupg flex bison gperf build-essential
sudo apt-get install zip curl zlib1g-dev gcc-multilib g++-multilib libc6-dev-i386
sudo apt-get install lib32ncurses5-dev x11proto-core-dev libx11-dev lib32z-dev ccache
sudo apt-get install libgl1-mesa-dev libxml2-utils xsltproc unzip m4

# 3.编译源码;
 编译目标的格式:BUILD-BUILDTYPE,
 aosp_arm-eng 的B UILD是aosp_arm,  BUILDTYPE是eng.

BUILD指的是特定功能的组合的特定名称,即表示编译出的镜像可以运行在什么环境.
其中,aosp(Android Open Source Project)代表Android开源项目;
arm表示系统是运行在arm架构的处理器上,arm64则是指64位arm架构处理器,x86则表示x86架构的处理器;此外,还有一些单词代表了特定的Nexus设备.
eng表示engineer版本，其直接开放了一些root等权限。
unch命令会列出所有可选的编译目标

source build/envsetup.sh 初始化编译环境
lunch aosp_arm64-eng 通过lunch指令可以设置编译目标
make -j8


在编译高版本android源码时，可能出现的jdk版本问题，我之前是使用上面命令介绍的方式配置了openjdk却在java -version发现还是不是open jdk版本，究其原因竟是我在更之前配置android studio时在/etc/profile 中配置了另一个jdk版本导致该问题发生，后面在/etc/profile中删除对应配置就好了。

Out of memory error 编译过程中JVM heap size太小而导致的，解决办法比较简单
export JACK_SERVER_VM_ARGUMENTS="-Dfile.encoding=UTF-8 -XX:+TieredCompilation -Xmx4096m"
~/Android/source/prebuilts/sdk/tools/jack-admin kill-server
~/Android/source/prebuilts/sdk/tools/jack-admin start-server
这个配置可以讲JVM heap size调整为4g，这个应该是够用的，这里我们看到了jack-admin工具，后面可能还会使用这个工具，所以这里直接建议配置jack-admin的环境变量
export PATH=$PATH:~/Android/source/prebuilts/sdk/tools
jack-admin start-server

sizeof (_nl_value_type_LC_TIME[0]))’ ???
去除本地化配置解决
export LC_ALL=C

No Jack server running. Try ‘jack-admin start-server’
是jack-admin没有启动的原因，重启便可
jack-admin kill-server
jack-admin start-server

## 单独编译模块及SDK
除了通过make命令编译可以整个android源码外，Google也为我们提供了相应的命令来支持单独模块的编译。

编译环境初始化(即执行. build/envsetup.sh)之后，我们可以得到一些有用的指令，除了上边用到的lunch，在envsetup.sh文件中我们还可以找到如下的指令 
其中的mmm指令就可以用来编译指定模块，
mmm packages/apps/StorageManager/
稍等一会之后，如果提示编译完成，此时便可在out/target/product/gereric_arm64/system/priv-app就可以找到编译的StorageManager.apk文件了。

编译好指定模块后，如果我们想要将该模块对应的apk集成到系统镜像中，需要借助make snod指令重新打包系统镜像，这样我们新生成的system.img中就包含了刚才编译的Launcher2模块了，重启模拟器之后生效。

我们在不断的修改某些模块，总不能每次编译完成后都要重新打包system.img，然后重启手机吧?有没有什么简单的方法呢?
在编译完后，借助adb install命令直接将生成的apk文件安装到设备上即可，相比使用make snod，会节省很多时间。

直接执行make是不包括make sdk的，如果要编译自己的sdk则十分简单，只需要执行如下命令便可
. build/envsetup.sh
lunch sdk-eng
make sdk

如果编译成功，不出意外，在out/host/linux-x86/sdk就可以看到了。

补充
Android系统自带的apk文件都在out/target/product/generic_arm64/system/priv-app目录下
一些可执行文件(比如C编译的执行),放在out/target/product/generic_arm64/system/bin目录下
动态链接库放在out/target/product/generic_arm64/system/lib目录下
硬件抽象层文件都放在out/targer/product/generic_arm64/system/lib/hw目录下

这里需要注意的就是由于我们这里编译的是aosp_arm64-eng编译目标，所以这里的路径中是generic_arm64，如果是编译的其它目标路径可能不一样，比如当编译的是aosp_arm-eng时，则路径中就是generic。


# 4.运行.
source build/envsetup.sh
emulator
运行模拟器实际上需要四个组件
1、Linux Kernel
2、system.img
3、userdata.img
4、ramdisk.img
如果你在使用lunch命令时选择的是aosp_arm-eng，那么在执行不带参数的emualtor命令时，Linux Kernel默认使用的是/source/prebuilds/qemu-kernel/arm/kernel-qemu目录下的kernel-qemu文件。而android镜像文件则是默认使用source/out/target/product/generic目录下的system.img、userdata.img和ramdisk.img，也就是我们刚刚编译出来的镜像文件。

在使用lunch命令时选择的是aosp_arm64-eng，因此linux默认使用的/source/prebuilds/qemu-kernel/arm64/kernel-qemu下的kernel-qemu，而其他文件则是使用的source/out/target/product/generic64目录下的system.img、userdata.img和ramdisk.img。
