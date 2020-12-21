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