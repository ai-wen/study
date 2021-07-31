
tap0801 是n2n 20201230编译时源码依赖的版本
但tap0801 应该只支持win2k 以下版本的windows 系统。

此处使用9.21.2版本的tab虚拟网卡。

注意：
1、tap需要对应系统位数，安装后会增加一个网卡适配器
2、edge32.exe可以在所有位数的系统下使用
3、edge使用的配置文件，不需要指定 网卡名
```cpp
#windows上无须指定网卡名
# -d=n2n0
-c=longmai
-k=lmtest
-a=192.168.100.3
-l=47.94.149.220:5555
```

安装方式1：使用tap-windows-9.21.2目录下驱动安装
管理员权限运行CMD（或者PowerShell）
tapinstall.exe install OemVista.inf tap0901     没执行一次安装一个新网卡
tapinstall.exe update OemVista.inf tap0901 
tapinstall.exe remove tap0901               移除所有tap网卡

安装方式2：使用tap-windows-9.21.2.exe安装包安装

安装方式3：使用windows驱动安装向导安装
Win+R 运行 hdwwiz.exe

