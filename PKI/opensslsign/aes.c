
#include "aes.h"
#include <string.h>

#define Nb	4	
						// �ԡ��֡���4�ֽڣ�Ϊ��λ�ļӽ������ݿ��С���̶�Ϊ4
#define AES_KEY_128		16
#define AES_KEY_192		24
#define AES_KEY_256		32

#define AES_ROLL_10		10
#define AES_ROLL_12		12
#define AES_ROLL_14		14


#define AES_MAX_ROLL	14
// GF(28) ����ʽ
#define BPOLY 0x1B // Lower 8 BOOLs of (x^8 + x^4 + x^3 + x + 1), ie. (x^4 + x^3 + x + 1).

// AES����Կ������Կ����Ϊ128λʱ��ռ��176�ֽڿռ�
static  unsigned char g_roundKeyTable[4*Nb*(AES_MAX_ROLL+1)];

// �����õ�SBox
static const unsigned char SBox[256] = 
{
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 
};

// �����õ�SBox
static const unsigned char InvSBox[256] = 
{
	0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d	
};


///////////////////////////////////////////////////////////////////////////////
//	��������	RotationWord
//	������		��һ�����֡����ݽ���ѭ�����ơ�
//	���������	pWord -- Ҫ���Ƶ�4�ֽ����ݡ�
//	���������	pWord -- ���ƺ��4�ֽ����ݡ�
//	����ֵ��	�ޡ�
///////////////////////////////////////////////////////////////////////////////
static void RotationWord(unsigned char *pWord)
{
	unsigned char temp = pWord[0];
	pWord[0]  = pWord[1];
	pWord[1]  = pWord[2];
	pWord[2]  = pWord[3];
	pWord[3]  = temp;
}

///////////////////////////////////////////////////////////////////////////////
//	��������	XorBytes
//	������		��������������ݡ�
//	���������	pData1 -- Ҫ���ĵ�һ�����ݡ�
//				pData1 -- Ҫ���ĵڶ������ݡ�
//				nCount -- Ҫ�������ݳ��ȡ�
//	���������	pData1 -- ����Ľ����
//	����ֵ��	�ޡ�
///////////////////////////////////////////////////////////////////////////////
static void XorBytes(unsigned char *pData1, const unsigned char *pData2, unsigned char nCount)
{
	unsigned char i;
	
	for (i = 0; i < nCount; i++)
	{
		pData1[i] ^= pData2[i];
	}
}

///////////////////////////////////////////////////////////////////////////////
//	��������	AddRoundKey
//	������		�� �м�״̬���� ���ϣ��������Կ�����ݳ���Ϊ16�ֽڡ�
//	���������	pState	  -- ״̬���ݡ�
//				pRoundKey -- ����Կ���ݡ�
//	���������	pState	  -- ��������Կ���״̬���ݡ�
//	����ֵ��	�ޡ�
///////////////////////////////////////////////////////////////////////////////
// static void AddRoundKey(unsigned char *pState, const unsigned char *pRoundKey)
// {
// 	XorBytes(pState, pRoundKey, 4*Nb);
// }

// AddRoundKey�ĺ���ʽ���Ⱥ�����ʽ���Խ�ʡ4�ֽڵ�data����
#define AddRoundKey(pState, pRoundKey) \
	XorBytes((pState), (pRoundKey), 4*Nb)


///////////////////////////////////////////////////////////////////////////////
//	��������	SubBytes
//	������		ͨ��S�����û�״̬���ݡ�
//	���������	pState	-- ״̬���ݡ�
//				nCount  -- ״̬���ݳ��ȡ�
//				bInvert	-- �Ƿ�ʹ�÷���S���ӣ�����ʱʹ�ã���
//	���������	pState	-- �û����״̬���ݡ�
//	����ֵ��	�ޡ�
///////////////////////////////////////////////////////////////////////////////
static void SubBytes(unsigned char *pState, unsigned char nCount, BOOL bInvert)
{
	unsigned char i;
	const unsigned char  *pSBox = bInvert ? InvSBox : SBox;
	
	for (i = 0; i < nCount; i++)
	{
		pState[i] = pSBox[pState[i]];
	}
}

///////////////////////////////////////////////////////////////////////////////
//	��������	ShiftRows
//	������		��״̬�������С�
//	���������	pState	-- ״̬���ݡ�
//				bInvert	-- �Ƿ������У�����ʱʹ�ã���
//	���������	pState	-- ���к��״̬���ݡ�
//	����ֵ��	�ޡ�
///////////////////////////////////////////////////////////////////////////////
static void ShiftRows(unsigned char *pState, BOOL bInvert)
{
	// ע�⣺״̬����������ʽ��ţ�

	unsigned char r;	// row��   ��
	unsigned char c;	// column����
	unsigned char temp;
	unsigned char rowData[4];
	
	for (r = 1; r < 4; r++)
	{
		// ����һ������
		for (c = 0; c < 4; c++)
		{
			rowData[c] = pState[r + 4*c];
		}
		
		temp = bInvert ? (4 - r) : r;
		for (c = 0; c < 4; c++)
		{
			pState[r + 4*c] = rowData[(c + temp) % 4];
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//	��������	GfMultBy02
//	������		��GF(28)��� ��2 ���㡣
//	���������	num	-- ������
//	���������	�ޡ�
//	����ֵ��	num����2�Ľ����
///////////////////////////////////////////////////////////////////////////////
static unsigned char GfMultBy02(unsigned char num)
{
	if ((num & 0x80) == 0)
	{
		num = num << 1;
	}
	else
	{
		num = (num << 1) ^ BPOLY;
	}
	
	return num;
}

///////////////////////////////////////////////////////////////////////////////
//	��������	MixColumns
//	������		���״̬�������ݡ�
//	���������	pState	-- ״̬���ݡ�
//				bInvert	-- �Ƿ����ϣ�����ʱʹ�ã���
//	���������	pState	-- ����к��״̬���ݡ�
//	����ֵ��	�ޡ�
///////////////////////////////////////////////////////////////////////////////
static void MixColumns(unsigned char *pState, BOOL bInvert)
{
	unsigned char i;
	unsigned char temp;
	unsigned char a0Pa2_M4;	// 4(a0 + a2)
	unsigned char a1Pa3_M4;	// 4(a1 + a3)
	unsigned char result[4];

	for (i = 0; i < 4; i++, pState += 4)
	{
		// b0 = 2a0 + 3a1 + a2 + a3 
		//    = (a0 + a1 + a2 + a3) + 2(a0 + a1) + a0

		temp = pState[0] ^ pState[1] ^ pState[2] ^ pState[3];
		result[0] = temp ^ pState[0] ^ GfMultBy02((unsigned char) (pState[0] ^ pState[1]));
		result[1] = temp ^ pState[1] ^ GfMultBy02((unsigned char) (pState[1] ^ pState[2]));
		result[2] = temp ^ pState[2] ^ GfMultBy02((unsigned char) (pState[2] ^ pState[3]));
		result[3] = temp ^ pState[3] ^ GfMultBy02((unsigned char) (pState[3] ^ pState[0]));

		if (bInvert)
		{
		// b0' = 14a0 + 11a1 + 13a2 + 9a3 
		//     = (a0 + a1 + a2 + a3) + 2(a0 + a1) + a0	���ⲿ��Ϊb0��
		//       + 2(4(a0 + a2) + 4(a1 + a3))
		//       +   4(a0 + a2)

			a0Pa2_M4 = GfMultBy02(GfMultBy02((unsigned char) (pState[0] ^ pState[2])));
			a1Pa3_M4 = GfMultBy02(GfMultBy02((unsigned char) (pState[1] ^ pState[3])));
			temp	 = GfMultBy02((unsigned char) (a0Pa2_M4 ^ a1Pa3_M4));
			result[0] ^= temp ^ a0Pa2_M4;
			result[1] ^= temp ^ a1Pa3_M4;
			result[2] ^= temp ^ a0Pa2_M4;
			result[3] ^= temp ^ a1Pa3_M4;
		}

		memcpy(pState, result, 4);
	}
}

///////////////////////////////////////////////////////////////////////////////
//	��������	BlockEncrypt
//	������		�Ե������ݼ��ܡ�
//	���������	pState -- ״̬���ݡ�
//	���������	pState -- ���ܺ��״̬���ݡ�
//	����ֵ��	�ޡ�
///////////////////////////////////////////////////////////////////////////////
static void BlockEncrypt(unsigned char *pState,unsigned char roll)
{
	unsigned char i;
	
	AddRoundKey(pState, g_roundKeyTable);
	
	for (i = 1; i <= roll; i++)	// i = [1, Nr]
	{
		SubBytes(pState, 4*Nb, 0);
		ShiftRows(pState, 0);

		if (i != roll)
		{
			MixColumns(pState, 0);
		}

		AddRoundKey(pState, &g_roundKeyTable[4*Nb*i]);
	}
	
// Ϊ�˽�ʡ���룬�ϲ���ѭ��ִ��
// 	SubBytes(pState, 4*Nb);
//	ShiftRows(pState, 0);
// 	AddRoundKey(pState, &g_roundKeyTable[4*Nb*Nr]);
}

///////////////////////////////////////////////////////////////////////////////
//	��������	BlockDecrypt
//	������		�Ե������ݽ��ܡ�
//	���������	pState -- ״̬���ݡ�
//	���������	pState -- ���ܺ��״̬���ݡ�
//	����ֵ��	�ޡ�
///////////////////////////////////////////////////////////////////////////////
static void BlockDecrypt(unsigned char *pState,unsigned char roll)
{
	unsigned char i;
	
	AddRoundKey(pState, &g_roundKeyTable[4*Nb*roll]);
	
	for (i = roll; i > 0; i--)	// i = [Nr, 1]
	{
		ShiftRows(pState, 1);
		SubBytes(pState, 4*Nb, 1);
		AddRoundKey(pState, &g_roundKeyTable[4*Nb*(i-1)]);

		if (i != 1)
		{
			MixColumns(pState, 1);
		}
	}
	
// Ϊ�˽�ʡ���룬�ϲ���ѭ��ִ��
//  ShiftRows(pState, 1);
//  SubBytes(pState, 4*Nb, 1);
//  AddRoundKey(pState, g_roundKeyTable);
}


///////////////////////////////////////////////////////////////////////////////
//	��������	AES_Init
//	������		��ʼ�����ڴ�ִ����չ��Կ������
//	���������	pKey -- ԭʼ��Կ���䳤�ȱ���Ϊ AES_KEY_LENGTH/8 �ֽڡ�
//	���������	�ޡ�
//	����ֵ��	�ޡ�
///////////////////////////////////////////////////////////////////////////////
UINT8 Aes_Init( void *pKey,UINT16 KeyLen)
{
	// ��չ��Կ
	unsigned char i;
	unsigned char *pRoundKey;
	unsigned char Rcon[4] = {0x01, 0x00, 0x00, 0x00};
   	unsigned char Nk;
	unsigned char Nr;

	Nk = KeyLen /4;

	switch(KeyLen)
	{
		case AES_KEY_128:Nr = AES_ROLL_10;break;
		case AES_KEY_192:Nr = AES_ROLL_12;break;
		case AES_KEY_256:Nr = AES_ROLL_14;break;
		default:return 1;
	}
	memcpy(g_roundKeyTable, pKey, KeyLen);

	pRoundKey = &g_roundKeyTable[KeyLen];

	for (i = Nk; i < Nb*(Nr+1); pRoundKey += 4, i++)
	{
		memcpy(pRoundKey, pRoundKey - 4, 4);

		if (i % Nk == 0)
		{
			RotationWord(pRoundKey);
			SubBytes(pRoundKey, 4, 0);
			XorBytes(pRoundKey, Rcon, 4);

			Rcon[0] = GfMultBy02(Rcon[0]);
		}
		else if (Nk > 6 && i % Nk == Nb)
		{
			SubBytes(pRoundKey, 4, 0);
		}

		XorBytes(pRoundKey, pRoundKey - 4*Nk, 4);
	}
	return 0;
}

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
				 unsigned int nDataLen,  unsigned char *pIV,unsigned char roll,unsigned char nMode)
{
	unsigned int i;
	unsigned char iv[4*Nb];

	if (pPlainText != pCipherText)
	{
		memmove(pCipherText, pPlainText, nDataLen);
	}

	if( nMode == AES128_CBC || nMode == AES192_CBC || nMode == AES256_CBC)
	{
		memcpy(iv, pIV, 4*Nb);
	}

	for (i = nDataLen/(4*Nb); i > 0 ; i--, pCipherText += 4*Nb)
	{
		if( nMode == AES128_CBC || nMode == AES192_CBC || nMode == AES256_CBC)
		{
			XorBytes(pCipherText, iv, 4*Nb);
		}

		BlockEncrypt(pCipherText,roll);
		memcpy(iv, pCipherText, 4*Nb);
	}

	if( nMode == AES128_CBC || nMode == AES192_CBC || nMode == AES256_CBC)
	{
		memcpy(pIV, iv, 4*Nb);
	}	
}

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
				 unsigned int nDataLen,  unsigned char *pIV,unsigned char roll,unsigned char nMode)
{
	unsigned int i;
	unsigned char iv[4*Nb];

	if (pPlainText != pCipherText)
	{
		memmove(pPlainText, pCipherText, nDataLen);
	}

	for (i = nDataLen/(4*Nb); i > 0 ; i--, pPlainText += 4*Nb)
	{
		if( nMode == AES128_CBC || nMode == AES192_CBC || nMode == AES256_CBC)
		{
			memcpy(iv, pPlainText, 4*Nb);
		}

		BlockDecrypt(pPlainText,roll);

		if( nMode == AES128_CBC || nMode == AES192_CBC || nMode == AES256_CBC)
		{
			XorBytes(pPlainText, pIV, 4*Nb);  
			memcpy(pIV, iv, 4*Nb);
		}
	
	}
}
UINT8 Aes_Run(UINT8 nType,UINT8 nMode,UINT8* pIn,UINT8* pOut,UINT16 nDataLen,UINT8* pIV)
{
	if(nType == AES_ENCRYPT )
	{
		switch (nMode)
		{
			case AES128_CBC:
			case AES128_ECB:
				AES_Encrypt(pIn,pOut,nDataLen,pIV,AES_ROLL_10,nMode);
				break;
			case AES192_CBC:
			case AES192_ECB:
				AES_Encrypt(pIn,pOut,nDataLen,pIV,AES_ROLL_12,nMode);
				break;
			case AES256_CBC:
			case AES256_ECB:
				AES_Encrypt(pIn,pOut,nDataLen,pIV,AES_ROLL_14,nMode);
				break;
			default :return 1;
		}
	
	}
	else 
	{
		switch (nMode)
		{
			case AES128_CBC:
			case AES128_ECB:
				AES_Decrypt(pOut,pIn,nDataLen,pIV,AES_ROLL_10,nMode);
				break;
			case AES192_CBC:
			case AES192_ECB:
				AES_Decrypt(pOut,pIn,nDataLen,pIV,AES_ROLL_12,nMode);
				break;
			case AES256_CBC:
			case AES256_ECB:
				AES_Decrypt(pOut,pIn,nDataLen,pIV,AES_ROLL_14,nMode);
				break;
			default :return 1;		
		}
	}
	return 0;		
}
