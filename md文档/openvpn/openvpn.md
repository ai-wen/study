此版本的openvpn客户端安装包自带安装 tap0801 版本的虚拟网卡

OpenVPN for win32安装后，会自动安装成系统服务，并安装一个虚拟网卡：
配置文件：C:\Program Files\OpenVPN\config
日志文件：C:\Program Files\OpenVPN\log
Windows版的OpenVPN使用的配置文件后缀为.ovpn，类似Linux下的.conf。
参考client.ovpn

※同样的就不说了，不同的有两个：
1、dev-node ，如果你有两个以上的OpenVPN虚拟网卡，在配置对应的网卡名称，以识别；
2、ca.crt等的路径，Win下的路径必须用两个号分割，并且用双引号括起来；


若需要同时连接多个OpenVPN网络，可以用下面的命令添加多个网卡：
To install:
"C:\Program Files\OpenVPN\bin\tapinstall.exe" install "C:\Program Files\OpenVPN\driver\OemWin2k.inf" TAP0801

To update:
"C:\Program Files\OpenVPN\bin\tapinstall.exe" update "C:\Program Files\OpenVPN\driver\OemWin2k.inf" TAP0801

To uninstall:
"C:\Program Files\OpenVPN\bin\tapinstall.exe" remove TAP0801

※OemWin2k.inf 在“C:\Program Files\OpenVPN\driver”中。

※OemWin2k表示是win2000及xp系统使用的，不支持win7及以上系统