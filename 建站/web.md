https://www.dedeyun.com/muban/discuz/muban-6246.html

大家在安装使用Discuz!X3.4破解模版插件时会提示“对不起，您安装的不是正版应用，安装程序无法继续执行”
解决这个问题，请按下面的方法做：
修改文件：论坛根目录\source\function里面的function_cloudaddons.php
打开function_cloudaddons.php文件，查找代码：
X3.1及之前版本的大概在99行，X3.2的大概在97行，X3.4的大概在120行，把下面代码
cpmsg('cloudaddons_genuine_message', '', 'error', array('addonid' => $addonid));
添加注释，如下：
/*cpmsg('cloudaddons_genuine_message', '', 'error', array('addonid' => $addonid));*/
插件正版校验破解完成，现在就可以正常安装DZ所有的商业插件模板了。


经多位站长反馈，下载了其他网站提供的 function_cloudaddons.php 文件，覆盖了Discuz自带的文件，文件被植入后门，请站长注意，切勿使用非官方途径下载的discuz文件
function_cloudaddons.php被写入的后门
该后门会在论坛写入 index.bak.php、data.php 等文件，并可在任意目录写入任意后门