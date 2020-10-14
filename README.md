# [git备份镜像站](https://hub.fastgit.org/)

# [windbg调试命令](https://docs.microsoft.com/zh-cn/windows-hardware/drivers/debugger/getting-started-with-windbg)

# [图说设计模式](https://design-patterns.readthedocs.io/zh_CN/latest/index.html)
[git design_patterns](https://github.com/me115/design_patterns)

# [很好的密码学源码](https://www.oryx-embedded.com/products/CycloneTCP)

# host git
https://www.ipaddress.com/

140.82.113.3  github.com  
199.232.69.194 github.global.ssl.fastly.net  
140.82.113.3 codeload.github.com  

C:\Windows\System32\drivers\etc\hosts   

ipconfig /flushdns  


# 正则
^\s*\n              匹配空行   
^微信号.*\n         匹配字符串开头的1行  
^微信号.*\n.*\n     匹配字符串开头的2行  

# 编译脚本 ![编译脚本](编译脚本)
```cpp
#--retain-symbols-file控制静态符号表.symtab;
#--version-script控制动态符号表.dynsym;
#-Wl,--soname='libusb.so.1'
#-lssl -lcrypto

#动态库的命名格式是libbar.so.x.y.z，最后一个z版本的变动一定是兼容的。y版本升级一般向前兼容。所以这个y和z不能写死。x版本变动一般是不兼容升级。
#realname 	libusb.1.0.so.1.0.0		库文件真正的实体
#soname		libusb.1.0.so.1			库文件被系统加载后的标识名，是ldd查看时真正的					
#linkname	libusb.1.0.so

#readelf -d 	查看库的soname
#ldconfig -n  dir 将dir目录下的库文件缓存在系统环境变量LD_LIBRARY_PATH，并自动生成对应的soname文件
#ldconfig -l libusb.1.0.so.1.0.0				自动生成soname文件
#ln -s libusb.1.0.so.1  libusb.1.0.so		生成linkname文件


#libusb-1.0.23编译
#./configure --host=运行平台 --build=编译平台  --target=目标平台
#./configure CFLAGS="-fPIC -g" --prefix=./bin --enable-static --disable-udev --host=mips64el-linux --build=mips64el-linux --target=mips64el-linux  

#openssl-1.1.1d编译
#AES、MD5、RSA、SHA no-sm3 no-idea no-sm4 no-ec no-dsa no-sm2
./config no-asm no-aria no-async no-blake2 no-cast no-chacha no-cms no-comp no-ct no-dso no-engine no-err no-md2 no-md4 no-mdc2 no-poly1305 no-rc2 no-rc4 no-rc5 no-ripemd no-seed  no-ts no-srp no-ui  no-siphash no-bf no-camellia no-ocsp  no-cmac  no-hw no-pic no-egd no-zlib  no-sse2  no-rfc3779 no-ssl no-ssl2 no-ssl3 no-tls no-unit-test no-tests no-sock  --prefix=./../lib/linux/x64		
make build_libs
make install

```

# ![深入理解log机制](http://feihu.me/blog/2014/insight-into-log/) 
http://feihu.me/blog/2014/insight-into-log/   
# Git
 
## git"食用"方式
**0> 配置git用户**  
	git config --global user.email  "git邮箱"  
	git config --global user.name "git账户名"  
**1>DownLoad到目录**  
	git clone 复制远程仓库/仓库中的项目到电脑本地  
	git init git remote add origin + url  
**2>索引**  
	git add 将本地项目保存至暂存区  
**3>本地数据库**  
	git commit -m '说明' 提交（至本地仓库）  
**4>远程数据库**  
	git push 将本地项目上传至远程仓库 （提示手动校验git账户）  
**5>远程数据库到本地本地数据库再checkout到目录**  
	git pull 拉取/同步远程仓库的代码到本地  

## 查看更改 
git diff   可以查看本地的改动，即git status看到的文件的具体改动  
git diff commit-id1 commit-id2 --stat   这个指令可以看两个版本之间有哪些文件改动  
git diff branch1 branch2 --stat  这个指令可以看两个分支之间有哪些文件差异  
git diff tag1 tag2 --stat 这个指令可以看两个tag之间有哪些文件差异或者改动  
git log  file 可以看到一个文件的改动，以commit的形式展现   
git log -p file 可以看到具体一个文件的历史改动记录  

## 取消fork
在Github的三种操作：watch，star和fork  
watch也就是关注该repo的动态  
star则类似于Facebook、Twitter和朋友圈上的“like”和赞  
fork就是将别人的项目拷贝一份给自己，new一个新的分支（Branch）出来，你push的改进和补充则又由原作者选择是否接受。  
如果fork了一堆repo，影响到git的高效使用，该如何取消？  
方法很简单：  
1.进入已fork的repo内（注意是进入自己copy别人的repo页面，而不是原作者的页面）  
2.进入settings的options  
3.页面拉到最底，在Danger Zone直接delete this repository即可。  
Please type ai-wen/vscode-eslint to confirm.  
需要输入ai-wen/vscode-eslint才能确认删除，这一步实际上就是取消fork了。  
