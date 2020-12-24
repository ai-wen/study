[参考](https://www.cnblogs.com/loveyou/p/6989064.html)
[阿里云身份宝 or Google Authenticator ，OTP demo](https://hub.fastgit.org/suyin58/otp-demo)(https://www.cnblogs.com/loveyou/p/6989064.html)
[OpenVPN OTP Authentication support](https://hub.fastgit.org/evgeny-gridasov/openvpn-otp)

[mod-authn-otp](https://hub.fastgit.org/archiecobbs/mod-authn-otp)

[参考](https://blog.csdn.net/weixin_37569048/article/details/80257368)


# OTP 是 One-Time Password的简写，表示一次性密码，常见说法 动态口令。

# RFC参考
[RFC 4226 One-Time Password and HMAC-based One-Time Password.](https://tools.ietf.org/html/rfc4226)
[RFC 6238 Time-based One-Time Password.](https://tools.ietf.org/html/rfc6238)
[RFC 2104 HMAC Keyed-Hashing for Message Authentication.](https://tools.ietf.org/html/rfc2104)

# otp从技术来分有三种形式，时间同步、事件同步、挑战/应答。
## TOTP (RFC 6238)
TOTP 是Time-based One-Time Password的简写，表示基于时间戳算法的一次性密码。 
TOTP 是时间同步，基于客户端的动态口令和动态口令验证服务器的时间比对，一般每60秒产生一个新口令，要求客户端和服务器能够十分精确的保持正确的时钟，客户端和服务端基于时间计算的动态口令才能一致。每天可以产生43200个密码。

## HOTP  (RFC 4226)
HOTP 是HMAC-based One-Time Password的简写，表示基于HMAC算法加密的一次性密码。
HOTP 是事件同步，通过某一特定的事件次序及相同的种子值作为输入，通过HASH算法运算出一致的密码。

## 挑战/应答
常用于的网上业务，在网站/应答上输入服务端下发的挑战码，动态令牌输入该挑战码，通过内置的算法上生成一个6/8位的随机数字，口令一次有效，这种技术目前应用最为普遍，包括刮刮卡、短信密码、动态令牌也有挑战/应答形式。

主流的动态令牌技术是时间同步和挑战/应答两种形式。
OTP生成终端主流的有短信密码、动态令牌从终端来分类包含硬件令牌和手机令牌两种，手机令牌是安装在手机上的客户端软件。
（ 1 ）每个认证令牌（ Token ）预编程了一个唯一数字，称为随机种子（ random seed ）或种子（ seed ）。令牌上显示的用于登录的密码是根据这个种子和设定好的口令生成程序得到的，生成的口令是一次性的，生成之后，使用一次就放弃一次。
（ 2 ）用户名和口令在登录请求中传递到服务器，服务器取得用户数据库中与用户名相应的种子，然后调用口令验证程序进行验证，这个验证程序使用同步技术像认证令牌一样生成一次性口令。

 1 ）挑战 / 响应令牌
用户在发送登录请求的时候，先只输入用户 ID 。服务器收到请求，检查这个用户名是否有效，若有效，则生成一个随机挑战数，并反馈给用户，否则，服务器返回登录错误信息。用户将服务器返回的挑战数，输入令牌（这种类型的令牌需要要求有输入键），令牌接受随机挑战，并用种子值加密，并把加密的结果显示在令牌上。用户将这个读取的令牌结果发送给认证服务器。服务器要验证从用户收到的加密随机挑战，可以使用两种方法：用用户的种子值解密从用户那里收到的加密随机挑战，如果解密结果与服务器上原先的随机挑战一致，则验证成功；也可以用用户的种子加密发给用户的随机挑战，如果加密得到的随机挑战跟从用户收到的加密随机挑战一致，则验证成功。
 2 ）基于时间令牌
这种类型的令牌不需要输入键，服务器也不必向用户发送任何随机挑战，令牌只要用时间和种子作为认证过程的输入变量，代替随机挑战，进行某种加密处理，自动产生口令，然后在令牌的 LCD 上显示。
用户需要登录时，只要根据 LCD 的显示，读取口令信息，发送给验证服务器即可。
服务器接收口令，并对用户种子值和当前系统时间独立执行加密功能，生成自己的口令，如果两个口令一致，则验证成功。

# 原理 计算OTP串的公式
```cpp
OTP(K,C) = Truncate(HMAC-SHA-1(K,C))

K表示秘钥串；
C是一个数字，表示随机数；
HMAC-SHA-1表示使用SHA-1做HMAC；
Truncate是一个函数，就是怎么截取加密后的串，并取加密后串的哪些字段组成一个数字。

对HMAC-SHA-1方式加密来说，Truncate实现如下。
HMAC-SHA-1加密后的长度得到一个20字节的密串；
取这个20字节的密串的最后一个字节的低4位，作为截取加密串的下标偏移量；
按照下标偏移量开始，按照大端方式组成一个4个字节整数；
截取这个整数的后6位或者8位转成字符串返回。
```
```cpp
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    HMAC_CTX* hmac = HMAC_CTX_new();
    HMAC_CTX_reset(hmac);
    HMAC_Init_ex(hmac, otp_key, key_len, otp_digest, NULL);
    HMAC_Update(hmac, (uint8_t *)&Tn, sizeof(Tn));
    HMAC_Final(hmac, mac, &maclen);
    HMAC_CTX_free(hmac);
#else
    HMAC_CTX hmac;
    HMAC_CTX_init(&hmac);
    HMAC_Init(&hmac, otp_key, key_len, otp_digest);
    HMAC_Update(&hmac, (uint8_t *)&Tn, sizeof(Tn));
    HMAC_Final(&hmac, mac, &maclen);
#endif

static int tdigits = 6;//后6位或者8位转成字符串返回
uint32_t divisor = 1;
for (i = 0; i < tdigits; ++i) 
    divisor *= 10;

#大端 起点高字节，小端 起点低字节
const uint8_t *otp_bytes = mac + (mac[maclen - 1] & 0x0f); //最后一个字节的低4位的起始位置
uint32_t otp = ((otp_bytes[0] & 0x7f) << 24) | (otp_bytes[1] << 16) | (otp_bytes[2] << 8)  |  otp_bytes[3]; //按大端方式组成一个，4个字节整数
otp %= divisor;

```

```cpp HOTP
知道了OTP的基本原理，HOTP只是将其中的参数C变成了随机数
HOTP(K,C) = Truncate(HMAC-SHA-1(K,C))
HOTP： Generates the OTP for the given count
即：C作为一个参数，获取动态密码。
一般规定HOTP的散列函数使用SHA2，即：基于SHA-256 or SHA-512 [SHA2] 的散列函数做事件同步验证；
```

```cpp TOTP
TOTP只是将其中的参数C变成了由时间戳产生的数字。

TOTP(K,C) = HOTP(K,C) = Truncate(HMAC-SHA-1(K,C))
不同点是TOTP中的C是时间戳计算得出。
C = (T - T0) / X;
T 表示当前Unix时间戳


T0一般取值为 0.
X 表示时间步数，也就是说多长时间产生一个动态密码，这个时间间隔就是时间步数X，系统默认是30秒；

例如:
T0 = 0;
X = 30;
T = 30 ~ 59, C = 1; 表示30 ~ 59 这30秒内的动态密码一致。
T = 60 ~ 89, C = 2; 表示30 ~ 59 这30秒内的动态密码一致。


不同厂家使用的时间步数不同；
阿里巴巴的身份宝使用的时间步数是60秒；
宁盾令牌使用的时间步数是60秒；
Google的 身份验证器的时间步数是30秒；
腾讯的Token时间步数是60秒；

TOTP 的实现可以使用HMAC-SHA-256或者HMAC-SHA-512散列函数；
```

# OTP系统的构架：
动态口令认证系统由动态口令认证服务器集群、动态口令令牌以及动态口令管理服务站点组成。


# 开源OTP
Google Authenticator工具很好的实现了OTP移动端APP的使用与密钥管理
[(TOTP)golang实现google authencatior服务器RestAPI（密钥生成、OTP生成、OTP认证 ）](https://github.com/gotoolkits/AuthOTP)
[python的otp实现](https://pypi.python.org/pypi/pyotp https://github.com/pyotp/pyotp)
[Another implementation of RFC4226(HOTP) and RFC6238(TOTP).](https://github.com/gitchs/gootp)





# 使用场景
服务器登录动态密码验证（如阿里云ECS登录，腾讯机房服务器登录等）；
公司VPN登录双因素验证；
网络接入radius动态密码；
银行转账动态密码；
网银、网络游戏的实体动态口令牌；
等动态密码验证的应用场景。