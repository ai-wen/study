1、下载插件 Remote Development   
2、设置SSH中的ShowLoginTerminal   
![alt 2、设置SSH中的ShowLoginTerminal](https://github.com/ai-wen/study/blob/master/vscode_c++ssh/set.png)   
3、配置你的Linux服务器地址信息  
CTRL+SHIFT+P,搜索ssh，找到Open Configuration File     
4、Linux安装OpenSSH-Server和配置   
ubuntu  
sudo apt-get remove openssh-server// 先卸载  
sudo apt-get install openssh-server// 安装  
sudo service ssh --full-restart//重启sshd服务  
sudo systemctl enable ssh//自动启动  

centos   
yum install -y openssl openssh-server//安装  
systemctl restart sshd.service//重启sshd服务  
systemctl enable sshd//自动启动  
5、配置sshd_config  
sudo cp /etc/ssh/sshd_config /etc/ssh/sshd_config.backup  
Port 22  
PermitRootLogin yes   
RSAAuthentication yes  
PubKeyAUthentication yes  
PasswordAuthentications yes   
6、使用密钥免密码登录  
ssh-keygen -t rsa -b 4096 


