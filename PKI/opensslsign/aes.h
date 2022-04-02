///////////////////////////////////////////////////////////////////////////////
// �� �� ����AES.h
// ��    ����AES�����㷨
// �� �� �ˣ�Liangbofu
// �������ڣ�2009-07-17
///////////////////////////////////////////////////////////////////////////////
#ifndef __AES_H
#define __AES_H

#include "apdu_datatypes.h"


// ��bitΪ��λ����Կ���ȣ�ֻ��Ϊ 128��192 �� 256 ����

// �ӽ���ģʽ
#define AES_MODE_ECB	0				// �������뱾ģʽ��һ��ģʽ��
#define AES_MODE_CBC	1				// �����������ģʽ
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
//	��������	AES_Init
//	������		��ʼ�����ڴ�ִ����չ��Կ������
//	���������	pKey -- ԭʼ��Կ���䳤�ȱ���Ϊ AES_KEY_LENGTH/8 �ֽڡ�
//	���������	�ޡ�
//	����ֵ��	�ޡ�
///////////////////////////////////////////////////////////////////////////////
UINT8 Aes_Init( void *pKey,UINT16 KeyLen);

//////////////////////////////////////////////////////////////////////////
//	��������	AES_Encrypt
//	������		��������
//	���������	pPlainText	-- ���ģ�������ܵ����ݣ��䳤��ΪnDataLen�ֽڡ�
//				nDataLen	-- ���ݳ��ȣ����ֽ�Ϊ��λ������ΪAES_KEY_LENGTH/8����������
//				pIV			-- ��ʼ�����������ʹ��ECBģʽ������ΪNULL��
//	���������	pCipherText	-- ���ģ��������ļ��ܺ�����ݣ�������pPlainText��ͬ��
//	����ֵ��	�ޡ�
//////////////////////////////////////////////////////////////////////////
void AES_Encrypt( unsigned char *pPlainText, unsigned char *pCipherText, 
				 unsigned int nDataLen,  unsigned char *pIV,unsigned char roll,unsigned char nMode);

//////////////////////////////////////////////////////////////////////////
//	��������	AES_Decrypt
//	������		��������
//	���������	pCipherText -- ���ģ�������ܵ����ݣ��䳤��ΪnDataLen�ֽڡ�
//				nDataLen	-- ���ݳ��ȣ����ֽ�Ϊ��λ������ΪAES_KEY_LENGTH/8����������
//				pIV			-- ��ʼ�����������ʹ��ECBģʽ������ΪNULL��
//	���������	pPlainText  -- ���ģ��������Ľ��ܺ�����ݣ�������pCipherText��ͬ��
//	����ֵ��	�ޡ�
//////////////////////////////////////////////////////////////////////////
void AES_Decrypt(unsigned char *pPlainText,  unsigned char *pCipherText, 
				 unsigned int nDataLen,  unsigned char *pIV,unsigned char roll,unsigned char nMode);

UINT8 Aes_Run(UINT8 nType,UINT8 nMode,UINT8* pIn,UINT8* pOut,UINT16 nDataLen,UINT8* pIV);

#ifdef __cplusplus
}
#endif

#endif	// __AES_H
