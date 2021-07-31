:::@echo off
:::C:\Program Files (x86)\Windows Kits\8.1\bin\x64
:::makecert -r -pe -n "CN=LM CA" -$ commercial -a sha1 -b 01/01/2020 -e 01/01/2100 -cy authority -ss root -sr currentuser
:::"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\makecert.exe" -r -pe -n "CN=LM CA" -$ commercial -a sha256 -b 01/01/2020 -e 01/01/2100 -cy authority -ss root -sr currentuser

:::-r 创建一个自签名证书。
:::-pe 将私钥标记为可导出。
:::-n “名称” 发布者证书的名称。该名称必须符合X.500标准。最简单的方法是使用“ CN = MyName ”格式。例如：-n“ CN = Test”。
:::-$ 证书颁发机构 认证机构的类型。必须将CertificateAuthority设置为commercial商业的（对于要由商业软件发行者使用的证书）或individual个人的（对于要由各个软件发行者使用的证书）。
:::-a 一种 算法 哈希算法。必须设置为SHA-1或MD5（默认为MD5）。
:::-b 日期开始 证书首次生效的日期。默认值为创建证书的时间。DateStart的格式为mm/dd /yyyy。
:::-e 日期结束 有效期结束的日期。默认值为2039年。
:::-cy 证书类型 证书类别。CertificateTypes可以end结束对终端实体，或authority权威机构的权威认证。
:::-ss 主题证书存储库的名称，将在其中存储生成的证书。
:::-sr 主题的证书库的注册表位置。必须是LocalMachine（注册表项HKEY_LOCAL_MACHINE）或CurrentUser（注册表项HKEY_CURRENT_USER）(默认为CurrentUser)。

makecert -sv RootIssuer.pvk RootIssuer.cer -r  -$ commercial  -b 01/01/2020 -e 01/01/2100  -n "CN=LM"  -cy authority -ss root -sr currentuser  -len 1024 -a sha256 
makecert -iv RootIssuer.pvk -ic RootIssuer.cer -sv Test.pvk Test.cer  -b 01/01/2020 -e 01/01/2100  -n "CN=Test" -sky signature -iky signature -len 1024 -a sha256
cert2spc Test.cer Test.spc
pvk2pfx -pi 1 -po 1 -f -pvk Test.pvk -spc Test.spc -pfx Test.pfx
:::-n "CN=公司名称, E=E-MAIL地址, O=组织名称, OU=组织单位, C=国家, S=省份(州),  P=县城"

:::certmgr.msc 导出pfx

pause