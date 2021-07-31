#ifndef _CRYPTMSGAPIHOOK_H_
#define _CRYPTMSGAPIHOOK_H_

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




#ifdef _WIN64
#define aint unsigned long long
#else
#define aint DWORD
#endif


class CryptMsgApiHook
{
public:
	CryptMsgApiHook();
	~CryptMsgApiHook();

public:
	static CRYPTMSGCALCULATEENCODEDLENGTH _old_CryptMsgCalculateEncodedLength;

	static CRYPTMSGOPENTOENCODE _old_CryptMsgOpenToEncode;

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

	static BOOL InitHook();
};


#endif