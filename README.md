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

## Please type ai-wen/vscode-eslint to confirm.
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