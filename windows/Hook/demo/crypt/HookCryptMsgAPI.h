#ifndef _HOOKCRYPTMSGAPI_H_
#define _HOOKCRYPTMSGAPI_H_

#pragma once

#include <Windows.h>
#include <wincrypt.h>
#pragma comment(lib,"Crypt32.lib")

typedef DWORD(WINAPI * CRYPTMSGCALCULATEENCODEDLENGTH)(_In_ DWORD dwMsgEncodingType,
	_In_ DWORD dwFlags,
	_In_ DWORD dwMsgType,
	_In_ void const *pvMsgEncodeInfo,
	_In_opt_ LPSTR pszInnerContentObjID,
	_In_ DWORD cbData);

typedef HCRYPTMSG(WINAPI * CRYPTMSGOPENTOENCODE)(_In_ DWORD dwMsgEncodingType,
	_In_ DWORD dwFlags,
	_In_ DWORD dwMsgType,
	_In_ void const *pvMsgEncodeInfo,
	_In_opt_ LPSTR pszInnerContentObjID,
	_In_opt_ PCMSG_STREAM_INFO pStreamInfo
	);
typedef BOOL(WINAPI * CRYPTMSGUPDATE)(_In_ HCRYPTMSG hCryptMsg,
	_In_reads_bytes_opt_(cbData) const BYTE *pbData,
	_In_ DWORD cbData,
	_In_ BOOL fFinal);

typedef BOOL(WINAPI *  CRYPTMSGGETPARAM)(
	_In_ HCRYPTMSG hCryptMsg,
	_In_ DWORD dwParamType,
	_In_ DWORD dwIndex,
	_Out_writes_bytes_to_opt_(*pcbData, *pcbData) void *pvData,
	_Inout_ DWORD *pcbData);



#ifdef _WIN64
#define aint unsigned long long
#else
#define aint DWORD
#endif

class HookCryptMsgAPI
{
public:
	HookCryptMsgAPI();
	~HookCryptMsgAPI();

public:
	static CRYPTMSGCALCULATEENCODEDLENGTH _old_CryptMsgCalculateEncodedLength ;

	static CRYPTMSGOPENTOENCODE _old_CryptMsgOpenToEncode;

	static CRYPTMSGUPDATE _old_CryptMsgUpdate;

	static CRYPTMSGGETPARAM _old_CryptMsgGetParam ;


	static HCRYPTMSG _hCryptMsg;
	static DWORD _dwMsgType;
	static CMSG_SIGNED_ENCODE_INFO _signedMsgEncodeInfo;
	static BYTE *_pbSignature;
	static DWORD _dwSignLen;


	static DWORD WINAPI MyCryptMsgCalculateEncodedLength(
		_In_ DWORD dwMsgEncodingType,
		_In_ DWORD dwFlags,
		_In_ DWORD dwMsgType,
		_In_ void const *pvMsgEncodeInfo,
		_In_opt_ LPSTR pszInnerContentObjID,
		_In_ DWORD cbData
	);

	static HCRYPTMSG WINAPI MyCryptMsgOpenToEncode(
		_In_ DWORD dwMsgEncodingType,
		_In_ DWORD dwFlags,
		_In_ DWORD dwMsgType,
		_In_ void const *pvMsgEncodeInfo,
		_In_opt_ LPSTR pszInnerContentObjID,
		_In_opt_ PCMSG_STREAM_INFO pStreamInfo
	);

	static BOOL WINAPI MyCryptMsgUpdate(
		_In_ HCRYPTMSG hCryptMsg,
		_In_reads_bytes_opt_(cbData) const BYTE *pbData,
		_In_ DWORD cbData,
		_In_ BOOL fFinal
	);

	static BOOL WINAPI MyCryptMsgGetParam(
		_In_ HCRYPTMSG hCryptMsg,
		_In_ DWORD dwParamType,
		_In_ DWORD dwIndex,
		_Out_writes_bytes_to_opt_(*pcbData, *pcbData) void *pvData,
		_Inout_ DWORD *pcbData);

	static BOOL CryptMsgApiHook();


	static BOOL Signed_PKCS7(__in  HCRYPTPROV  hProv, __in DWORD dwKeySpec, __in ALG_ID Algid,
		CONST BYTE  *pbData, __in DWORD dwDataLen,
		__out BYTE *pbSignature, __inout DWORD *pdwSigLen);
};



#endif