快速 MD5 碰撞生成器（王小云改进版） 
作者： 马臣云2008-07-02 13:21分类：默认分类标签：
假如您现在仍然相信通过 MD5 验证来判断文件的完整性，那您不得不面对现实。不同内容不同功能的可执行文件是否可以伪造MD5码，一直是密码界一直关注的问题，近日，几名国外研究人员通过采用自行编写的“MD5 碰撞生成器”，成功伪造了MD5验证过程。
　　http://www.win.tue.nl/hashclash/SoftIntCodeSign/HelloWorld-colliding.exe

　　http://www.win.tue.nl/hashclash/SoftIntCodeSign/GoodbyeWorld-colliding.exe

　　这两个程序会在屏幕上打印出不同的字符，但是它们的 MD5 都是一样的。

　　近日，这几位密码学家使用的是“构造前缀碰撞法”(chosen-prefix collisions)来进行此次攻击(是王小云所使用的攻击方法的改进版本)。

　　他们所使用的计算机是一台 Sony PS3，且仅用了不到两天。

　　研究结论声称：MD5 算法不应再被用于任何软件完整性检查或代码签名的用途。

　　研究人员将编写的“快速 MD5 碰撞生成器”，放到网上供用户参考http://www.win.tue.nl/hashclash/fastcoll_v1.0.0.5.exe.zip，如果仅仅是想要生成 MD5 相同而内容不同的文件的话，在任何主流配置的电脑上用几秒钟就可以完成了。

　　“快速 MD5 碰撞生成器”源代码：http://www.win.tue.nl/hashclash/fastcoll_v1.0.0.5_source.zip

以下是我使用该工具在1秒内产生的2个文件，他们的md5 哈希值相等。
http://5233studio.bokee.com/inc/msg1.jpg
http://5233studio.bokee.com/inc/msg2.jpg
2个文件的哈希值都是： 3e 7b b2 84 4e d9 25 70 45 62 e3 89 c1 40 fe 0f