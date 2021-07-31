# Windows 下导入pfx个人证书
[参考](https://blog.csdn.net/weixin_34347651/article/details/85823467)

证书保存在 C:\Users\xxx\AppData\Roaming\Microsoft\SystemCertificates\My\Certificates
私钥保存在 C:\Users\xxx\AppData\Roaming\Microsoft\SystemCertificates\My\Keys
都是以证书或私钥的指纹命名：
在证书的属性中查看
证书:Thumbprint     b5572e68f3bee8b2829162391f3c39ede0039bb6
私钥:SubjectKey Identifier      5e5f25793924c7c4eea3f23585e1798110e7acfa

导入之后生成文件：
C:\Users\xxx\AppData\Roaming\Microsoft\SystemCertificates\My\Certificates\b5572e68f3bee8b2829162391f3c39ede0039bb6
C:\Users\xxx\AppData\Roaming\Microsoft\SystemCertificates\My\Keys\5e5f25793924c7c4eea3f23585e1798110e7acfa


## 自建证书
MakeCert -sv d:DevelopmentCA.pvk -n "CN=WebAPi CA" d:DevelopmentCA.cer -b 09/20/2016 -e 12/31/2050 -r
http add sslcert ipport=0.0.0.0:8084 certhash=‎996645BAB7169F2AFD7599A696DA2586862843C6 appid={41992502-E5D4-4794-BB01-D4A7414480CC}
还要在本地计算中的个人证书中看自创建的证书左上角是否有个小钥匙，这个钥匙也就是私钥，我们还得创建私钥才行，通过如下命令进行。
## 创建私钥
Pvk2pfx -pvk DevelopmentCA.pvk -spc DevelopmentCA.cer -pfx DevelopmentSSL.pfx -po password

MakeCert -sv d:DevelopmentCA.pvk -n "CN=WebAPi CA" d:DevelopmentCA.cer -b 09/20/2016 -e 12/31/2050 -r 中的CN修改为localhost即颁发给localhost时却可以

## 首先得将证书导入【受信任的颁发机构中】，但是还需要通过MMC将证书导入个人证书，这一点是我无法接受，我继续开始探索之旅。通过代码的形式将证书导入到MMC的【个人】证书中。 
```
var fileName = AppDomain.CurrentDomain.BaseDirectory + "DevelopmentSSL.pfx";
var cert = new X509Certificate2(fileName, "password");
var store = new X509Store(StoreName.My, StoreLocation.LocalMachine);
store.Open(OpenFlags.ReadWrite);
store.Add(cert);
store.Close();

Process p = new Process();
p.StartInfo.FileName = "netsh.exe";
p.StartInfo.Arguments = string.Format("http add sslcert ipport=0.0.0.0:8084 certhash={0} appid={1}", cert.GetCertHashString(), "{" + "41992502-E5D4-4794-BB01-D4A7414480CC"+"}");
p.StartInfo.UseShellExecute = false;
p.StartInfo.RedirectStandardOutput = true;            
p.Start();
p.Close();

```

## 第一点：证书和PKCS #12/PFX文件的不同 
X509Certificate2此类有两个属性即Public Key（公钥）和Private Key（私钥），当我们导入证书时可以是否导出该私钥，在Windows中典型的证书是以扩展名.cer结尾，当然它没有包含私钥。
下面我们可以这样导出一个证书：
var fileName = AppDomain.CurrentDomain.BaseDirectory + "DevelopmentCA.cer";
var cert = new X509Certificate2(fileName);
File.WriteAllBytes(@"d:Hello.cer", cert.Export(X509ContentType.Cert));
有时我们需要导出私钥，此时私钥及扩展名.Pfx结尾在另外一个文件，可以通过如下导出：
File.WriteAllBytes("Hello.pfx", cert.Export(X509ContentType.Pkcs12, (string)null));

## 第二点：证书存储
关于这点上述也已经演示，我们通过MMC打开的是控制台证书管理器，可以将当前用户或本地计算机账户导入其中，若只是想看当前用户证书则可以通过certmgr.msc来打开。如下：
var store = new X509Store(StoreName.My, StoreLocation.CurrentUser);
store.Open(OpenFlags.ReadWrite);
store.Add(certificate);
store.Close();
可以将证书导入到通过StoreLocation.CurrentUser映射到当前用户，通过StoreName.My映射到当前用户个人证书中，也可以是本机计算机账户中的其他机构中通过上述枚举即可。此时则会添加如下注册表中
HKEY_CURRENT_USER\SOFTWARE\Microsoft\SystemCertificates
或者通过桌面路径
C:\Users\username\AppData\Roaming\Microsoft\SystemCertificates\My\Certificates
当然在本地计算中则是如下路径：
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\SystemCertificates

## 第三点：理解私钥
我们上述已经叙述过证书中是不带私钥，私钥时单独作为一个文件来使用，那么私钥到底存储在什么地方呢？我们给出如下代码：
    var fileName = AppDomain.CurrentDomain.BaseDirectory + "DevelopmentSSL.pfx";
    var cert = new X509Certificate2(fileName,"password", X509KeyStorageFlags.MachineKeySet | X509KeyStorageFlags.PersistKeySet | X509KeyStorageFlags.PersistKeySet | X509KeyStorageFlags.Exportable);
    var store = new X509Store(StoreName.My, StoreLocation.LocalMachine);
    store.Open(OpenFlags.ReadWrite);
    store.Add(cert);
    store.Close();
此时私钥会存储在如下注册表中：
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\SystemCertificates\MY\Keys

如果我们进行如下修改将MachineKeySet修改为UserKeySet:
var fileName = AppDomain.CurrentDomain.BaseDirectory + "DevelopmentSSL.pfx";
var cert = new X509Certificate2(fileName,"password", X509KeyStorageFlags.UserKeySet | X509KeyStorageFlags.PersistKeySet | X509KeyStorageFlags.PersistKeySet | X509KeyStorageFlags.Exportable);
var store = new X509Store(StoreName.My, StoreLocation.LocalMachine);
store.Open(OpenFlags.ReadWrite);
store.Add(cert);
store.Close();
此时则会存储在如下地方：
C:\Users\username\AppData\Roaming\Microsoft\SystemCertificates\My\Keys\
此时可能会出现一点问题，当将证书导入并供本地计算机去使用，但是此时私钥却个人用户文件夹中，要是计算机中的其他账户想要访问这个私钥可能没有这个权限或者说得不到这个私钥。

关于枚举X509KeyStorageFlags的几点说明：
（1）Exportable ：创建证书时指定它可以用于备份私钥。
（2）PersistKeySet：创建证书时指定它可以导入一次并使用多次。
（3）UserKeySet：创建证书时指定它可以在另外一个账户使用它。
（4）MachineKeySet：创建证书时指定它可能导致其他账户没有权限或者访问不到私钥导致该私钥不存在。


# PowerShell
（1）利用如下命令来创建证书并获取到其指纹
 New-SelfSignedCertificate -certstorelocation cert:\localmachine\my -dnsname localhost
 颁发给localhost，并将其保存到本地计算机中的【个人】证书下，结果得到如下

（2）需要导出证书时，需要用一个变量来保存密码
 $pwd = ConvertTo-SecureString -String "Pa$$w0rd" -Force -AsPlainText
 
（3）导出pfx，指定第一步获取到的指纹和第二步保存的密码
Export-PfxCertificate -cert cert:\localMachine\my\CE0976529B02DE058C9CB2C0E64AD79DAFB18CF4 -FilePath d:cert.pfx -Password $pwd