#ifndef __APDU_DEFS_H__
#define __APDU_DEFS_H__

#include "apdu_datatypes.h"

/************************************************************************
 unsigned int �� unsigned long�Ƚ�(������32λ���µ�ƽ̨��

��������ǿ���ֲ�ԣ�
��32λƽ̨�ϸ�Ӧ��ʹ��unsigned int����Ϊ����
1)��unsigned long һ���Ĵ�С��32λ���Ա�ʾ��42.9�ڡ�
2) ��unsigned long������
3) ��std::size_t��һ��������

�����64λƽ̨�Ļ���
1) unsinged int����32λ����unsigned long����64λ�ˡ�
2) ��Ӧ��ʹ��unsigned long��Ϊ��������64λ���и���Ĵ����ٶȡ�

��Ŀǰ���ԣ�64λƽ̨���������죬������64λƽ̨����ֲ�����������ǡ�

�����������Ҫ���ǿ���ֲ��(ע����Ӳ��32λƽ̨��64λ��ֲ�������������
1) ������ٶ����У�ʹ��unsigned long��������32λ����64λ�������Ĵ����ٶȡ�
2) ������ڴ����У�ʹ��unsigned int��ʹ���ڴ���������ƽ̨���ı䡣
����ͨ������Ӳ��ƽ̨�Ŀ���ֲ�ԵĿ��Ƕ��Ƕ���ģ���������Ŷ����
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


//��/�ⲿʹ�� 
#define FS_SECRET_TYPE_MC 0x80 //������Կ����MF�£����ڷ��������(��ɫ)��ʶ��(ֻ����һ��) 
#define FS_SECRET_TYPE_IA 0x81 //�ڲ���֤��Կ����MF�£������ⲿ�豸��֤���豸(ֻ����һ��) 
#define FS_SECRET_TYPE_PIN 0x82 //ʶ���룬�����û����(��ɫ)��ʶ��(���Դ��ڶ��)
#define FS_SECRET_TYPE_ESK 0x84 //���ûỰ��Կ(���Դ��ڶ��


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
