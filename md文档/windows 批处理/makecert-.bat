:::@echo off
:::C:\Program Files (x86)\Windows Kits\8.1\bin\x64
:::makecert -r -pe -n "CN=LM CA" -$ commercial -a sha1 -b 01/01/2020 -e 01/01/2100 -cy authority -ss root -sr currentuser
:::"C:\Program Files (x86)\Windows Kits\8.1\bin\x64\makecert.exe" -r -pe -n "CN=LM CA" -$ commercial -a sha256 -b 01/01/2020 -e 01/01/2100 -cy authority -ss root -sr currentuser

:::-r ����һ����ǩ��֤�顣
:::-pe ��˽Կ���Ϊ�ɵ�����
:::-n �����ơ� ������֤������ơ������Ʊ������X.500��׼����򵥵ķ�����ʹ�á� CN = MyName ����ʽ�����磺-n�� CN = Test����
:::-$ ֤��䷢���� ��֤���������͡����뽫CertificateAuthority����Ϊcommercial��ҵ�ģ�����Ҫ����ҵ���������ʹ�õ�֤�飩��individual���˵ģ�����Ҫ�ɸ������������ʹ�õ�֤�飩��
:::-a һ�� �㷨 ��ϣ�㷨����������ΪSHA-1��MD5��Ĭ��ΪMD5����
:::-b ���ڿ�ʼ ֤���״���Ч�����ڡ�Ĭ��ֵΪ����֤���ʱ�䡣DateStart�ĸ�ʽΪmm/dd /yyyy��
:::-e ���ڽ��� ��Ч�ڽ��������ڡ�Ĭ��ֵΪ2039�ꡣ
:::-cy ֤������ ֤�����CertificateTypes����end�������ն�ʵ�壬��authorityȨ��������Ȩ����֤��
:::-ss ����֤��洢������ƣ��������д洢���ɵ�֤�顣
:::-sr �����֤����ע���λ�á�������LocalMachine��ע�����HKEY_LOCAL_MACHINE����CurrentUser��ע�����HKEY_CURRENT_USER��(Ĭ��ΪCurrentUser)��

makecert -sv RootIssuer.pvk RootIssuer.cer -r  -$ commercial  -b 01/01/2020 -e 01/01/2100  -n "CN=LM"  -cy authority -ss root -sr currentuser  -len 1024 -a sha256 
makecert -iv RootIssuer.pvk -ic RootIssuer.cer -sv Test.pvk Test.cer  -b 01/01/2020 -e 01/01/2100  -n "CN=Test" -sky signature -iky signature -len 1024 -a sha256
cert2spc Test.cer Test.spc
pvk2pfx -pi 1 -po 1 -f -pvk Test.pvk -spc Test.spc -pfx Test.pfx
:::-n "CN=��˾����, E=E-MAIL��ַ, O=��֯����, OU=��֯��λ, C=����, S=ʡ��(��),  P=�س�"

:::certmgr.msc ����pfx

pause