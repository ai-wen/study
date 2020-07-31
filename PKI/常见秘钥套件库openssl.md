
openssl
gmssl
tassl
gnustl
mbedtls
bouncycastlepp
libtomcrypt
matrixssl-4-2-1

botan

mkcert


#### 下载openssl-1.1.1g源码
https://www.openssl.org/source/openssl-1.1.1g.tar.gz

#### 打开并使用 Visual Studio 2017 Command Prompt 命令行

#### 配置参数
32位和64位的release和debug版本  
VC-WIN32  debug-VC-WIN32  
VC-WIN64A debug-VC-WIN64A  
动态库和静态库  
shared  动态库  
no-shared 静态库  

#### 配置组合
perl Configure VC-WIN32  shared  32位release动态库  
perl Configure VC-WIN32  no-shared 32位release静态库  
perl Configure debug-VC-WIN32  shared 32位debug动态库 
perl Configure debug-VC-WIN32  no-shared 32位debug动态库  

#### 示例 
编译可调式debug版本32位动态库 步骤  
1.perl Configure debug-VC-WIN32  shared no-asm --prefix="D:/Git/SSL/openssl-1.1.1g/win32-release" --openssldir="D:/Git/SSL/openssl-1.1.1g/win32-release/ssl"  

2.nmake  
3.nmake  install  
4.nmake test  

nmake时编译提示错误：NMAKE : fatal error U1077: “rc”: 返回代码“0x1”  
分析原因：  
将Rc.exe路径添加到系统环境变量PATH，我的路径是：  
C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin  
C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin\x64  


#### MT 和 MD
常规windosw下提供的库都是MD模式编译的，即动态链接的编译方式，编译出来的动态库会动态链接一些VS的库  
按照示例步骤编译出来的库都是MD模式，想要编译不依赖vs的库，则需要手动修改makefile文件  

将perl Configure debug-VC-WIN32 生成的makefile文件中2处MDd手动修改成MTd之后再进行nmake

#### vs调试
按照以上步骤编译出来的库，使用对应的vs版本可进行调试，进入openssl的源码中
