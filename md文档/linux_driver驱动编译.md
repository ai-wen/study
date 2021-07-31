# Linux 驱动开发环境构建
Linux 内核模块与运行内核之间存在着版本依赖。如果编译内核模块使用的代码树与运行内核的版本不完全匹配，则在用insmod安装模块时，很可能会产生Invalid module format. 错误。
为了解决这个问题，要么使用与当前运行内核完全匹配的代码树来编译内核模块，要么用手头的内核代码重新构建并安装内核。



## 1、驱动编译的时候提示错误：
symbolmap: 00000001: invalid section

## 2、获取编译细节
make V=1
=========== Create TSR NORMAL Driver ===========
make -C /lib/modules/4.15.0-112-generic/build M=/home/lm/lkl/tsr-sdk-V2.0/driver modules
make[1]: Entering directory '/usr/src/linux-headers-4.15.0-112-generic'
test -e include/generated/autoconf.h -a -e include/config/auto.conf || (                \
echo >&2;                                                       \
echo >&2 "  ERROR: Kernel configuration is invalid.";           \
echo >&2 "         include/generated/autoconf.h or include/config/auto.conf are missing.";\
echo >&2 "         Run 'make oldconfig && make prepare' on kernel src to fix it.";      \
echo >&2 ;                                                      \
/bin/false)

## 3、下载对应操作系统版本的内核源码
1. apt-cache search linux-source
linux-source - Linux kernel source with Ubuntu patches
linux-source-4.4.0 - Linux kernel source for version 4.4.0 with Ubuntu patches
linux-source-4.10.0 - Linux kernel source for version 4.10.0 with Ubuntu patches
linux-source-4.11.0 - Linux kernel source for version 4.11.0 with Ubuntu patches
linux-source-4.13.0 - Linux kernel source for version 4.13.0 with Ubuntu patches
linux-source-4.15.0 - Linux kernel source for version 4.15.0 with Ubuntu patches
linux-source-4.8.0 - Linux kernel source for version 4.8.0 with Ubuntu patches

2. uname -a
Linux longmai 4.15.0-112-generic #113~16.04.1-Ubuntu SMP Fri Jul 10 04:37:08 UTC 2020 x86_64 x86_64 x86_64 GNU/Linux

3. apt-get install linux-source-4.15.0
tar -jxv -f 

4. ls /usr/src
linux-headers-4.15.0-112  linux-headers-4.15.0-112-generic  linux-source-4.15.0  linux-source-4.15.0.tar.bz2


5. 编译内核
apt-get install fakeroot 
apt-get install gcc make 
apt-get install pkg-config libc6-dev bison flex 
apt-get install kernel-package
apt-get install libncurses5-dev   
apt-get install libssl-dev openssl
apt-get install build-essential
apt-get install libelf-dev


配置内核,为了使得要编译的内核有正确的配置，我们最好使用当前运行系统的配置文件。
cp /boot/config-`uname -r` /usr/src/linux-source-4.15.0/linux-source-4.15.0/.config

我们将使用这个文件作为默认配置
make menuconfig
在弹出的配置界面中，选择Load an alternate configuration file菜单项，需载入的文件名为.config，选择OK。
选择退出，并保存当前配置。
#make-kpkg clean
#fakeroot make-kpkg --initrd --append-to-version=-custom kernel_image kernel_headers 

6. make oldconfig && make prepare
make
make modules
make modules_install


7. 查看内核错误信息
dmesg | tail
一、错误：Unknown symbol platform_device_register_simple
在main文件中添加 MODULE_LICENSE("GPL");
二、错误：Unknown symbol uio_unregister_device
modprobe uio

modprobe则可以一次将有依赖关系的驱动全部加载到内核。此驱动模块需要在编译内核的时候安装到内核目录。

8. 查看驱动信息
lsmod

modinfo xx.ko
modinfo tsr-drv.ko
filename:       /home/lm/DLL/tsr-sdk-V2.0/driver/tsr-drv.ko
license:        GPL
srcversion:     9B7D621387BC8BC34081107
alias:          pci:v00001DABd00008001sv*sd*bc*sc*i*
alias:          pci:v00001DABd00007001sv*sd*bc*sc*i*
depends:        uio  //依赖的驱动模块
retpoline:      Y
name:           tsr_drv
vermagic:       4.15.0-128-generic SMP mod_unload

9. 更新内核头文件
centos是 yum install kernel-devel   yum install kernel-headers
/lib/modules/$(shell uname -r)/build
/lib/modules/4.15.0-128-generic/