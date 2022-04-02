
#ifndef _MTOKENAPI_H_
#define _MTOKENAPI_H_

#include "apdu_defs.h"


#pragma pack(1)

#ifdef WIN32
#define DEVAPI  __stdcall
#else
#define DEVAPI
#endif

#define MAX_DATA_LEN			1024

#define MCOS_TAG_DEVATR				0x0100
#define MCOS_TAG_DEVINFO			0x0101
#define MCOS_TAG_VERSION			0x0102
#define MCOS_TAG_DEVMEMORY			0x0103
#define MCOS_TAG_DEVWORKINGMODE		0x0104
#define MCOS_TAG_DFLIST				0x0105
#define MCOS_TAG_EFLIST				0x0106
#define MCOS_TAG_CURRENTPATH		0x0107
#define MCOS_TAG_AUTHSTATUS			0x0108
#define MCOS_TAG_FORMATSTATUS		0x0109
#define MCOS_TAG_FSMODIFYCNT		0x010A



#define MCOS_PUT_TAG_LABEL		   0x0101
#define MCOS_PUT_TAG_LED		   0x0104
#define MCOS_PUT_TAG_LED2		   0x0105



#ifdef __cplusplus
extern "C" {
#endif


	typedef signed char         INT8;
	typedef signed short        INT16;
	typedef signed int          INT32;
	typedef unsigned char       UINT8;
	typedef unsigned short      UINT16;
	typedef unsigned int	    UINT32;

	typedef UINT8				BYTE;
	typedef char				CHAR;
	typedef INT16				SHORT;
	typedef UINT16				USHORT;
	typedef long				LONG;
#ifdef WIN32
	typedef unsigned long		ULONG;
#else
	typedef unsigned int		ULONG;				
#endif
	typedef UINT32				UINT;
	typedef UINT16				WORD;
	typedef unsigned long		DWORD;
	typedef UINT32				FLAGS;
	typedef CHAR *				LPSTR;
	typedef void *				HANDLE;
	typedef int					BOOL;
	typedef BYTE *				PBYTE;

#ifndef NULL
	#define NULL				0
#endif
	
#pragma pack()

	//enum all devices separated with '\0' and end with '\0\0'
	ULONG DEVAPI mToken_EnumDev(LPSTR szParameter, LPSTR szNameList, ULONG *pulSize );

	ULONG DEVAPI mToken_WaitforDevEvent(LPSTR szParameter, ULONG * ulEvent);

	ULONG DEVAPI mToken_CancelWait();

	//connect device by name 
	ULONG DEVAPI mToken_ConnectDev(LPSTR szName, HANDLE *phDev);

	//set device from external application 
	ULONG DEVAPI mToken_SetDevHandle(LPSTR szName, HANDLE hCtx, HANDLE hCardCtx, HANDLE *phDev);

	//add device from external application 
	ULONG DEVAPI mToken_AddDevHandle(LPSTR szName, HANDLE hCtx, HANDLE hCardCtx, HANDLE *phDev);

	//Update the OS handle of the device
	ULONG DEVAPI mToken_UpdateDevHandle(HANDLE hDev, HANDLE hCtx, HANDLE hCard);

	//disconnect the device 
	ULONG DEVAPI mToken_DisconnectDev(HANDLE hDev);

	//Only remove without close
	ULONG DEVAPI mToken_RemoveDevice(HANDLE hDev);

	//setup secure message channel
	ULONG DEVAPI mToken_SetupSecureMessageChannel(HANDLE hDev);

	//directly io with device 
	ULONG DEVAPI mToken_DeviceIO(HANDLE hDev, unsigned char *sendBuff, int sendLen, unsigned char * receiveBuff, int * recvLen);

	//Smart card Begin transaction 
	ULONG DEVAPI mToken_BeginTransaction(HANDLE hDev);

	//Smart card End transaction 
	ULONG DEVAPI mToken_EndTransaction(HANDLE hDev);

	//Secure Message is used or not
	ULONG DEVAPI mToken_UseSecureChannel(BOOL bSM);

	//Get max availalbe space of the file system inside the token
	ULONG DEVAPI mToken_GetFSMaxSpace(HANDLE hDev, ULONG * maxSpace);

	//Get raw smart card chip id
	ULONG DEVAPI mToken_GetChipID(HANDLE hDev,  unsigned char chipID[32], int * idLen);

	//Format device
	ULONG DEVAPI mToken_FormatDevice(HANDLE hDev, char manufacture[32], char issuer[32], 
									char label[32], char serialNum[32], 
									long total, int workingMode, char MFName[16], 
									unsigned long ulAlgIdOfMC, unsigned char MCKeyValue[32], int maxRetryOfMC,
									unsigned long ulAlgIdOfIA, unsigned char IAKeyValue[32], int maxRetryOfIA,
									unsigned long ulAlgIdOfEA, unsigned char EAKeyValue[32], int maxRetryOfEA);

	//Get health check results 
	ULONG DEVAPI mToken_GetHealthStatus(HANDLE hDev, unsigned char status[8]);

	/*
	errorType
	0 self-test log
	1 apdu error log
	*/
	ULONG DEVAPI mToken_GetErrorLog(HANDLE hDev, unsigned char errorType, unsigned char status[8]);

	//Switch working mode between FIPS/NON FIPS
	ULONG DEVAPI mToken_ChangeWorkingMode(HANDLE hDev, int mode);

	//Health check 
	ULONG DEVAPI mToken_HealthCheck(HANDLE hDev);

	//DRBG Reseed
	ULONG DEVAPI mToken_DRBGReseed(HANDLE hDev);

	//Install PIN for the current application
	ULONG DEVAPI mToken_InstallPin(HANDLE hDev, int pinID, unsigned long ulAlgID, 
								int maxRetryTimes, 
								unsigned char keyValue[32], int keyValueLen, 
								struAccessControl * UseAC, struAccessControl * ModifyAC, 
								int unblock_keyId, int roleId, int isFinal);

	//Install Secret Key for the current application
	ULONG DEVAPI mToken_InstallSecretKey(HANDLE hDev, int keyID, unsigned long ulAlgID,
		unsigned char keyValue[32], int keyValueLen,
		struAccessControl * UseAC, struAccessControl * ModifyAC,
		int isFinal);

	//get the specified keys' information, including PIN and Secret Keys
	ULONG DEVAPI mToken_GetSecretKeysInfo(HANDLE hDev, int startKeyId, int * keyNumber,
		unsigned long ulAlgIDs[], int type[]);

	ULONG DEVAPI mToken_GetChallenge(HANDLE hDev, int length, unsigned char * challenge);

	ULONG DEVAPI mToken_DRBGGenRandom(HANDLE hDev, int length, unsigned char * random);

	ULONG DEVAPI mToken_DeviceAuth(HANDLE hDev, int keyId, unsigned char * pAck, int ack_len );

	ULONG DEVAPI mToken_InternalAuth(HANDLE hDev, unsigned char * request, int request_len,
					unsigned char * response, int * response_len);

	/*
	// 2021.4 old
	mToken_VerifyPin  mToken_ChangePin  mToken_UnblockPin mToken_ClearSecureState no mode
	*/ 
	ULONG DEVAPI mToken_VerifyPin(HANDLE hDev, int pinId, int mode, unsigned char * pAck, int ack_len );


	ULONG DEVAPI mToken_GetPinInfo(HANDLE hDev, int pinId, unsigned long * ulAlgID, int * type, int * leftTimes,
		struAccessControl * UseAC, struAccessControl * ModifyAC, int * unbloked_secretId, int * roleId, int * bDefault);

	ULONG DEVAPI mToken_ChangePin(HANDLE hDev, int pinID, int mode, unsigned char * acknowlege, int ack_len, unsigned char * newKeyValue, int keyValueLen);

	ULONG DEVAPI mToken_UnblockPin(HANDLE hDev, int pinID, int mode, unsigned char * acknowlege, int ack_len, unsigned char * newKeyValue, int keyValueLen);

	ULONG DEVAPI mToken_UnloadPin(HANDLE hDev, int pinID, unsigned char * newKeyValue, int keyValueLen);

	ULONG DEVAPI mToken_ClearSecureState(HANDLE hDev, int mode, int role);
	
	ULONG DEVAPI mToken_SelectFileById(HANDLE hDev, int fileId, struFILE * sFile);

	ULONG DEVAPI mToken_SelectFileById2(HANDLE hDev, unsigned char fileId[2], struFILE * sFile);

	ULONG DEVAPI mToken_SelectChildDFById(HANDLE hDev, int fileId, struFILE * sFile);

	ULONG DEVAPI mToken_SelectEFById(HANDLE hDev, int fileId, struFILE * sFile);

	ULONG DEVAPI mToken_SelectParentDF(HANDLE hDev, struFILE * sFile);

	ULONG DEVAPI mToken_SelectDFByName(HANDLE hDev, char * DFName, struFILE * sFile);

	ULONG DEVAPI mToken_SelectEFByName(HANDLE hDev, char * EFName, struFILE * sFile);

	ULONG DEVAPI mToken_SelectDFByPathFromMF(HANDLE hDev, unsigned char * path, int path_len, struFILE * sFile);

	ULONG DEVAPI mToken_SelectDFByPathFromDF(HANDLE hDev, unsigned char * path, int path_len, struFILE * sFile);

	ULONG DEVAPI mToken_CreateDF(HANDLE hDev, char DFName[16], int fileID, struAccessControl * createAC, struAccessControl * deleteAC);

	ULONG DEVAPI mToken_CreateEF(HANDLE hDev, int fileType, int fileID, int shortFileID, int fileSize, int SMAccess, 
								char EFName[16],
								struAccessControl * ReadAC, struAccessControl * WriteAC,
								struAccessControl * useAC);
	

	/* // 2021.4  old
	ULONG DEVAPI mToken_CreateEF(HANDLE hDev, int fileType, int fileID, int shortFileID, int fileSize, int SMAccess,
		struAccessControl * ReadAC, struAccessControl * WriteAC,
		struAccessControl * useAC);
	*/

	ULONG DEVAPI mToken_DeleteCurrentFile(HANDLE hDev);

	ULONG DEVAPI mToken_DeleteFileById(HANDLE hDev, int fileId);

	ULONG DEVAPI mToken_DeleteFileByName(HANDLE hDev, int isEF, char Name[16]);

	ULONG DEVAPI mToken_ReadBinary(HANDLE hDev, int offset, int read_len, unsigned char * pData, int * dataLen);

	ULONG DEVAPI mToken_ReadBinarySid(HANDLE hDev, int shortFileId, int offset, int read_len, unsigned char * pData, int * dataLen);

	ULONG DEVAPI mToken_ReadBinaryName(HANDLE hDev, char EFName[16], int offset, int read_len, unsigned char * pData, int * dataLen);

	ULONG DEVAPI mToken_UpdateBinary(HANDLE hDev, int offset, unsigned char * data, int data_len, int isEOF);

	ULONG DEVAPI mToken_UpdateBinarySid(HANDLE hDev, int shortFileId, int offset, unsigned char * data, int data_len, int isEOF);

	ULONG DEVAPI mToken_UpdateBinaryName(HANDLE hDev, char EFName[16], int offset, unsigned char * data, int data_len, int isEOF);

	ULONG DEVAPI mToken_AppendRecord(HANDLE hDev, unsigned char * data, int data_len, int * recordId);

	ULONG DEVAPI mToken_AppendRecordSid(HANDLE hDev, int shortFileId, unsigned char * data, int data_len, int * recordId);

	ULONG DEVAPI mToken_AppendRecordName(HANDLE hDev, char EFName[16], unsigned char * data, int data_len, int * recordId);

	ULONG DEVAPI mToken_ReadRecordLength(HANDLE hDev, int recordId, int * recordLength);

	ULONG DEVAPI mToken_ReadRecordLengthSid(HANDLE hDev, int shortFileId, int recordId, int * recordLength);

	ULONG DEVAPI mToken_ReadRecordLengthName(HANDLE hDev, char EFName[16], int recordId, int * recordLength);

	ULONG DEVAPI mToken_ReadRecord(HANDLE hDev, int recordId, unsigned char * data, int * length);

	ULONG DEVAPI mToken_ReadRecordSid(HANDLE hDev, int shortFileId, int recordId, unsigned char * data, int * length);

	ULONG DEVAPI mToken_ReadRecordName(HANDLE hDev, char EFName[16], int recordId, unsigned char * data, int * length);

	ULONG DEVAPI mToken_UpdateRecord(HANDLE hDev, int recordId, unsigned char * data, int data_len);

	ULONG DEVAPI mToken_UpdateRecordSid(HANDLE hDev, int shortFileId, int recordId, unsigned char * data, int data_len);

	ULONG DEVAPI mToken_UpdateRecordName(HANDLE hDev, char EFName[16], int recordId, unsigned char * data, int data_len);

	ULONG DEVAPI mToken_GetData(HANDLE hDev, int tag, unsigned char * data, int * length);

	ULONG DEVAPI mToken_PutData(HANDLE hDev, int tag, unsigned char * data, int data_len);

	ULONG DEVAPI mToken_HashInit(HANDLE hDev, unsigned long ulAlgId);

	ULONG DEVAPI mToken_HashUpdate(HANDLE hDev, unsigned char * pInput, int input_len);

	ULONG DEVAPI mToken_HashFinal(HANDLE hDev, unsigned char * pInput, int input_len, unsigned char *hashVal, int *hashLen);

	ULONG DEVAPI mToken_Hash(HANDLE hDev,  unsigned long ulAlgId, unsigned char * pInput, int input_len, 
		unsigned char *hashVal, int *hashLen);

	ULONG DEVAPI mToken_SecureMessageGenEphemeral(HANDLE hDev, unsigned long ulAsymAlgID, int bDomainID, struECCPublicBlob * eccPubKey);

	ULONG DEVAPI mToken_SecureMessageKDF(HANDLE hDev, unsigned long ulAsymAlgID, unsigned long ulHashAlgID, struOtherInfo * otherInfo, struECCPublicBlob* eccPubBlob);

	ULONG DEVAPI mToken_SessionKASGenerateEphemeral(HANDLE hDev,unsigned long ulAsymAlgID , int bDomainID, struECCPublicBlob * pubKey);

	ULONG DEVAPI mToken_SessionKDF(HANDLE hDev,unsigned long ulAsymAlgID, unsigned long ulHashAlgID, 
								int session_key_id,unsigned long SessionAlgID, 
								struOtherInfo * otherInfo,struECCPublicBlob* peerEccPubBlob);

	ULONG DEVAPI mToken_GenSessionKey(HANDLE hDev,int session_key_id, unsigned long SessionAlgID);

	ULONG DEVAPI mToken_CreateSessionKey(HANDLE hDev,int session_key_id, unsigned long SessionAlgID,
									unsigned char * keyValue, int keyLen);

	ULONG DEVAPI mToken_GetSessionKeyInfo(HANDLE hDev, int session_key_id, unsigned long * ulAlgId);

	ULONG DEVAPI mToken_DestroySessionKey(HANDLE hDev,int session_key_id);

	ULONG DEVAPI mToken_DestroyAllSessionKeys(HANDLE hDev);

	ULONG DEVAPI mToken_SymImportSessionKey(HANDLE hDev,int unwrap_session_key_id, int new_session_key_id, 
											unsigned long ulAlgID,	unsigned char * keyWrapped, int wrappedLen);

	ULONG DEVAPI mToken_SymExportSessionKey(HANDLE hDev,int wrap_session_key_id, int session_key_id, unsigned long * ulAlgId,
					unsigned char * wrappedKey, int * wrappedLen);

	//Save session key into a secret key of the storage 
	ULONG DEVAPI mToken_SymSaveSessionKey(HANDLE hDev, int session_key_id, int secret_key_id);

	//Load session key from the secret key of the storage
	ULONG DEVAPI mToken_SymLoadSessionKey(HANDLE hDev, int session_key_id, int secret_key_id);

	//session_key_id:  SessionID:1-4
	//Padding mode: 0.	Zero padding 		1.	PKCS7 Padding
	//iv : iv value for cryption 
	ULONG DEVAPI mToken_SymCryptInit(HANDLE hDev,int session_key_id, int padding_mode, unsigned char * iv, int iv_len);

	ULONG DEVAPI mToken_SymEncryptUpdate(HANDLE hDev,unsigned char * input, int input_len, unsigned char * output, int * output_len);

	ULONG DEVAPI mToken_SymEncryptFinal(HANDLE hDev,unsigned char * input, int input_len, unsigned char * output, int * output_len);

	ULONG DEVAPI mToken_SymDecryptUpdate(HANDLE hDev,unsigned char * input, int input_len, unsigned char * output, int * output_len);

	ULONG DEVAPI mToken_SymDecryptFinal(HANDLE hDev,unsigned char * input, int input_len, unsigned char * output, int * output_len);

	ULONG DEVAPI mToken_MacInit(HANDLE hDev,unsigned long session_key_id);

	ULONG DEVAPI mToken_MacUpdate(HANDLE hDev,unsigned char * pInput, int input_len);

	ULONG DEVAPI mToken_MacFinal(HANDLE hDev,unsigned char * pInput, int input_len, unsigned char *macVal, int * macLen);

	ULONG DEVAPI mToken_Mac(HANDLE hDev,  int session_key_id, unsigned char * pInput, int input_len, 
							unsigned char *Mac, int *MacLen);

	ULONG DEVAPI mToken_AsymGenKeypair(HANDLE hDev,int pubFileId, int priFileId, unsigned long ulAlgID, int bDomainID, int bUsage);

	ULONG DEVAPI mToken_AsymWrapImportPub(HANDLE hDev,int wrapp_session_key_id, int pubFileId,
		unsigned long asymAlgoID, int bDomainID, int bUsage,
		unsigned char * wrappedPubKey, int wrapLen);

	ULONG DEVAPI mToken_AsymImportPub(HANDLE hDev, int pubFileId, unsigned long asymAlgoID, int bDomainID, int bUsage,
		unsigned char * pubKey, int pubKeyLen);

	ULONG DEVAPI mToken_AsymWrapImportPri(HANDLE hDev,int wrapp_session_key_id, int priFileId, int pairedPubFileId,
		unsigned long ulAlgID, int bDomainID, int bUsage,
		unsigned char * wrappedPriKey, int wrappedLen);

	ULONG DEVAPI mToken_AsymWrapExportPub(HANDLE hDev,int wrapp_session_key_id, int pubFileId,
		unsigned long * ulAlgID, int * bDomainId, 
		int *bUsage, unsigned char * wrappedKey, int * wrappedLen);

	ULONG DEVAPI mToken_AsymExportPub(HANDLE hDev,int pubFileId, unsigned long * ulAlgID, int * bDomainId, 
		int *bUsage, unsigned char * pubKey, int * pubKeyLen);

	ULONG DEVAPI mToken_AsymWrapExportPri(HANDLE hDev,int wrapp_session_key_id, int priFileId,
		unsigned long * ulAlgID, int * bDomainId, 
		int *bUsage, unsigned char * wrappedKey, int * wrappedLen);


	ULONG DEVAPI mToken_AsymImportSessionKey(HANDLE hDev,int session_key_id, int priFileId, 
		unsigned long ulAlgID, unsigned char * bCipherkey, int cipherLen);

	ULONG DEVAPI mToken_AsymExportSessionKey(HANDLE hDev,int session_key_id, int pubFileId,  
		unsigned long * ulSessionAlgID, unsigned char * bCipherKey, int * keyLen);

	ULONG DEVAPI mToken_AsymSignHash(HANDLE hDev,int priFileId, unsigned char * signature, int *sig_len);

	ULONG DEVAPI mToken_AsymSign(HANDLE hDev,int priFileId, unsigned long ulHashAlgoID, unsigned char * hashVal, int hashLen, 
		unsigned char * signature, int *sig_len);

	ULONG DEVAPI mToken_AsymVerifyHash(HANDLE hDev,int pubFileId, unsigned char * signature, int sig_len);

	ULONG DEVAPI mToken_AsymVerify(HANDLE hDev,int pubFileId, unsigned char * signature, int sig_len,
		unsigned long ulHashAlgoID, unsigned char * hashVal, int hashLen);

	ULONG DEVAPI mToken_AsymPublicEncryption(HANDLE hDev,int pubFileId, unsigned char * input, int input_len, unsigned char * output, int *output_len);

	ULONG DEVAPI mToken_AsymPrivateDecryption(HANDLE hDev,int priFileId, unsigned char * input, int input_len, unsigned char * output, int *output_len);

	ULONG DEVAPI mToken_AsymPublicOperation(HANDLE hDev,int pubFileId, unsigned char * input, int input_len, unsigned char * output, int *output_len);

	ULONG DEVAPI mToken_AsymPrivateOperation(HANDLE hDev,int priFileId, unsigned char * input, int input_len, unsigned char * output, int *output_len);

	////////////////Helper//////
	
	ULONG DEVAPI mToken_KASSessionKey( HANDLE hDev, int session_key_id, unsigned long sessionAlgId, 
							unsigned char * keyValue, int * keyLen );

	ULONG DEVAPI mToken_KASSessionKeyFast( HANDLE hDev, int session_key_id, unsigned long sessionAlgId, 
							unsigned char * keyValue, int * keyLen );

	ULONG DEVAPI mToken_SetSessionKey(HANDLE hDev, int wrappSessionId, int session_key_id, unsigned long sessionAlgId, 
		unsigned char * keyValue, int keyLen);

	ULONG DEVAPI mToken_SoftDES3KeyUnwrap(unsigned char *Key, int KLen, 
		unsigned char *Input, int InputLen, unsigned char *Output, int * OutputLen);
	
	ULONG DEVAPI mToken_SoftAESKeyWrap(unsigned char *Key, unsigned char KLen, 
		unsigned char *Input, int InputLen, unsigned char *Output, int *OutputLen );

	ULONG DEVAPI mToken_SoftAESKeyUnwrap(unsigned char *Key, unsigned char KLen,
		unsigned char *Input, int InputLen, unsigned char *Output, int * OutputLen);


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif


	///////////////Command for FIPS Testing
	ULONG DEVAPI mToken_DRBGInstantiate(HANDLE hDev, int wEntropyLen, unsigned char * EntropyInput,
		int wNonceLen, unsigned char * bNonce,
		int wPersonalizationStringLen, unsigned char * bPersonalizationString);

	ULONG DEVAPI mToken_DRBGReseed2(HANDLE hDev, int wEntropyLen, unsigned char * EntropyInputReseed, int wAdditionalLen, unsigned char * AdditionalInputReseed);

	ULONG DEVAPI mToken_DRBGGenerate(HANDLE hDev, int wAdditionalLen, unsigned char * AdditionalInput, int rngLen, unsigned char * rngValue);

	ULONG DEVAPI mToken_DRBGGetEntropy(HANDLE hDev, int length, unsigned char * entropy);

	ULONG DEVAPI mToken_DRBGGenerateCRNGT(HANDLE hDev, int rngLen, unsigned char * rngValue);

	ULONG DEVAPI mToken_DRBGGetEntropyCRNGT(HANDLE hDev, int length, unsigned char * entropy);

	ULONG DEVAPI mToken_ECCCDHPrimitiveGenerateEphemeral(HANDLE hDev, unsigned long ulAsymAlgID, int bDomainID, struECCPublicBlob * eccPubkey);

	ULONG DEVAPI mToken_ECCCDHPrimitiveCalculateZ(HANDLE hDev, unsigned long ulAsymAlgID, struECCPublicBlob * peerPubKey, unsigned char * Z, int * Z_len);

	ULONG DEVAPI mToken_KASGenerateEphemeral(HANDLE hDev, unsigned long ulAsymAlgID, int bDomainID, struECCPublicBlob * eccPubkey);

	ULONG DEVAPI mToken_KASDKM(HANDLE hDev, unsigned long ulAsymAlgID, int bDomainID, unsigned long ulHashAlgID,
		int bDKMLen, int bMsgLen, unsigned char * bMessage,
		unsigned char * OtherInfo, int otherInfoLen, struECCPublicBlob * peerPubKey,
		unsigned char * DKM, unsigned char * macTag, int * tagLen);

	ULONG DEVAPI mToken_KASValidityImportECCKeypair(HANDLE hDev, unsigned long ulAsymAlgID, int bDomainID,
		struECCPublicBlob * pEccPubKey, struECCPrivateBlob * pEccPriKey);

	ULONG DEVAPI mToken_KASValidityKeyAgreement(HANDLE hDev, unsigned long ulAsymAlgID, int bDomainID, unsigned long ulHashAlgID,
		int bDKMLen, int bMsgLen, unsigned char * bMessage,
		unsigned char * OtherInfo, int otherInfoLen, struECCPublicBlob * PeerPubKey,
		unsigned char * DKM, int *tagLen, unsigned char *macTag);

	ULONG DEVAPI mToken_KeyWrap(HANDLE hDev, unsigned long ulAlgID, unsigned char *key, int keyLen,
		unsigned char * bPlaintext, int plainLen, unsigned char * bCiphertext, int * cipherLen, int inverse);

	ULONG DEVAPI mToken_KeyUnwrap(HANDLE hDev, unsigned long ulAlgID, unsigned char *key, int keyLen,
		unsigned char * bCiphertext, int cipherLen, unsigned char * bPlaintext, int * plainLen, int inverse);

	ULONG DEVAPI mToken_GenExtRSAKeyPair(HANDLE hDev, unsigned long ulAlgID, struRSAPublicBlob * rsaPubKey, struRSAPrivateBlob * rsaPriKey);

	ULONG DEVAPI mToken_GenExtKeyPairPCT(HANDLE hDev, unsigned long ulAlgID, unsigned char * pubKey, int * pubKeyLen,
		unsigned char * priKey, int * priKeyLen);

	ULONG DEVAPI mToken_GenExtECDSAKeyPair(HANDLE hDev, unsigned long ulAlgID, struECCPublicBlob * eccPubKey, struECCPrivateBlob * eccPriKey);

	ULONG DEVAPI mToken_EraseFile(HANDLE hDev, int fileId);

	ULONG DEVAPI mToken_ReadFileContent(HANDLE hDev, int fileId, int offset, int length, unsigned char * fileData, int * dataLength);

	ULONG DEVAPI mToken_ReadSession(HANDLE hDev, int sessionId, unsigned char * sessionData, int * length);

	//2022.3.1 1ÃÌº”≤‚ ‘»Ì∏¥Œª÷∏¡Ó
	ULONG DEVAPI mToken_SoftReset(HANDLE hDev);

	ULONG DEVAPI mToken_ReadFileSystem(HANDLE hDev, int offset, unsigned char data[1024]);

	ULONG DEVAPI mToken_WriteFileSystem(HANDLE hDev, int offset, unsigned char data[1024]);

	ULONG DEVAPI mToken_ClearFW(HANDLE hDev);

#ifdef __cplusplus
}
#endif



#ifdef __cplusplus
extern "C" {
#endif

	ULONG DEVAPI mToken_ConfigureVIDPID(HANDLE hDev, int vid, int pid);

	ULONG DEVAPI mToken_ConfigureReader(HANDLE hDev, unsigned char * readerName, int len);

	ULONG DEVAPI mToken_ConfigureCDInquiry(HANDLE hDev, LPSTR pszVendor, LPSTR pszProduct);

	ULONG DEVAPI mToken_EnableCD(HANDLE hDev, int enable);

	ULONG DEVAPI mToken_ConfigureVendor(HANDLE hDev, unsigned char * vendorName, int vendor_len);

	ULONG DEVAPI mToken_GetVIDPID(HANDLE hDev, int * vid, int * pid);

	ULONG DEVAPI mToken_GetReader(HANDLE hDev, unsigned char * readerName, int * len);

	ULONG DEVAPI mToken_GetCDInquiry(HANDLE hDev, unsigned char * inquiry, int * inquiryLen);

	ULONG DEVAPI mToken_GetUSBVendor(HANDLE hDev, unsigned char * vendorName, int * len);

	ULONG DEVAPI mToken_USBReset(HANDLE hDev);


#ifdef __cplusplus
}
#endif

#endif //_SKFAPI_H_

