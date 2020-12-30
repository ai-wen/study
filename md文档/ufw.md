# ubuntu 防火墙 设置###########################################################
ufw disable 关闭
ufw enable 开启
ufw allow 5555 允许5555 tcp/udp  ufw allow 5555/tcp
ufw deny 5555

ufw status verbose

ufw status 查看端口状态
  Status: active
  To                         Action      From
  --                         ------      ----
  5555/tcp                   ALLOW       Anywhere
  5555/udp                   ALLOW       Anywhere
  5555/tcp (v6)              ALLOW       Anywhere (v6)
  5555/udp (v6)              ALLOW       Anywhere (v6)

ufw delete allow 5555 按端口号删除

fw status numbered 按规则号排列显示
  Status: active
      To                         Action      From
      --                         ------      ----
  [ 1] 5555/tcp                   ALLOW IN    Anywhere
  [ 2] 5555/udp                   ALLOW IN    Anywhere
  [10] 5555/tcp (v6)              ALLOW IN    Anywhere (v6)
  [11] 5555/udp (v6)              ALLOW IN    Anywhere (v6)

ufw delete 1 按规则号删除

ufw reset 删除所有规则





#获取所有网卡名
cat /proc/net/dev | awk '{i++; if(i>2){print $1}}' | sed 's/^[\t]*//g' | sed 's/[:]*$//g'
(1) /proc/net/dev是给用户读取或更改网络适配器及统计信息的方法;
(2) awk '{i++; if(i>2){print $1}}'命令是从第二行开始循环获取第一列数据;
(3) sed 's/^[\t]*//g'命令为去除行首的空格;
(4) sed 's/[:]*$//g'命令为去除行尾的":"字符.

ifconfig | awk '{i++; if(i>0){print $1}}'


#获取正在使用的网卡名
NIC=$( cat /proc/net/dev | awk 'BEGIN {max = 0} {if ($2+0 > max+0) {max=$2 ;content=$0} } END {print $1}' | sed 's/[:]*$//g')


/etc/init.d/networking restart
ifdown enp0s3    （关闭网卡enp0s3）
ifup   enp0s3    (启动网卡enp0s3)
service network-manager restart 

静态IP设置
cat /etc/network/interfaces
auto lo
iface lo inet loopback

静态IP
auto enp0s3
iface enp0s3 inet static
address 192.168.0.110
netmask 255.255.255.0
gateway 192.168.0.1

动态IP
auto enp0s3
iface enp0s3 inet dhcp


dns配置

查看DNS /etc/resolv.conf

223.5.5.5是阿里的dns地址
在 /etc/network/interfaces 中追加 dns-nameservers 223.5.5.5 命令
在 /etc/resolvconf/resolv.conf.d/base写入下面的命令 nameserver 223.5.5.5， 然后重启，DNS生效。

reboot 


```cpp
 NIC=''
  for NICVAR in `cat /proc/net/dev | awk '{i++; if(i>2){print $1}}' | sed 's/^[\t]*//g' | sed 's/[:]*$//g'`
  do        
      IPVAR=`ifconfig $NICVAR | grep "inet " | awk -F: '{print $2}' | awk '{print $1}'`            
      if [ -z ${IPVAR} ] || [ ${IPVAR} = '127.0.0.1' ] || [ ${NICVAR} = 'n2n0' ];then
          echo ""
      else
          #echo $NICVAR
          #echo $IPVAR
          NIC=$NICVAR
      fi        
  done

  #NIC=$( cat /proc/net/dev | awk 'BEGIN {max = 0} {if ($2+0 > max+0) {max=$2 ;content=$0} } END {print $1}' | sed 's/[:]*$//g')
  IPADDR=`ifconfig $NIC | grep "inet " | awk -F: '{print $2}' | awk '{print $1}'`
  NETMASK=`ifconfig $NIC | grep "inet " | awk -F: '{print $4}' | awk '{print $1}'`
  GATEWAY=`route -n | grep $NIC | grep UG | awk '{print $2}'`
  
  if [ ${IPADDR} = '127.0.0.1' ];then
      echo "静态IP不能设置为 127.0.0.1, 请联网后再配置"
      exit 1
  fi
  if [ -z ${NIC} ];then
      echo "网卡名不能为空"
      exit 1
  fi
  if [ -z ${NETMASK} ];then
      echo "掩码不能为空"
      exit 1
  fi
  if [ -z ${GATEWAY} ];then
      echo "网关不能为空"
      exit 1
  fi

  echo "设置本机静态IP如下:"
  echo '网卡名: '$NIC
  echo '固定IP: '$IPADDR
  echo '掩码:   '$NETMASK
  echo '网关:   '$GATEWAY    


NETCONFIG=/etc/network/interfaces
if [ ! -f /etc/network/interfaces.old ]
then
    cp  $NETCONFIG /etc/network/interfaces.old
fi

if [ -f $NETCONFIG ]
then
    HAS_NIC=`grep "iface ${NIC} inet static" $NETCONFIG | wc -l`
    if [ $HAS_NIC -lt 1 ]
    then
        echo "auto ${NIC}" > $NETCONFIG
        echo "iface ${NIC} inet static" >> $NETCONFIG
        echo "address $IPADDR" >> $NETCONFIG
        echo "netmask $NETMASK" >> $NETCONFIG
        echo "gateway $GATEWAY" >> $NETCONFIG
    fi
fi

echo "----------当前文件服务器静态IP配置如下----------"
cat  $NETCONFIG
echo ""


reboot 

```