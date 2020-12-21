# 安装samba 服务###########################################################

1、apt-get install samba
2、 /etc/samba/smb.conf  末尾添加
  不同版本配置不同  
[sharepath]
    comment = 220
    path = /home/samb
    writeable = yes
    valid user = root

[share]
  comment = Share
  path = /root
  available = yes
  browseable = yes
  public = no
  writable = yes

  [share]
  comment = share folder
  browseable = yes
  path = /
  create mask = 0777
  directory mask = 0777
  valid users = root
  force user = nobody
  force group = nogroup
  public = yes
  available = yes

3、smbpasswd -a root
4、service smbd restart


netstat -anop | grep smbd
ufw allow 139/tcp
ufw allow 445/tcp

```
netstat -ano |findstr 445
腾讯云禁止了139和445端口
vim /etc/samba/smb.conf 
[global]
   smb ports = 5555

https://hub.fastgit.org/Arno0x/DivertTCPconn.git
divertTCPconn.exe 445 5555
```

映射网络驱动器
\\47.94.149.220\
