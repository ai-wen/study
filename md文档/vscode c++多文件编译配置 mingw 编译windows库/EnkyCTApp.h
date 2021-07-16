#ifndef ENKYCT_API_H__H_
#define ENKYCT_API_H__H_

#ifdef _WIN32
	#define WINAPI __stdcall
#else
	#define WINAPI
#endif

	#ifdef __cplusplus
	extern "C"
	{
	#endif
		long WINAPI EnkyCTFind(char appID[32], long * keyHandles, long * keyNumber);
		long WINAPI EnkyCTCheckExist(long keyHandle);
		long WINAPI EnkyCTGetUid(long keyHandle, char UID[32]);
		long WINAPI EnkyCTOpen( long keyHandle,long password1,long password2, long password3, long password4 );

		long WINAPI EnkyCTReadPageFile( long keyHandle, long page, long startAddr, long length, unsigned char *pBuffer );
		long WINAPI EnkyCTWritePageFile( long keyHandle, long page, long startAddr, long length, unsigned char *pBuffer );

		long WINAPI EnkyCTTriDesEncrypt(long keyHandle, unsigned char * pDataBuffer, long dataLen);
		long WINAPI EnkyCTTriDesDecrypt(long keyHandle, unsigned char * pDataBuffer, long dataLen);

		long WINAPI EnkyCTReadMemory(long keyHandle, long startAddr, long length, unsigned char* pBuffer);
		long WINAPI EnkyCTWriteMemory(long keyHandle, long startAddr, long length, unsigned char* pBuffer);

		long WINAPI EnkyCTGetExpiryDateTime( long keyHandle, long *endYear, long *endMonth, long *endDay, long *endHour, long *endMin , long *endSec);
		long WINAPI EnkyCTGetCurrentDateTime( long keyHandle, long *currYear, long *currMonth, long *currDay, long *currHour, long *currMin , long *currSec );
		long WINAPI EnkyCTGetCount(long keyHandle, long *Count);

		long WINAPI EnkyCTGetSoftVersion(long keyHandle, long *SoftVersion);	

		long WINAPI EnkyCTRemoteRequest(long keyHandle,char Request[16]);
		long WINAPI EnkyCTRemoteRegister(long keyHandle, char Register[16]);
		long WINAPI EnkyCTRemoteRequestEx(long keyHandle,char Request[32]);

		long WINAPI EnkyCTUpgrade(long keyHandle, char *FilePath);

		long WINAPI EnkyCTClose( long keyHandle );

		long WINAPI EnkyCTGetLastError();
#ifdef __cplusplus
	}
	#endif
#endif
