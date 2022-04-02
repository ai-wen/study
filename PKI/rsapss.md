

/*pss 模式签名********************************************************
								 +-----------+
								 |     M     |
								 +-----------+
									   |
									   V
									 Hash
									   |
									   V
						 +--------+----------+----------+
					M' = |Padding1|  mHash   |   salt   |
						 +--------+----------+----------+
										|
			  +--------+----------+     V
		DB =  |Padding2|   salt   |   Hash
			  +--------+----------+     |
						|               |
						V               |
					   xor <--- MGF <---|
						|               |
						|               |
						V               V
			  +-------------------+----------+--+
		EM =  |    maskedDB       |     H    |bc|
			  +-------------------+----------+--+

若要对数据M进行签名
步骤：
1：计算 mHash = Hash(M)，其长度为hashlen
2：计算slen长度的随机值salt，这里slen取hashlen。
3：计算 M' = Hash(00 00 00 00 00 00 00 00 || mHash || salt)
4：计算 DB = PS || 0x01 || salt，PS是字节0。PS的长度是nLen - 2*hashlen - 2 。然后
到这里，待签名数据的内存布局如下

ptr = 00 00 00 ... 00 00 || 0x01 || salt || M' || 0xBC
看上面这种内存布局，也就好理解为什么PS的长度是 nLen - 2*hashlen - 2 了。

5：进行MGF运算

5-1：计算hash值mask
unsigned char counter[4] = {0};
mask = Hash(M' || counter);
counter++；

5-2：
ptr ^= mask;
ptr += hashlen;

5-3：
执行5-1，5-2，直到 ptr 中的 salt 也被 mask 亦或运算 为止。

最后的结果是 EM。
EM = maskDB || M' || 0xBC。
换句话说，不考虑counter以及hash函数，maskDB, 是由M’亦或得到的。

6：执行 EM^d mod n 得到签名结果C。

**********************************************************/



#include <time.h>
void getRandom(unsigned char* out, unsigned int Len)
{
	srand(time(NULL));
	for (unsigned int i = 0; i < Len; i++)
		out[i] = rand() % 0xFF;
}

void doHash(unsigned int hType, const unsigned char* src, unsigned int srcLen, unsigned char* des, unsigned int& hash_len)
{
	if (NULL == src || 0 == srcLen || NULL == des)
	{
		hash_len = 0;
		return;
	}
	switch (hType)
	{
	case CIPHER_ID_MD5:
		hash_len = 16;
		break;
	case CIPHER_ID_SHA1:
		hash_len = 20;
		break;
	case CIPHER_ID_SHA224:
		hash_len = 28;
		sha2(src, srcLen, des, 1);
		break;
	case CIPHER_ID_SHA256:
		hash_len = 32;
		sha2(src, srcLen, des, 0);
		break;
	case CIPHER_ID_SHA384:
		hash_len = 48;
		break;
	case CIPHER_ID_SHA512:
		hash_len = 64;
		break;
	default:
		break;
	}

}

static int PKCS1_MGF1(unsigned int hType, unsigned char* DB, unsigned int dblen,
	const unsigned char* seed, unsigned int mdlen)
{
	unsigned int i, outlen = 0;
	unsigned char cnt[4];
	unsigned char md[64];
	unsigned char tmp[64 + 4] = { 0 };

	for (i = 0; outlen < dblen; i++)
	{
		cnt[0] = (unsigned char)((i >> 24) & 255);
		cnt[1] = (unsigned char)((i >> 16) & 255);
		cnt[2] = (unsigned char)((i >> 8)) & 255;
		cnt[3] = (unsigned char)(i & 255);

		memcpy(tmp, seed, mdlen);
		memcpy(tmp + mdlen, cnt, 4);

		if (outlen + mdlen <= dblen)
		{
			doHash(hType, tmp, mdlen + 4, DB + outlen, mdlen);
			outlen += mdlen;
		}
		else
		{
			doHash(hType, tmp, mdlen + 4, md, mdlen);
			memcpy(DB + outlen, md, dblen - outlen);
			outlen = dblen;
		}
	}

	return 0;
}


static int doPSS(unsigned int hType, unsigned int rsamodulLen,
	const unsigned char* src, unsigned int srcLen, unsigned char* EM)
{
	static unsigned char padding[256] = { 0 };

	int i;
	int ret = 0;
	int maskedDBLen, MSBits, emLen;
	unsigned char* H, salt[64] = { 0 }, * p;
	unsigned char mHash[64] = { 0 };
	unsigned int hashLen = 0;

	/*
	1、生成消息M 的Hash值，mHash = Hash(M)
	2、生成伪随机字节串作为盐，得到M’ = padding1 || mHash || salt 的数据块  =  00 00 00 00 00 00 00 00 || mHash || salt
	3、生成M’ 的Hash值，H = Hash(M’)
	4、构造数据块DB = padding2 || salt										 = 00 00 00 ... 00 00 || 0x01 || salt
	5、计算H 的MGF值：dbMask = MGF(H, emLen - hLen - 1)
	计算maskedDB = DB xor dbMask
	将maskedDB 的最左8emLen - emBits设为0
	EM = maskedDB || H || BC												 = 00 00 00 ... 00 00 || 0x01 || salt || M' || 0xBC
	*/

	doHash(hType, src, srcLen, mHash, hashLen);

	MSBits = (rsamodulLen - 1) & 0x7;
	emLen = rsamodulLen / 8;
	if (MSBits == 0)
	{
		*EM++ = 0;
		emLen--;
	}

	if (emLen < (hashLen + hashLen + 2))
	{
		goto err;
	}

	if (hashLen > 0)
	{
		getRandom(salt, hashLen);
	}

	maskedDBLen = emLen - hashLen - 1;
	H = EM + maskedDBLen;

	memset(padding, 0, 8);
	memcpy(padding + 8, mHash, hashLen);
	memcpy(padding + 8 + hashLen, salt, hashLen);

	doHash(hType, padding, 8 + hashLen + hashLen, H, hashLen);

	/* Generate dbMask in place then perform XOR on it */
	if (PKCS1_MGF1(hType, EM, maskedDBLen, H, hashLen))
		goto err;

	/*
	 * Initial PS XORs with all zeroes which is a NOP so just update pointer.
	 * Note from a test above this value is guaranteed to be non-negative.
	 */
	p = EM;
	p += emLen - hashLen - hashLen - 2;
	*p++ ^= 0x1;
	if (hashLen > 0)
	{
		for (i = 0; i < hashLen; i++)
			*p++ ^= salt[i];
	}
	if (MSBits)
		EM[0] &= 0xFF >> (8 - MSBits);

	/* H is already in place so just set final 0xbc */

	EM[emLen - 1] = 0xbc;

	ret = 1;

err:

	return ret;

}