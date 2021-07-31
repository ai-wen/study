#pragma once
class Pkcs7WithOpenssl
{
public:
	Pkcs7WithOpenssl();
	~Pkcs7WithOpenssl();


};

#include <windows.h>

ULONG SOF_MakeSignDataToPKCS7(ULONG ulDetached, ULONG digestMethod/*SHA256*/,
	BYTE * hashData, ULONG hashDataLen,
	BYTE *signData, ULONG signDatalen,
	BYTE	*pcert, ULONG certLen,
	BYTE *OutData, ULONG *OutDataLen);