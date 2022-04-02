///////////////////////////////////////////////////////////////////////////////
// 文 件 名：AES.h
// 描    述：AES加密算法
// 创 建 人：Liangbofu
// 创建日期：2009-07-17
///////////////////////////////////////////////////////////////////////////////
#ifndef __AES_H
#define __AES_H

#include "apdu_datatypes.h"


// 以bit为单位的密钥长度，只能为 128，192 和 256 三种

// 加解密模式
#define AES_MODE_ECB	0				// 电子密码本模式（一般模式）
#define AES_MODE_CBC	1				// 密码分组链接模式
#define AES_MODE		AES_MODE_CBC

#define AES_ENCRYPT 1
#define AES_DECRYPT 2 

#define AES128_CBC 1
#define AES128_ECB 2 
#define AES192_CBC 3
#define AES192_ECB 4
#define AES256_CBC 5
#define AES256_ECB 6

#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
//	函数名：	AES_Init
//	描述：		初始化，在此执行扩展密钥操作。
//	输入参数：	pKey -- 原始密钥，其长度必须为 AES_KEY_LENGTH/8 字节。
//	输出参数：	无。
//	返回值：	无。
///////////////////////////////////////////////////////////////////////////////
UINT8 Aes_Init( void *pKey,UINT16 KeyLen);

//////////////////////////////////////////////////////////////////////////
//	函数名：	AES_Encrypt
//	描述：		加密数据
//	输入参数：	pPlainText	-- 明文，即需加密的数据，其长度为nDataLen字节。
//				nDataLen	-- 数据长度，以字节为单位，必须为AES_KEY_LENGTH/8的整倍数。
//				pIV			-- 初始化向量，如果使用ECB模式，可设为NULL。
//	输出参数：	pCipherText	-- 密文，即由明文加密后的数据，可以与pPlainText相同。
//	返回值：	无。
//////////////////////////////////////////////////////////////////////////
void AES_Encrypt( unsigned char *pPlainText, unsigned char *pCipherText, 
				 unsigned int nDataLen,  unsigned char *pIV,unsigned char roll,unsigned char nMode);

//////////////////////////////////////////////////////////////////////////
//	函数名：	AES_Decrypt
//	描述：		解密数据
//	输入参数：	pCipherText -- 密文，即需解密的数据，其长度为nDataLen字节。
//				nDataLen	-- 数据长度，以字节为单位，必须为AES_KEY_LENGTH/8的整倍数。
//				pIV			-- 初始化向量，如果使用ECB模式，可设为NULL。
//	输出参数：	pPlainText  -- 明文，即由密文解密后的数据，可以与pCipherText相同。
//	返回值：	无。
//////////////////////////////////////////////////////////////////////////
void AES_Decrypt(unsigned char *pPlainText,  unsigned char *pCipherText, 
				 unsigned int nDataLen,  unsigned char *pIV,unsigned char roll,unsigned char nMode);

UINT8 Aes_Run(UINT8 nType,UINT8 nMode,UINT8* pIn,UINT8* pOut,UINT16 nDataLen,UINT8* pIV);

#ifdef __cplusplus
}
#endif

#endif	// __AES_H
