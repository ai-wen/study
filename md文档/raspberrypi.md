[树莓派交叉编译环境](https://hub.fastgit.org/raspberrypi/tools)

apt-get install build-essential git
git clone git://hub.fastgit.org/raspberrypi/tools.git

5个不同版本的交叉编译工具
arm-bcm2708hardfp-linux-gnueabi 
gcc-linaro-arm-linux-gnueabihf-raspbian        gcc version 4.8.3 20140303 (prerelease) (crosstool-NG linaro-1.13.1+bzr2650
arm-bcm2708-linux-gnueabi 
gcc-linaro-arm-linux-gnueabihf-raspbian-x64    gcc version 4.8.3 
arm-rpi-4.9.3-linux-gnueabihf


export PATH=$PATH:$HOME/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin
export PATH=$PATH:$HOME/rpi/tools/arm-bcm2708-linux-gnueabi/bin

export PATH=$PATH:$HOME/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
export PATH=$PATH:$HOME/rpi/tools/arm-bcm2708-linux-gnueabi/bin

CC=arm-linux-gnueabihf-gcc  
hello:hello.o  
clean:  
    rm -rf hello.o hello



# 获取内核源码
git clone --depth=1 https://hub.fastgit.org/raspberrypi/linux  

# 配置文件
在raspberrypi系统的3.x版本以前，树莓派系统默认存在/proc/config.gz文件，但是更新到4.x版本之后，/proc/config.gz文件默认不存在了，所以我们需要先获取到这个文件。 
执行命令：
modprobe configs

执行以上命令之后，会在/proc/目录下生成config.gz文件。然后将此文件从树莓派系统内拷贝到我们的PC平台
将config.gz拷贝到内核源码的根目录（~/raspberry_src/linux/），然后在需要编译的内核的顶层目录下执行命令：
cd ~/raspberry_src/linux/
zcat config.gz > .config

目的是生成.config配置文件
KERNEL=kernel7

# 编译内核
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
编译成功后，在路径arch/arm/boot/下会生成zImage镜像文件。我的虚拟机加上-jn，会出错。


# 直接安装到SD卡
在装有树莓派系统的SD卡插入电脑之前，可以先使用 lsblk 看一下，可以看到如下输出：
sda      
├─sda1  
├─sda2   
└─sda5  
插入SD卡，再使用 lsblk 看一下，可以看到新增如下输出：

sdc     
├─sdc1    
└─sdc2  

1)、挂载SD卡
mkdir mnt
mkdir mnt/fat32
mkdir mnt/ext4
sudo mount /dev/sdc1 mnt/fat32
sudo mount /dev/sdc2 mnt/ext4

2)、安装模块
这里交叉编译器最好指定路径，第一次没有指定，导致安装到SD卡的系统启动后，键盘鼠标识别不了，原因是找不到交叉编译器,导致有些库没有装上。
sudo make ARCH=arm CROSS_COMPILE=~/raspberry_src/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/arm-linux-gnueabihf- INSTALL_MOD_PATH=mnt/ext4 modules_install

执行完之后，模块就会安装到/lib目录下。

# 拷贝内核及相关文件到SD卡

拷贝之前，确保备份你的旧内核
sudo cp mnt/fat32/$KERNEL.img mnt/fat32/$KERNEL-backup.img
sudo scripts/mkknlimg arch/arm/boot/zImage mnt/fat32/$KERNEL.img//将zImage格式转成树莓派需要的img格式，并复制到SD卡
sudo cp arch/arm/boot/dts/*.dtb mnt/fat32/
sudo cp arch/arm/boot/dts/overlays/*.dtb* mnt/fat32/overlays/
sudo cp arch/arm/boot/dts/overlays/README mnt/fat32/overlays/
sudo umount mnt/fat32
sudo umount mnt/ext4

注意，编译生成的内核zImage是无法给树莓派直接用的，所以必须把它转成.img类型。 
采用交叉编译工具里的imagetool-uncompressed.py（~/raspberry_src/tools/mkimage/）转成的.img无法使用。必须使用源码目录下的scripts/mkknlimg。

升级内核的另一个办法是将img文件复制到相同目录下，使用不同的文件名，如 kernel-myconfig.img，然后修改boot目录下的config.txt文件，加入：
kernel=kernel-myconfig.img
最后，将SD卡插入树莓派启动。