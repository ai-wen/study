# study
for future

# git"食用"方式

### 配置git用户	0>
	git config --global user.email  "git邮箱"
	git config --global user.name "git账户名"
### download到目录	1>
	git clone 复制远程仓库/仓库中的项目到电脑本地
	git init git remote add origin + url
### 索引	2>
	git add 将本地项目保存至暂存区
### 本地数据库	3>
	git commit -m '说明' 提交（至本地仓库）
### 远程数据库	4>
	git push 将本地项目上传至远程仓库 （提示手动校验git账户）
### 远程数据库到本地本地数据库再checkout到目录	5>
	git pull 拉取/同步远程仓库的代码到本地