
openssl
gmssl
tassl
gnustl
mbedtls
bouncycastlepp
libtomcrypt
matrixssl-4-2-1

botan

mkcert


#### 下载openssl-1.1.1g源码
https://www.openssl.org/source/openssl-1.1.1g.tar.gz

#### 打开并使用 Visual Studio 2017 Command Prompt 命令行

#### 配置参数
32位和64位的release和debug版本  
VC-WIN32  debug-VC-WIN32  
VC-WIN64A debug-VC-WIN64A  
动态库和静态库  
shared  动态库  
no-shared 静态库  

#### 配置组合
perl Configure VC-WIN32  shared  32位release动态库  
perl Configure VC-WIN32  no-shared 32位release静态库  
perl Configure debug-VC-WIN32  shared 32位debug动态库 
perl Configure debug-VC-WIN32  no-shared 32位debug动态库  

#### 示例 
编译可调式debug版本32位动态库 步骤  
1.perl Configure debug-VC-WIN32  shared no-asm --prefix="D:/Git/SSL/openssl-1.1.1g/win32-release" --openssldir="D:/Git/SSL/openssl-1.1.1g/win32-release/ssl"  

2.nmake  
3.nmake  install  
4.nmake test  

nmake时编译提示错误：NMAKE : fatal error U1077: “rc”: 返回代码“0x1”  
分析原因：  
将Rc.exe路径添加到系统环境变量PATH，我的路径是：  
C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin  
C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin\x64  


#### MT 和 MD
常规windosw下提供的库都是MD模式编译的，即动态链接的编译方式，编译出来的动态库会动态链接一些VS的库  
按照示例步骤编译出来的库都是MD模式，想要编译不依赖vs的库，则需要手动修改makefile文件  

将perl Configure debug-VC-WIN32 生成的makefile文件中2处MDd手动修改成MTd之后再进行nmake

#### vs调试
按照以上步骤编译出来的库，使用对应的vs版本可进行调试，进入openssl的源码中



编译GMSSL
步骤同上
链接的时候可能缺少EVP_get_ciphernames接口的实现，在旧的提交中找到以下实现,修改后可编译成功
```cpp
GmSSL\crypto\evp\names2.c


#include <stdio.h>
#include "internal/cryptlib.h"
#include <openssl/evp.h>
#include <internal/objects.h>
#include <openssl/x509.h>
#include "internal/evp_int.h"

const EVP_CIPHER *EVP_get_default_cipher(void)
{
	return NULL;
}

/*
 * use MD5 as default:
 *	X509_REQ_to_X509		x509_r2x.c
 *	X509_issuer_and_serial_hash	x509_cmp.c
 *	X509_NAME_hash_old		x509_cmp.c
 *	PEM_ASN1_write_bio		pem_lib.c
 */
const EVP_MD *EVP_get_default_digest(void)
{
#if !defined(OPENSSL_NO_MD5)
	return EVP_md5();
#elif !defined(OPENSSL_NO_SHA)
	return EVP_sha1();
#elif !defined(OPENSSL_NO_SM3)
	return EVP_sm3();
#elif !defined(OPENSSL_NO_RIPEMD)
	return EVP_rmd160();
#else
	return NULL;
#endif
}

static void cipher_name_len(const EVP_CIPHER *cipher, const char *from,
	const char *to, void *x)
{
	*((int *)x) += strlen(EVP_CIPHER_name(cipher));
}

static void cipher_name(const EVP_CIPHER *cipher, const char *from,
	const char *to, void *x)
{
	strcat((char *)x, EVP_CIPHER_name(cipher));
}

char *EVP_get_ciphernames(int aliases)
{
	char *ret = NULL;
	int len = 0;
	EVP_CIPHER_do_all_sorted(cipher_name_len, &len);

	ret = OPENSSL_zalloc(len);
	if (!ret) {
		return NULL;
	}

	EVP_CIPHER_do_all_sorted(cipher_name, ret);
	return ret;
}

char *EVP_get_digestnames(int aliases)
{
	return "sm3:sha1:sha256";
}

```