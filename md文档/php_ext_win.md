# phh windows扩展开发

<?php
phpinfo();
?>

通过上述代码查看 php 的版本信息得知：
PHP Version 7.3.4

PHP API	20180731
PHP Extension	20180731
Zend Extension	320180731
Zend Extension Build	API320180731,NTS,VC15
PHP Extension Build	    API20180731,NTS,VC15


NTS no thread security
API20180731,NTS,VC15 编译的扩展库需要符合上述需求否则 扩展加载不上：
----------------------------------------------------------------
Warning: PHP Startup: mTokenOTP: Unable to initialize module
Module compiled with build ID=API20180731,TS,VC15
PHP    compiled with build ID=API20180731,NTS,VC15
These options need to match
----------------------------------------------------------------



# 扩展的编译：

进入 php-xxx-src\ext 目录下，在该目录下有一个名为 ext_skel_win32.php 的文件，在命令行中执行：
php.exe ext_skel_win32.php --extname=xxx
xxx 是扩展名，windows下一般会生成libxxx.dll的扩展
在 \ext 目录下生成了 xxx 的目录，该目录是生成出的 PHP 的扩展模板，可以在模板的基础上进行开发。

### 1、扩展工程一般需要包含php源码中下列目录下的所有头文件

├─ext
│  └─standard 
├─main
│  │  config.w32.h
│  └─streams
├─TSRM
└─Zend

### 2、编译配置

1. 在main/config.w32.h文件中加入下面的宏
win32\build\ 目录下的 config.w32.h.in 复制到 php-xx-src\main\ 目录下，并重命名为 config.w32.h
#define PHP_COMPILER_ID "VC9"   //与php的版本保持一致

2. 编译选项中编译设置 NTS 和 TS 版本宏   “配置属性” -> “C/C++” -> “预处理器”，在“预处理器定义”中点击“编辑”来添加相关预处理指令
TS版本:ZTS=1;ZEND_DEBUG=0;WIN32;NDEBUG;_WINDOWS;_USRDLL;MTOKENOTP_EXPORTS;COMPILE_DL_MTOKENOTP;ZEND_WIN32;PHP_WIN32;HAVE_MTOKENOTP=1;LIBZEND_EXPORTS
NTS版本:ZEND_DEBUG=0;WIN32;NDEBUG;_WINDOWS;_USRDLL;MTOKENOTP_EXPORTS;COMPILE_DL_MTOKENOTP;ZEND_WIN32;PHP_WIN32;HAVE_MTOKENOTP=1;LIBZEND_EXPORTS

在预处理中，HAVE_MTOKENOTP 中的 MTOKENOTP 是扩展的名称，COMPILE_DL_MTOKENOTP 中的 MTOKENOTP 同样也是扩展的名称

# 扩展的使用
将xxx.dll 扩展复制到PHP\ext 目录下。
打开PHP安装目录下的php.ini文件
在php.ini文件中“Dynamic Extensions”的部分加入
	Extension = xxx.dll

