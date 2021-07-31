#include "CryptMsgApiHook.h"
#include <stdio.h>


CRYPTMSGCALCULATEENCODEDLENGTH CryptMsgApiHook::_old_CryptMsgCalculateEncodedLength = 0;
CRYPTMSGOPENTOENCODE CryptMsgApiHook::_old_CryptMsgOpenToEncode = 0;

CryptMsgApiHook::CryptMsgApiHook()
{
}


CryptMsgApiHook::~CryptMsgApiHook()
{
}

DWORD WINAPI CryptMsgApiHook::MyCryptMsgCalculateEncodedLength(
	_In_ DWORD dwMsgEncodingType,
	_In_ DWORD dwFlags,
	_In_ DWORD dwMsgType,
	_In_ void const *pvMsgEncodeInfo,
	_In_opt_ LPSTR pszInnerContentObjID,
	_In_ DWORD cbData
)
{
	printf("MyCryptMsgCalculateEncodedLength\n");
	DWORD dwRetLen = 4096 * 10;
	CMSG_SIGNED_ENCODE_INFO* signedMsgEncodeInfo = (CMSG_SIGNED_ENCODE_INFO*)pvMsgEncodeInfo;
	CMSG_SIGNER_ENCODE_INFO signerEncodeInfo = signedMsgEncodeInfo->rgSigners[0];
	CRYPT_ALGORITHM_IDENTIFIER hashAlgorithm = signerEncodeInfo.HashAlgorithm;

	if (CMSG_SIGNED == dwMsgType &&
		(0 == memcmp(hashAlgorithm.pszObjId, (LPSTR)szOID_RSA_SHA256RSA, strlen(szOID_RSA_SHA256RSA)) ||
			0 == memcmp(hashAlgorithm.pszObjId, (LPSTR)szOID_NIST_sha256, strlen(szOID_NIST_sha256)))
		)
	{
		//signedMsgEncodeInfo->rgSigners[0].HashAlgorithm.pszObjId = (LPSTR)szOID_RSA_SHA1RSA;
		signedMsgEncodeInfo->rgSigners[0].HashAlgorithm.pszObjId = (LPSTR)szOID_OIWSEC_sha1;
	}
	
	if (_old_CryptMsgCalculateEncodedLength)
		return  _old_CryptMsgCalculateEncodedLength(dwMsgEncodingType, dwFlags, dwMsgType, pvMsgEncodeInfo, pszInnerContentObjID, cbData);
	
	return 0;
}

HCRYPTMSG WINAPI CryptMsgApiHook::MyCryptMsgOpenToEncode(
	_In_ DWORD dwMsgEncodingType,
	_In_ DWORD dwFlags,
	_In_ DWORD dwMsgType,
	_In_ void const *pvMsgEncodeInfo,
	_In_opt_ LPSTR pszInnerContentObjID,
	_In_opt_ PCMSG_STREAM_INFO pStreamInfo
)
{
	CMSG_SIGNED_ENCODE_INFO* signedMsgEncodeInfo = (CMSG_SIGNED_ENCODE_INFO*)pvMsgEncodeInfo;
	CMSG_SIGNER_ENCODE_INFO signerEncodeInfo = signedMsgEncodeInfo->rgSigners[0];
	CRYPT_ALGORITHM_IDENTIFIER hashAlgorithm = signerEncodeInfo.HashAlgorithm;
	
	if (CMSG_SIGNED == dwMsgType &&
		(0 == memcmp(hashAlgorithm.pszObjId, (LPSTR)szOID_RSA_SHA256RSA, strlen(szOID_RSA_SHA256RSA))||
		 0 == memcmp(hashAlgorithm.pszObjId, (LPSTR)szOID_NIST_sha256, strlen(szOID_NIST_sha256)) )
		)
	{
		//signedMsgEncodeInfo->rgSigners[0].HashAlgorithm.pszObjId = (LPSTR)szOID_RSA_SHA1RSA;
		signedMsgEncodeInfo->rgSigners[0].HashAlgorithm.pszObjId = (LPSTR)szOID_OIWSEC_sha1;
	}

	if (_old_CryptMsgOpenToEncode)
		return _old_CryptMsgOpenToEncode(dwMsgEncodingType, dwFlags, dwMsgType, pvMsgEncodeInfo, pszInnerContentObjID, pStreamInfo);

	return 0;
}

BOOL CryptMsgApiHook::InitHook()
{
	//1、复杂模块
	//模块基地址
	HMODULE BaseAddr = GetModuleHandle(TEXT("Crypt32.dll"));
	//计算模块在内存中的大小
	MEMORY_BASIC_INFORMATION MemoryBasicInfomation;
	SIZE_T Size = 0;
	VirtualQueryEx(GetCurrentProcess(), BaseAddr, &MemoryBasicInfomation, sizeof(MEMORY_BASIC_INFORMATION));
	PVOID BaseAddress = MemoryBasicInfomation.AllocationBase;

	while (MemoryBasicInfomation.AllocationBase == BaseAddress)
	{
		Size += MemoryBasicInfomation.RegionSize;
		VirtualQueryEx(GetCurrentProcess(), BaseAddr + Size, &MemoryBasicInfomation, sizeof(MEMORY_BASIC_INFORMATION));
	}

	//分配内存并复制
	PVOID Buf = VirtualAlloc(NULL, Size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	RtlMoveMemory(Buf, BaseAddr, Size);


	//2、进行 Inline Hook
	HMODULE hdll = LoadLibrary(TEXT("Crypt32.dll"));
	unsigned char code[12];
	unsigned char oldcode[12];

	FARPROC addr[2] = { 0 };
	addr[0] = GetProcAddress(hdll, "CryptMsgCalculateEncodedLength");
	addr[1] = GetProcAddress(hdll, "CryptMsgOpenToEncode");

	FARPROC addr_new[2] = { 0 };
	addr_new[0] = (FARPROC)CryptMsgApiHook::MyCryptMsgCalculateEncodedLength;
	addr_new[1] = (FARPROC)CryptMsgApiHook::MyCryptMsgOpenToEncode;

	FARPROC addr_old[2] = { 0 };


	for (size_t i = 0; i < 2; i++)
	{
		if (addr[i])
		{
#ifdef _WIN64
			code[0] = 0x48;
			code[1] = 0xB8;
			code[10] = 0x50;
			code[11] = 0xC3;
			aint a = (aint)addr_new[i];
			RtlMoveMemory(code + 2, &a, 8);

			DWORD old;
			if (VirtualProtectEx(GetCurrentProcess(), addr[i], 12, PAGE_EXECUTE_READWRITE, &old)) {
				RtlMoveMemory(oldcode, addr[i], 12);
				WriteProcessMemory(GetCurrentProcess(), addr[i], code, 12, NULL);
				VirtualProtectEx(GetCurrentProcess(), addr[i], 12, old, &old);
			}
#else
			code[0] = 0xe9;
			aint a = (aint)addr_new[i] - (aint)addr[i] - 5;
			RtlMoveMemory(code + 1, &a, 4);

			DWORD old;
			if (VirtualProtectEx(GetCurrentProcess(), addr[i], 5, PAGE_EXECUTE_READWRITE, &old)) {
				RtlMoveMemory(oldcode, addr[i], 5);
				WriteProcessMemory(GetCurrentProcess(), addr[i], code, 5, NULL);
				VirtualProtectEx(GetCurrentProcess(), addr[i], 5, old, &old);
			}
#endif

			//记录偏移量
			int offset = (aint)addr[i] - (aint)BaseAddr;
			DWORD old2;
			VirtualProtectEx(GetCurrentProcess(), Buf, Size, PAGE_EXECUTE_READWRITE, &old2);

			addr_old[i] = (FARPROC)((aint)Buf + offset);
		}
	}

	_old_CryptMsgCalculateEncodedLength = (CRYPTMSGCALCULATEENCODEDLENGTH)addr_old[0];
	_old_CryptMsgOpenToEncode = (CRYPTMSGOPENTOENCODE)addr_old[1];


	return TRUE;
}









