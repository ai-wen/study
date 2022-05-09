https://github.com/rustdesk/rustdesk-server/releases
https://github.com/rustdesk/rustdesk/releases
服务端 1.1.5
客户端 1.1.8

https://rustdesk.com/docs/zh-cn/self-host/install/

hbbs - RustDesk ID注册服务器
hbbr - RustDesk 中继服务器
static - 该文件夹中包含网页控制台所有文件

./hbbs -r <hbbr运行所在主机的地址> -m <registered_email>
./hbbr -m <registered_email>
hhbs的-r参数不是必须的，他只是方便你不用在客户端指定中继服务器。客户端指定的中继服务器优先级高于这个。

hbbs.exe -k _ -m demo
hbbr.exe -k _ -m demo

hbbs.exe -r 192.168.0.110 -k _ -m demo
hbbr.exe -k _ -m demo

默认情况下，
hbbs 监听21114(tcp), 21115(tcp), 21116(tcp/udp), 21118(tcp)， 请注意21116同时要开启TCP和UDP。
hbbr 监听21117(tcp), 21119(tcp)。务必在防火墙开启这几个端口。
其中21114是网页控制台+API，
21115是hbbs用作NAT类型测试，
21116/UDP是hbbs用作ID注册与心跳服务，21116/TCP是hbbs用作TCP打洞与连接服务，
21117是hbbr用作中继服务, 21118和21119是为了支持网页客户端。
如果您不需要网页控制台+API（21114）或者网页客户端（21118，21119）支持，对应端口可以不开。

port: 21115, 21116(tcp/udp), 21117


hbbs在第一次运行时，会自动产生一对加密私钥和公钥（分别位于运行目录下的id_ed25519和id_ed25519.pub文件中），其主要用途是为了通讯加密。
禁止非加密连接，请在运行hbbs和hbbr的时候添加-k _ 参数，例如:


在 ID 服务器输入框中（被控端+主控端）输入 hbbs 主机或 ip 地址，另外两个地址可以不填，RustDesk会自动推导（如果没有特别设定），中继服务器指的是hbbr（21116）端口，API服务器指的是上面的网页控制台+API（21114）。
Key:id_ed25519.pub 服务端生成的公钥

