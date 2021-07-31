# 安装Oracle JDK
add-apt-repository ppa:webupd8team/java
apt update
apt-get install oracle-java9-installer

在一个服务器上可以有多个Java安装。 
您可以配置版本是通过使用命令行使用默认的update-alternatives ，它管理这些符号链接用于不同的命令。
update-alternatives --config java
```
There are 5 choices for the alternative java (providing /usr/bin/java).

  Selection    Path                                            Priority   Status
------------------------------------------------------------
* 0            /usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java   1081      auto mode
  1            /usr/lib/jvm/java-6-oracle/jre/bin/java          1         manual mode
  2            /usr/lib/jvm/java-7-oracle/jre/bin/java          2         manual mode
  3            /usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java   1081      manual mode
  4            /usr/lib/jvm/java-8-oracle/jre/bin/java          3         manual mode
  5            /usr/lib/jvm/java-9-oracle/bin/java              4         manual mode

Press <enter> to keep the current choice[*], or type selection number:
```

/etc/environment
JAVA_HOME="/usr/lib/jvm/java-8-oracle"
source /etc/environment


安装默认JRE / JDK
apt-get install default-jre
apt-get install default-jdk

java -version




apt-get install tomcat8 tomcat8-docs tomcat8-examples tomcat8-admin
service tomcat8 start
service tomcat8 status
service tomcat8 stop

/var/lib/tomcat8/conf/server.xml
修改8080端口

/var/lib/tomcat8/webapps/ROOT
放置工程

/var/lib/tomcat8/conf/tomcat-users.xml
<role rolename="manager-gui"/>
<role rolename="admin-gui"/>
<user username="root" password="123456" roles="manager-gui,admin-gui"/>