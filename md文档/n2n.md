
# n2n安装###########################################################

https://blog.csdn.net/qq_38232598/article/details/105262710


git clone https://hub.fastgit.org/ntop/n2n.git

cd n2n
./autogen.sh
./configure
./configure CFLAGS="-O3 -march=native"
make
# optionally install
# make install

cd packages/ubuntu
cat README
Prerequisites
-------------
apt-get install debhelper fakeroot dpkg-sig
apt-get install libzstd1
cd ..
dpkg -i n2n_2.9.0-789_amd64.deb




supernode -h
-p <port>         | Set UDP main listen port to <port>
-c <path>         | File containing the allowed communities.
-l <sn host:port> | Name/IP of a known supernode:port.
-F <fed_name>     | Name of the supernodes federation (otherwise use '*Federation' by default)
-u <UID>          | User ID (numeric) to use when privileges are dropped.
-g <GID>          | Group ID (numeric) to use when privileges are dropped.
-t <port>         | Management UDP Port (for multiple supernodes on a machine).
-a <net-net/bit>  | Subnet range for auto ip address service, e.g.
                  | -a 192.168.0.0-192.168.255.0/24, defaults to 10.128.255.0-10.255.255.0/24
-v                | Increase verbosity. Can be used multiple times.
-h                | This help message.


supernode -f -v -p 5555 -l 47.94.149.220:5555

netstat -anop |grep supernode 
udp        0      0 0.0.0.0:5555
udp        0      0 127.0.0.1:5645 

-p指定外网udp端口
-f表明前台执行方便看输出log，-v表示详细log输出，
-l指定另一supernode节点
-t  Management UDP port to bind to

/etc/n2n/supernode.conf
-l=0.0.0.0:5555
-v


systemctl enable supernode
systemctl start supernode

阿里云服务器开放此端口
ufw allow 5555  deny


edge -d n2n0 -c longmai -k lmtest -a 192.168.100.1 -f -l 47.94.149.220:5555

-d表示device，即虚拟网卡设备名(此命令将会创建虚拟网卡，命令执行完毕后，ifconfig可看到你创建的网卡信息)，可随意。
-c表示网络名，可随意。
-k 表示密码，你可以想象成等下和PC3连接时，需要的暗号。
-a 表示虚拟设备的ip地址。可随意。
-l 你要连接到哪台supernode?因为要连接到此台机器的supernode进程，故而需要指定端口。



edge can also be run as a service instead of cli:

/etc/n2n/edge.conf     See /etc/n2n/edge.conf.sample.
systemctl start edge
systemctl enable edge
You can run multiple edge service instances by creating /etc/n2n/edge-instance1.conf and starting it with sudo systemctl start edge@instance1.


On host1 run:
$ sudo edge -c mynetwork -k mysecretpass -a 192.168.100.1 -f -l supernode.ntop.org:7777
On host2 run:
$ sudo edge -c mynetwork -k mysecretpass -a 192.168.100.2 -f -l supernode.ntop.org:7777


```cpp
/etc/n2n/supernode.conf
-p=5555

/etc/n2n/edge.conf
-d=n2n0
-c=longmai
-k=lmtest
-a=192.168.100.1
-l=47.94.149.220:5555

```

```shell n2n

if [ "$( which edge )" = "" ];then
    apt-get install libzstd1
    dpkg -i n2n_2.9.0-789_amd64.deb
fi

#supernode
ufw allow 5555
if [ ! -f /etc/n2n/supernode.conf ]
then
    mkdir -p /etc/n2n
    touch /etc/n2n/supernode.conf
fi
echo "-p=5555" > /etc/n2n/supernode.conf

systemctl enable supernode
systemctl start supernode


#edge
EDGEIP="-a=192.168.100.2"
EDGECONFIG=/etc/n2n/edge.conf
if [ ! -f $EDGECONFIG ]
then
    mkdir -p /etc/n2n
    touch $EDGECONFIG
fi

echo "-d=n2n0" > $EDGECONFIG
echo "-c=longmai" >> $EDGECONFIG
echo "-k=lmtest" >> $EDGECONFIG
echo $EDGEIP >> $EDGECONFIG
echo "-l=47.94.149.220:5555" >> $EDGECONFIG

#ip配置冲突报错
#ERROR: 3E:0D:B5:2C:3A:81 is already used. Stopping the program

systemctl enable edge
systemctl restart edge

```