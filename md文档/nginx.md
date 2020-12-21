nginx -v
nginx version: nginx/1.10.3 (Ubuntu)

#------------------------------------------
/etc/nginx/nginx.conf

http {
        ...
        # 1、nginx总配置文件搜寻下列路径下的服务配置
        include /etc/nginx/conf.d/*.conf;
        include /etc/nginx/sites-enabled/*;
}


#1、/etc/nginx/sites-enabled/------------------------------------------
ll /etc/nginx/sites-enabled/
default -> /etc/nginx/sites-available/default
cat /etc/nginx/sites-available/default
server {
        listen 80 default_server;
        listen [::]:80 default_server;
        root /var/www/html;
        index index.html index.htm index.nginx-debian.html;
        server_name _;
        location / {
                # First attempt to serve request as file, then
                # as directory, then fall back to displaying a 404.
                try_files $uri $uri/ =404;
        }
}


#2、/etc/nginx/conf.d/------------------------------------------
#此路径可放置用户自定义的配置
#eg: 自定义一个静态文件服务器 8080
cat /etc/nginx/conf.d/lm.conf
server {
    client_max_body_size 4G;
    listen       8080; #需要进入阿里云添加 安全组规则 放开8080  ufw  allow 8080/tcp
    server_name  0.0.0.0;

    location / {
        root   /home/tool;   #目录文件服务器根目录 #不配置下列项root目录,会自动指向索引文件
        #auth_basic   "Longmai etc";   #表示的输入密码时的提示语
        #auth_basic_user_file /etc/nginx/pass_file; #认证时的用户密码文件存放路径
        autoindex on;   #显示索引
        autoindex_exact_size off;  #显示文件大小
        autoindex_localtime on;    #显示文件时间
        index index.html index.htm;
    }

    # ^~ /conf 文件夹及文件夹下的所有文件都需要进行验证
    # /conf 仅仅能对文件夹进行验证直接訪问其下的文件。
    # 对子目录进行认证时，必须要保证子目录的访问域名没有歧异，上述root 与下面 alias 配置保证了二级域名总是 127.0.0.1/conf    
    # 如果将网络域名节点设置为/COS 则子目录的访问域名有2个   127.0.0.1/conf   127.0.0.1/COS, 通过127.0.0.1/conf访问时无需认证
    location ^~ /conf {  #设置网络域名节点
        alias /home/tool/conf;  #设置节点对应的绝对路径 只有一级域名 /节点才能用 root， 子域名节点需要用别名  alias                           
        auth_basic   "Longmai etc";   #表示的输入密码时的提示语 
        auth_basic_user_file /etc/nginx/pass_file; #认证时的用户密码文件存放路径
        autoindex on;   #显示索引
        autoindex_exact_size off;  #显示文件大小
        autoindex_localtime on;    #显示文件时间
        index index.html index.htm;
    }

    error_page   500 502 503 504  /50x.html;
    location = /50x.html {
        root   /usr/share/nginx/html;
    }
}

cat /etc/nginx/pass_file
lkl:XXXXXXXX

而且 password必须使用函数 crypt(3) 加密
官方档说 能够用 Apache 的 htpasswd 工具来创建password文件
htpasswd -c /opt/nginxpwd lkl

nginx -t 测试配置文件语法


需要改变root路径的读写权限
drwx------   8 root root  4096 Dec 20 19:51 root/
尽量不要放在root 路径下
drwxr-xr-x  3 root root 4096 Dec 20 19:51 www/



```shell nginx

#安装 nginx 
if [ "$( which nginx )" = "" ];then
    apt-get install nginx 
    nginx -t
fi 

#删除无用配置文件
#/etc/nginx/nginx.conf
if [ -d /etc/nginx/conf.d ]
then
    rm /etc/nginx/conf.d/*.conf  -rf
fi
if [ -d /etc/nginx/sites-enabled ]
then
    rm /etc/nginx/sites-enabled/* -rf
fi

#配置文件服务器
HTTPCONFIG=/etc/nginx/conf.d/lm.conf

if [ ! -f /etc/nginx/conf.d/lm.conf ]
then
    mkdir -p /etc/nginx/conf.d
    touch $HTTPCONFIG
fi

#下列配置简单说：http://127.0.0.1/ 对应的路径为 /home/tool 在这个配置中不需要用户登录直接访问
ROOTPATH="        root   /home/tool;\n"

#下列配置简单说：http://127.0.0.1/cos 对应的路径为 /home/tool/cos  在这个配置中需要用户登录才能访问
COSWEBSITE="    location ^~ /cos {\n"
COSPATH="        alias /home/tool/cos;\n"


echo "server {" >$HTTPCONFIG
echo "    client_max_body_size 4G;" >>$HTTPCONFIG
echo "    listen       80;" >>$HTTPCONFIG
echo "    server_name  0.0.0.0;" >>$HTTPCONFIG

echo "    location / {" >>$HTTPCONFIG
#echo "        root   /home/tool;" >>$HTTPCONFIG
echo $ROOTPATH >>$HTTPCONFIG
echo "        autoindex on;" >>$HTTPCONFIG
echo "        autoindex_exact_size off;" >>$HTTPCONFIG
echo "        autoindex_localtime on;" >>$HTTPCONFIG
echo "        index index.html index.htm;" >>$HTTPCONFIG
echo "    }" >>$HTTPCONFIG
echo "" >>$HTTPCONFIG
    
#echo "    location ^~ /conf {" >>$HTTPCONFIG
echo $COSWEBSITE >>$HTTPCONFIG
#echo "        alias /home/tool/conf;" >>$HTTPCONFIG
echo $COSPATH >>$HTTPCONFIG
echo "        auth_basic   Longmai_etc;" >>$HTTPCONFIG
echo "        auth_basic_user_file /etc/nginx/pass_file;" >>$HTTPCONFIG
echo "        autoindex on;" >>$HTTPCONFIG
echo "        autoindex_exact_size off;" >>$HTTPCONFIG
echo "        autoindex_localtime on;" >>$HTTPCONFIG
echo "        index index.html index.htm;" >>$HTTPCONFIG
echo "    }" >>$HTTPCONFIG
echo "" >>$HTTPCONFIG

echo "    error_page   500 502 503 504  /50x.html;" >>$HTTPCONFIG
echo "    location = /50x.html {" >>$HTTPCONFIG
echo "        root   /usr/share/nginx/html;" >>$HTTPCONFIG
echo "    }" >>$HTTPCONFIG
echo "}" >>$HTTPCONFIG



#配置账号 
if [ ! -f /etc/nginx/pass_file ]
then
    mkdir -p /etc/nginx
    touch /etc/nginx/pass_file
fi
echo -e "lkl:XXXXXXXX" > /etc/nginx/pass_file

#测试并重启 nginx
nginx -t
service nginx restart

```