#include "HookCryptMsgAPI.h"
#include <stdio.h>

#include "Pkcs7WithOpenssl.h"

CRYPTMSGCALCULATEENCODEDLENGTH HookCryptMsgAPI::_old_CryptMsgCalculateEncodedLength = 0;
CRYPTMSGUPDATE HookCryptMsgAPI::_old_CryptMsgUpdate = 0;
CRYPTMSGOPENTOENCODE HookCryptMsgAPI::_old_CryptMsgOpenToEncode = 0;
CRYPTMSGGETPARAM HookCryptMsgAPI::_old_CryptMsgGetParam = 0;

HCRYPTMSG HookCryptMsgAPI::_hCryptMsg = 0;
DWORD HookCryptMsgAPI::_dwMsgType = 0;
CMSG_SIGNED_ENCODE_INFO HookCryptMsgAPI::_signedMsgEncodeInfo = {0};
BYTE *HookCryptMsgAPI::_pbSignature = 0;
DWORD HookCryptMsgAPI::_dwSignLen = 0;

#include <openssl/sha.h>


BOOL FindCertificateBySN(BYTE *byCertSN, DWORD dwLen, BYTE *byCert, int* certLen)
{
	HCERTSTORE hstore;
	hstore = CertOpenSystemStore(0, "MY");
	if (!hstore)
		return FALSE;
	
	PCCERT_CONTEXT  pCertContext = NULL;

	long found = 0;
	
	while (pCertContext = CertEnumCertificatesInStore(hstore,pCertContext)) 
	{
		if ((0 == memcmp(pCertContext->pCertInfo->SerialNumber.pbData, byCertSN, dwLen))
			&& dwLen == pCertContext->pCertInfo->SerialNumber.cbData)
		{
			memcpy(byCert, pCertContext->pbCertEncoded, pCertContext->cbCertEncoded);
			*certLen = pCertContext->cbCertEncoded;
			found = 1;
			break;
		}
	}

	if (hstore != NULL)
		CertCloseStore(hstore, CERT_CLOSE_STORE_FORCE_FLAG);

	if (found)
		return TRUE;

	return FALSE;
}

BOOL HookCryptMsgAPI::Signed_PKCS7(__in  HCRYPTPROV  hProv, __in DWORD dwKeySpec, __in ALG_ID Algid,
	CONST BYTE  *pbData, __in DWORD dwDataLen,
	__out BYTE *pbSignature, __inout DWORD *pdwSigLen)
{
	const BYTE  *pPlainText = NULL;
	BYTE  *pbDataTmp = NULL;
	BYTE  *pbSignatureTmp = NULL;

	BOOL ret = TRUE;
	HCRYPTHASH  phHash = 0;
	*pdwSigLen = 0;

	unsigned char mdStr[33] = { 0 };
	
	BYTE tmpCert[8192] = {0};

	int certLen = sizeof(tmpCert);

	if (_signedMsgEncodeInfo.cbSize > 0)
	{
		CMSG_SIGNER_ENCODE_INFO signerEncodeInfo = HookCryptMsgAPI::_signedMsgEncodeInfo.rgSigners[0];
		ret = FindCertificateBySN(signerEncodeInfo.pCertInfo->SerialNumber.pbData, signerEncodeInfo.pCertInfo->SerialNumber.cbData,
			tmpCert, &certLen);
	}

	do
	{
		if (!ret)
			break;

		if (!CryptCreateHash(hProv, Algid, 0, 0, &phHash))
		{
			ret = FALSE;
			break;
		}
		if (NULL == pbData)
		{
			pbDataTmp = (BYTE  *)calloc(dwDataLen, 1);
			if (pbDataTmp)
			{
				SHA256((const unsigned char *)pbDataTmp, dwDataLen, mdStr);
				ret = CryptHashData(phHash, pbDataTmp, dwDataLen, 0);

				pPlainText = pbDataTmp;
			}
			else		
				ret = FALSE;
		}
		else
		{
			
			SHA256((const unsigned char *)pbData, dwDataLen, mdStr);

			ret = CryptHashData(phHash, pbData, dwDataLen, 0);

			pPlainText = pbData;
		}
			

		if (! ret)
		{
			ret = FALSE;
			break;
		}

		if (!CryptSignHash(phHash, dwKeySpec, NULL, 0, NULL, pdwSigLen))
		{
			ret = FALSE;
			break;
		}

		if (NULL != pbSignature)
		{
			if (!CryptSignHash(phHash, dwKeySpec, NULL, 0, pbSignature, pdwSigLen))
			{
				ret = FALSE;
				break;
			}

			ret = SOF_MakeSignDataToPKCS7(0, 0, (BYTE  *)pPlainText, dwDataLen, pbSignature, *pdwSigLen, tmpCert, certLen, pbSignature, pdwSigLen);
			if (0 == ret)
				ret = TRUE;
		}
		else
		{
			pbSignatureTmp = (BYTE  *)calloc(*pdwSigLen, 1);
			if (pbSignatureTmp)
			{
				ret = SOF_MakeSignDataToPKCS7(0, 0, (BYTE  *)pPlainText, dwDataLen, pbSignatureTmp, *pdwSigLen, tmpCert, certLen, pbSignature, pdwSigLen);
			
				if (0 == ret)
					ret = TRUE;
			}
			else
				ret = FALSE;
		}

	} while (0);

	if (pbSignatureTmp)
		free(pbSignatureTmp), pbSignatureTmp = NULL;

	if (pbDataTmp)
		free(pbDataTmp), pbDataTmp = NULL;

	if (phHash)
		CryptDestroyHash(phHash);

	return ret;
}


HookCryptMsgAPI::HookCryptMsgAPI()
{
}


HookCryptMsgAPI::~HookCryptMsgAPI()
{
}

DWORD WINAPI HookCryptMsgAPI::MyCryptMsgCalculateEncodedLength(
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
	//CERT_BLOB signerCertBlob = signedMsgEncodeInfo->rgCertEncoded[0];
	//signerCertBlob.cbData;
	//signerCertBlob.pbData;
	//signerEncodeInfo.pCertInfo;
	HCRYPTPROV hCryptProv = signerEncodeInfo.hCryptProv;
	DWORD dwKeySpec = signerEncodeInfo.dwKeySpec;
	CRYPT_ALGORITHM_IDENTIFIER hashAlgorithm = signerEncodeInfo.HashAlgorithm;

	//CRYPT_INTEGER_BLOB* signerEncodeInfo.pCertInfo->SerialNumber

	//dwMsgEncodingType = (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING);
	//dwFlags = 0;
	//cbData; //数据长度

	if (dwMsgType == CMSG_SIGNED &&
		0 == memcmp(hashAlgorithm.pszObjId, (LPSTR)szOID_RSA_SHA256RSA, strlen(szOID_RSA_SHA256RSA)))
	{
		_hCryptMsg = 0;
		_dwMsgType = CMSG_SIGNED;
		memcpy(&_signedMsgEncodeInfo, pvMsgEncodeInfo, sizeof(CMSG_SIGNED_ENCODE_INFO));
		
		Signed_PKCS7(hCryptProv, dwKeySpec, CALG_SHA_256, NULL, cbData, NULL, &dwRetLen);
	}
	else
	{
		//如果不是访问我们的进程则去 Call 复制的 模块
		if (_old_CryptMsgCalculateEncodedLength)
			return  _old_CryptMsgCalculateEncodedLength(dwMsgEncodingType, dwFlags, dwMsgType, pvMsgEncodeInfo, pszInnerContentObjID, cbData);
	}

	return dwRetLen;
}


HCRYPTMSG WINAPI HookCryptMsgAPI::MyCryptMsgOpenToEncode(
	_In_ DWORD dwMsgEncodingType,
	_In_ DWORD dwFlags,
	_In_ DWORD dwMsgType,
	_In_ void const *pvMsgEncodeInfo,
	_In_opt_ LPSTR pszInnerContentObjID,
	_In_opt_ PCMSG_STREAM_INFO pStreamInfo
)
{
	printf("MyCryptMsgOpenToEncode\n");

	HCRYPTMSG hmsg = 0;
	if (_old_CryptMsgOpenToEncode)
	{
		if (0 == (hmsg = _old_CryptMsgOpenToEncode(dwMsgEncodingType, dwFlags, dwMsgType, pvMsgEncodeInfo, pszInnerContentObjID, pStreamInfo)))
		{
			return hmsg;
		}

		CMSG_SIGNED_ENCODE_INFO* signedMsgEncodeInfo = (CMSG_SIGNED_ENCODE_INFO*)pvMsgEncodeInfo;
		CMSG_SIGNER_ENCODE_INFO signerEncodeInfo = signedMsgEncodeInfo->rgSigners[0];
		CRYPT_ALGORITHM_IDENTIFIER hashAlgorithm = signerEncodeInfo.HashAlgorithm;
		if (CMSG_SIGNED == dwMsgType &&
			0 == memcmp(hashAlgorithm.pszObjId, (LPSTR)szOID_RSA_SHA256RSA, strlen(szOID_RSA_SHA256RSA)))
		{
			_hCryptMsg = 0;
			_dwMsgType = 0;
			memset(&_signedMsgEncodeInfo, 0, sizeof(CMSG_SIGNED_ENCODE_INFO));

			_hCryptMsg = hmsg;
			_dwMsgType = CMSG_SIGNED;
			memcpy(&_signedMsgEncodeInfo, pvMsgEncodeInfo, sizeof(CMSG_SIGNED_ENCODE_INFO));
		}
	}
	return hmsg;
}



BOOL WINAPI HookCryptMsgAPI::MyCryptMsgUpdate(
	_In_ HCRYPTMSG hCryptMsg,
	_In_reads_bytes_opt_(cbData) const BYTE *pbData,
	_In_ DWORD cbData,
	_In_ BOOL fFinal
)
{
	printf("MyCryptMsgUpdate\n");
	if (_hCryptMsg == hCryptMsg && _dwMsgType == CMSG_SIGNED)
	{
		CMSG_SIGNER_ENCODE_INFO signerEncodeInfo = _signedMsgEncodeInfo.rgSigners[0];
		CRYPT_ALGORITHM_IDENTIFIER hashAlgorithm = signerEncodeInfo.HashAlgorithm;
		if (0 == memcmp(hashAlgorithm.pszObjId, (LPSTR)szOID_RSA_SHA256RSA, strlen(szOID_RSA_SHA256RSA)))
		{
			if (fFinal == TRUE)
			{
				DWORD dwRetLen = cbData * 2;
				HCRYPTPROV hCryptProv = signerEncodeInfo.hCryptProv;
				DWORD dwKeySpec = signerEncodeInfo.dwKeySpec;

				//加锁
				Signed_PKCS7(hCryptProv, dwKeySpec, CALG_SHA_256, pbData, cbData, NULL, &dwRetLen);

				//只在此处释放申请的内存
				if (NULL != _pbSignature)
					free(_pbSignature), _pbSignature = NULL;

				_pbSignature = (BYTE*)calloc(dwRetLen + 1, 1);
				if (_pbSignature)
				{
					_dwSignLen = dwRetLen;
					Signed_PKCS7(hCryptProv, dwKeySpec, CALG_SHA_256, pbData, cbData, _pbSignature, &dwRetLen);
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
		}
	}

	if (_old_CryptMsgUpdate)
		return _old_CryptMsgUpdate(hCryptMsg, pbData, cbData, fFinal);
	else
		return FALSE;
}


BOOL WINAPI HookCryptMsgAPI::MyCryptMsgGetParam(
	_In_ HCRYPTMSG hCryptMsg,
	_In_ DWORD dwParamType,
	_In_ DWORD dwIndex,
	_Out_writes_bytes_to_opt_(*pcbData, *pcbData) void *pvData,
	_Inout_ DWORD *pcbData)
{
	printf("MyCryptMsgGetParam\n");
	if (_hCryptMsg == hCryptMsg &&
		_dwMsgType == CMSG_SIGNED &&
		dwParamType == CMSG_CONTENT_PARAM)
	{
		if (NULL != _pbSignature)
		{
			if (pcbData && *pcbData >= _dwSignLen)
			{				
				*pcbData = _dwSignLen;
				if (pvData)
				{
					memcpy(pvData, _pbSignature, _dwSignLen);
					//清除头标志
					_hCryptMsg = 0;
					_dwMsgType = 0;
					memset(&_signedMsgEncodeInfo, 0, sizeof(CMSG_SIGNED_ENCODE_INFO));
					return TRUE;
				}
			}
		}
	}

	if (_old_CryptMsgGetParam)
		return _old_CryptMsgGetParam(hCryptMsg, dwParamType, dwIndex, pvData, pcbData);

	return FALSE;
}




BOOL HookCryptMsgAPI::CryptMsgApiHook()
{
	printf("CryptMsgApiHook\n");
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

	FARPROC addr[4] = { 0 };
	addr[0] = GetProcAddress(hdll, "CryptMsgCalculateEncodedLength");
	addr[1] = GetProcAddress(hdll, "CryptMsgOpenToEncode");
	addr[2] = GetProcAddress(hdll, "CryptMsgUpdate");
	addr[3] = GetProcAddress(hdll, "CryptMsgGetParam");

	FARPROC addr_new[4] = { 0 };
	addr_new[0] = (FARPROC)HookCryptMsgAPI::MyCryptMsgCalculateEncodedLength;
	addr_new[1] = (FARPROC)HookCryptMsgAPI::MyCryptMsgOpenToEncode;
	addr_new[2] = (FARPROC)HookCryptMsgAPI::MyCryptMsgUpdate;
	addr_new[3] = (FARPROC)HookCryptMsgAPI::MyCryptMsgGetParam;

	FARPROC addr_old[4] = {0};
	

	for (size_t i = 0; i < 4; i++)
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
	_old_CryptMsgUpdate = (CRYPTMSGUPDATE)addr_old[2];
	_old_CryptMsgGetParam = (CRYPTMSGGETPARAM)addr_old[3];


	return TRUE;
}
