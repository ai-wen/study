#ifndef __APDU_DEFS_H__
#define __APDU_DEFS_H__

#include "apdu_datatypes.h"

/************************************************************************
 unsigned int 和 unsigned long比较(不考虑32位以下的平台）

如果不考虑可移植性：
在32位平台上更应该使用unsigned int，因为它：
1)和unsigned long 一样的大小，32位可以表示到42.9亿。
2) 比unsigned long更常用
3) 和std::size_t是一样的类型

如果是64位平台的话：
1) unsinged int仍是32位，而unsigned long就是64位了。
2) 更应该使用unsigned long因为处理器对64位具有更快的处理速度。

就目前而言，64位平台还不够成熟，所以向64位平台的移植基本不做考虑。

但是如果你坚持要考虑可移植性(注意是硬件32位平台向64位移植，而非软件）：
1) 如果对速度敏感：使用unsigned long，无论在32位还是64位都有最快的处理速度。
2) 如果对内存敏感：使用unsigned int，使用内存量不会因平台而改变。
不过通常对于硬件平台的可移植性的考虑都是多余的（不够敏捷哦）。
************************************************************************/

//custom defined SW1, SW2 values
#define MCOS_SW_COMM_SERIALIZE		0x8001
#define MCOS_SW_DEVHANDLE_INVALID	0x8002
#define MCOS_SW_COMM_SEND			0x8003
#define MCOS_SW_COMM_RECEIVE		0x8004
#define MCOS_SW_COMM_DESERILIZE		0x8005

#define FS_FILE_TYPE_DF					0x00
#define FS_FILE_TYPE_EF					0x01

#define FS_FILE_STRUCT_BINARY			0x00
#define FS_FILE_STRUCT_RECORD			0x10
#define FS_FILE_STRUCT_ASYM_PUBKEY		0x20
#define FS_FILE_STRUCT_ASYM_PRIKEY		0x30


//内/外部使用 
#define FS_SECRET_TYPE_MC 0x80 //主控密钥，在MF下，用于发行者身份(角色)的识别(只存在一个) 
#define FS_SECRET_TYPE_IA 0x81 //内部认证密钥，在MF下，用于外部设备认证本设备(只存在一个) 
#define FS_SECRET_TYPE_PIN 0x82 //识别码，用于用户身份(角色)的识别(可以存在多个)
#define FS_SECRET_TYPE_ESK 0x84 //永久会话密钥(可以存在多个


#define MAX_DF_NAME_LEN			16
#define MAX_EF_NAME_LEN			16
#define MAX_ECC_KEY_LEN			68

#define MAX_RSA_KEY_LEN			256

#define MCOS_ROLE_EVERYONE				0xFF
#define MCOS_ROLE_NOBODY				0x00
#define MCOS_ROLE_ADMIN(n)				((0x01 << (4+n))&0x70) 
#define MCOS_ROLE_USER(n)				((0x01 << n)&0x0F)

#define MCOS_AC_ISSUER(ac)				(ac &0x80)
#define MCOS_AC_USER(ac)				(ac &0x0F)
#define MCOS_AC_ADMIN(ac)				(ac &0x70)

#define WORKINGMODE_NONFIPS			0x00
#define WORKINGMODE_FIPS			0x01

#define KEY_MC_ID				0x01
#define KEY_IA_ID				0x02
#define KEY_EA_ID				0x03

	#pragma pack(1)

	//other infor ( max 512 bytes)
	typedef struct
	{
		char * label;
		unsigned char * s_nonce;
		int s_len;
		unsigned char * t_nonce;
		int t_len;

	}struOtherInfo;

	typedef struct
	{
		UINT32 ulBitLen;
		unsigned char bModulus[MAX_RSA_KEY_LEN];
		unsigned char bPublicExponent[4];

	}struRSAPublicBlob;

	typedef struct
	{
		UINT32 ulBitLen;
		unsigned char bPrivateExponent[MAX_RSA_KEY_LEN];
		unsigned char Prime1[MAX_RSA_KEY_LEN/2];
		unsigned char Prime2[MAX_RSA_KEY_LEN/2];

		unsigned char Prime1Exponent[MAX_RSA_KEY_LEN/2];
		unsigned char Prime2Exponent[MAX_RSA_KEY_LEN/2];

		unsigned char Coefficient[MAX_RSA_KEY_LEN/2];
		unsigned char Reserved[4];

	}struRSAPrivateBlob;

	typedef struct
	{
		UINT32 ulBitLen;
		unsigned char bX[MAX_ECC_KEY_LEN];
		unsigned char bY[MAX_ECC_KEY_LEN];
		unsigned char Reserved[4];

	}struECCPublicBlob;

	typedef struct
	{
		UINT32 ulBitLen;
		unsigned char bD[MAX_ECC_KEY_LEN];

	}struECCPrivateBlob;

	typedef struct 
	{
		UINT8 Roles; //bit 0 - 3 : USER  bit 4 - 6 : Admin bit 7:manufacture

	}struAccessControl;

	typedef struct  
	{
		char DFName[MAX_DF_NAME_LEN];
		UINT16 fID;		
		struAccessControl createAC;
		struAccessControl deleteAC;
	}struDF;

	typedef struct  
	{
		char   EFName[MAX_EF_NAME_LEN];
		UINT16 fID;
		UINT8  bModifyCNT;
		UINT8  shortID;					
		UINT16 Size;					
		UINT8  fileStructure;
		struAccessControl  readAC;
		struAccessControl  writeAC;
		UINT8  bSM;

	}struEFBinary;

	typedef struct  
	{
		char   EFName[MAX_EF_NAME_LEN];
		UINT16 fID;						
		UINT8  bModifyCNT;
		UINT8  shortID;					
		UINT16 Size;	
		UINT8  fileStructure;
		UINT8  recordNum;
		struAccessControl  readAC;
		struAccessControl  writeAC;
		UINT8  bSM;

	}struEFRecord;

	typedef struct  
	{
		char   EFName[MAX_EF_NAME_LEN];
		UINT16 fID;						
		UINT8  bModifyCNT;
		UINT8  shortID;					
		UINT16 Size;	
		UINT8  fileStructure;
		UINT32 ulAlgId;
		UINT16 pairedFID;
		UINT8  domainId;
		UINT8  usage;
		
		struAccessControl  readAC;
		struAccessControl  writeAC;
		struAccessControl  useAC;
		UINT8  bSM;

	}struEFKeyFile;

	typedef struct
	{
		UINT8			fileType;
		UINT8			fileStruct;
		union
		{
			struDF			df;
			struEFBinary	efBinary;
			struEFRecord	efRecord;
			struEFKeyFile	efKeyFile;

		}fileProp;

	}struFILE;

	#pragma pack()


#endif
