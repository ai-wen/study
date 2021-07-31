# OpenSC 0.21.0 is the latest stable version released on 24.11.2020. It is available as
最新编译的稳定的release版本OpenSC 0.21.0

## OpenSC工作由各种子项目组成，这些子项目也可以独立使用，无需OpenSC:
libp11 is a wrapper library for PKCS#11 modules with OpenSSL interface
pkcs11-helper is a wrapper library for PKCS#11 modules with extended callback mechanisms for user and token interaction
PAM-PKCS#11 is a feature rich plugable authentication module (PAM) for authentication via PKCS#11 modules, which includes various tools to controls the login process
pam_p11 is a simple plugable authentication module (PAM) for authentication via PKCS#11 modules
OpenCT implements a reader driver interface for various non-standard readers on Linux
OpenSC-Java is a Java PKCS#11 wrapper and JCE Provider





# OpenSC nightly 
Nightly Build,也叫Daily Build), 是将一个软件项目的所有最新代码取出,从头开始编译,链接,运行测试软件包对代码进行单元测试并对主要功能进行测试,发现错误并报告错误的完整过程.每日 构建是连续集成的一个最佳实践,它要求每天至少构建软件一次.因为对于许多大型项目来说,每次构建花掉的时间可能高达几个小时,在白天进行构建可能会消耗 过多的计算机资源,对开发造成一定的影响,所以许多大型项目的每日构建是在夜间无人工作或者人比较少的时候进行的.所以Daily Build就成了Nightly Build.
### Get OpenSC
git clone https://github.com/OpenSC/OpenSC --single-branch
cd OpenSC

### Get name of branch in Nightly which corresponds to the latest commit in OpenSC
BRANCH=`git log --max-count=1 --date=short --abbrev=8 --pretty=format:"%cd_%h"`

### Download the build
curl https://github.com/OpenSC/Nightly/archive/${BRANCH}.zip -L --output ${BRANCH}.zip

#### Unpack the build
unzip ${BRANCH}.zip

Some build artifacts may be split up into multiple chunks due to file size limitations. The chunks have extensions in alphabetic order (.aa, .ab, ...). To concatenate them, use something like the following in a terminal:
#### Recreate the macOS image
cat OpenSC*.dmg.* > OpenSC.dmg
REM Recreate the Windows debug symbols
copy /b OpenSC-0.21.0-rc1_win64-Debug.zip.aa+OpenSC-0.21.0-rc1_win64-Debug.zip.ab OpenSC-Debug.zip





# https://hub.fastgit.org/OpenSC/OpenSC/wiki/Compiling-on-Windows

必须的依赖 
CPDK WiX Toolset
可选的依赖 ，建议都屏蔽，可以快速编译
OpenSSL (optional)
OpenPACE (optional)
zlib (optional)

## 1、WiX Toolset Visual Studio 打包工具 https://wixtoolset.org/
    该工具集从XML源代码构建你的Windows安装程序包。可以无缝集成到构建过程。
    https://blog.csdn.net/catshitone/article/details/77802365
## 2、CPDK Cryptographic Provider Development Kit https://www.microsoft.com/en-us/download/details.aspx?id=30688
http://download.microsoft.com/download/2/C/9/2C93059C-0532-42DF-8C24-9AEAFF00768E/cngsdk.msi
## 3、OpenSSL (optional)
D:\software\OpenSSL-1.0.2u-Win32\bin\openssl.exe version
OpenSSL 1.0.2u  20 Dec 2019

## 4、OpenPACE (optional) 编译 https://hub.fastgit.org/frankmorgner/openpace/releases/latest

set OPENSSL=D:\software\OpenSSL-1.0.2u-Win32
set OPENSSL_CONF=D:\software\OpenSSL-1.0.2u-Win32\bin\openssl.cfg

set OPENSSL=D:\software\OpenSSL-1.1.1f-Win32

set OPENSSL=D:\software\OpenSSL-1.1.0f-Win32
set OPENSSL_CONF=D:\software\OpenSSL-1.1.0f-Win32\bin\openssl.cfg
cd openpace-1.1.1\src

cl  /I%OPENSSL%\include /I. /DX509DIR=\"/\" /DCVCDIR=\"/\" /W3 /D_CRT_SECURE_NO_DEPRECATE /DWIN32_LEAN_AND_MEAN /GS /MT /c ca_lib.c cv_cert.c cvc_lookup.c x509_lookup.c eac_asn1.c eac.c eac_ca.c eac_dh.c eac_ecdh.c eac_kdf.c eac_lib.c eac_print.c eac_util.c misc.c pace.c pace_lib.c pace_mappings.c ri.c ri_lib.c ta.c ta_lib.c objects.c /Wall

lib /out:libeac.lib ca_lib.obj cv_cert.obj cvc_lookup.obj x509_lookup.obj eac_asn1.obj eac.obj eac_ca.obj eac_dh.obj eac_ecdh.obj eac_kdf.obj eac_lib.obj eac_print.obj eac_util.obj misc.obj pace.obj pace_lib.obj pace_mappings.obj ri.obj ri_lib.obj ta.obj ta_lib.obj objects.obj /VERBOSE

## 5、zlib  (optional)编译
git clone https://hub.fastgit.org/madler/zlib.git
nmake -f win32/Makefile.msc LOC="-DASMV -DASMINF" OBJA="inffas32.obj match686.obj" zlib.lib
nmake -f win32/Makefile.msc AS=ml64 LOC="-DASMV -DASMINF -I." OBJA="inffasx64.obj gvmat64.obj inffas8664.obj" zlib.lib


### win32/Make.rules.mak 修改编译配置---------------------------------------------------------
WIX for WiX Toolset
OPENSSL_DEF for OpenSSL
OPENPACE_DEF and OPENPACE_DIR for OpenPACE
ZLIBSTATIC_DEF, ZLIB_LIB and ZLIB_INCL_DIR for zlib

#### 使用Autoconf 自动创建构建的源文件方式 ---------------------------------------------------------
http://repo.msys2.org/distrib/i686/
https://www.msys2.org/wiki/How-does-MSYS2-differ-from-Cygwin/

MSYS2
Updating packages
pacman -Syuu
pacman -Sy autoconf automake libtool make pkg-config
Removing packages: pacman -R <package_names|package_groups>
Searching for packages: pacman -Ss <name_pattern>

#### PGP签名问题 https://www.pianshen.com/article/60651722284/
１． 下载 msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz 软件包
curl -O http://repo.msys2.org/msys/x86_64/msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz
curl -O http://repo.msys2.org/msys/x86_64/msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz.sig
２．验证签名是否建立完好
pacman-key --verify msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz{.sig,}
３．安装 msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz 包
pacman -U --config <(echo) msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz


下载mingw32 i686-8.1.0-release-posix-sjlj-rt_v6-rev0.7z
https://sourceforge.net/projects/mingw-w64/files/

pacman -Sy autoconf automake libtool make pkg-config
将 i686-8.1.0-release-posix-sjlj-rt_v6-rev0.7z 解压到 C:\msys32 路径后面使用gcc

##### 
使用Cygwin或MSYS2安装：
autoconf
automake
libtool
make
gcc or mingw-w64 (will not be used for build)
pkg-config


Open a Cygwin/MSYS2 terminal or console and change to the OpenSC directory

autoreconf -i
./configure --disable-openssl --disable-readline --disable-zlib
make -C etc opensc.conf
cp win32/winconfig.h config.h

#### 手动创建构建的源文件方式 ---------------------------------------------------------
copy src\minidriver\versioninfo-minidriver.rc.in src\minidriver\versioninfo-minidriver.rc
copy src\pkcs11\versioninfo-pkcs11.rc.in src\pkcs11\versioninfo-pkcs11.rc
copy src\pkcs11\versioninfo-pkcs11-spy.rc.in src\pkcs11\versioninfo-pkcs11-spy.rc
copy src\tools\versioninfo-tools.rc.in src\tools\versioninfo-tools.rc
copy win32\OpenSC.wxs.in win32\OpenSC.wxs
copy win32\versioninfo-customactions.rc.in win32\versioninfo-customactions.rc
copy win32\versioninfo.rc.in win32\versioninfo.rc
copy win32\winconfig.h.in win32\winconfig.h


# 编译 进入到VS 控制台---------------------------------------------------------
nmake /f Makefile.mak
cd win32
nmake /f Makefile.mak OpenSC.msi


# 改配置 ---------------------------------------------------------
When the new CPDK is used, you shall update the Include path:
--- a/win32/Make.rules.mak
+++ b/win32/Make.rules.mak
@@ -131,7 +131,7 @@ CANDLEFLAGS = -dOpenPACE="$(OPENPACE_DIR)" $(CANDLEFLAGS)

 # Used for MiniDriver
-CNGSDK_INCL_DIR = "/IC:\Program Files (x86)\Microsoft CNG Development Kit\Include"
+CNGSDK_INCL_DIR = "/IC:\Program Files (x86)\Windows Kits\10\Cryptographic Provider Development Kit\Include"
 !IF "$(PROCESSOR_ARCHITECTURE)" == "x86" && "$(PROCESSOR_ARCHITEW6432)" == ""
 CNGSDK_INCL_DIR = "/IC:\Program Files\Microsoft CNG Development Kit\Include"
 !ENDIF