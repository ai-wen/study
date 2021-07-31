/******************************************************************************
 *
 * Copyright (c) 2009 Antillia.com TOSHIYUKI ARAI. ALL RIGHTS RESERVED.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer.
 *
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 *  CryptMsg.h
 *
 *****************************************************************************/

 // SOL9
 // 2009/03/18 Added two methods encodStream and decodeStream.
 // 2009/03/23 Added two methods sign and envelop.
 // 2009/03/24 Added open, upate, close, counterSign and control methods.

#pragma once

#include <Windows.h>
#include <wincrypt.h>
#pragma comment(lib,"Crypt32.lib")

class CryptMsg  {

private:
	HCRYPTMSG hCryptMsg;

public:
	CryptMsg()
		:hCryptMsg(NULL)
	{

	}

public:
	CryptMsg(HCRYPTMSG hMsg)
		:hCryptMsg(hMsg)
	{

	}

public:
	//2009/03/24
	int open(DWORD encodingType = (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING))
	{
		int rc = NO_ERROR;
		close();

		if ((this->hCryptMsg = CryptMsgOpenToDecode(encodingType, 0, 0, 0, NULL, NULL)) == NULL) {
			rc = GetLastError();
		}
		return rc;
	}

public:
	//2009/03/24

	int update(const BYTE* data, DWORD dataSize, BOOL final)
	{
		int rc = NO_ERROR;

		if (!CryptMsgUpdate(this->hCryptMsg, data, dataSize, final)) {
			rc = GetLastError();
		}
		return rc;
	}

public:
	//2009/03/24

	void close() {
		if (this->hCryptMsg != NULL) {
			CryptMsgClose(this->hCryptMsg);
			this->hCryptMsg = NULL;
		}
	}

public:
	~CryptMsg() {
		close();
	}

public:
	//2009/03/26
	HCRYPTMSG  getCryptMsg() {
		return this->hCryptMsg;
	}

public:
	//2009/03/25
	void setCryptMsg(HCRYPTMSG hMsg) {
		this->hCryptMsg = hMsg;
	}


public:

	//2009/03/24

	int counterSign(
		__in DWORD dwIndex,
		__in DWORD cCountersigners,
		__in PCMSG_SIGNER_ENCODE_INFO rgCountersigners)
	{
		int rc = NO_ERROR;
		if (!CryptMsgCountersign(
			this->hCryptMsg,
			dwIndex,
			cCountersigners,
			rgCountersigners)) {
			rc = GetLastError();
		}
		return rc;
	}

public:
	//2009/03/24
	int control(
		__in DWORD dwFlags,
		__in DWORD dwCtrlType,
		__in const void *pvCtrlPara)
	{
		int rc = NO_ERROR;
		if (!CryptMsgControl(
			this->hCryptMsg,
			dwFlags,
			dwCtrlType,
			pvCtrlPara)) {
			rc = GetLastError();
		}
		return rc;
	}

private:
	//This is a static callback function, which can be called
	//from the decodeStream method

	static BOOL WINAPI encodeCallback(
		const void *pvArg,
		BYTE *pbData,
		DWORD cbData,
		BOOL fFinal)
	{
		BOOL rc = TRUE;
		DWORD writtenBytes = 0;// -1;

		//Write encoded data to the file of outFileName    

		HANDLE hFile = *((HANDLE *)pvArg);
		if (!WriteFile(
			hFile,
			pbData,
			cbData,
			&writtenBytes,
			NULL) ||
			(writtenBytes != cbData)) {
			rc = FALSE;
		}
		return rc;
	}


public:
	/**
		*/
		//2009/03/18 
	int encodeStream(
		__in PCCERT_CONTEXT pSignerCert,
		__in const TCHAR* inFileName,
		__in const TCHAR* outFileName,
		__in_opt DWORD encodingType = (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING),
		__in_opt DWORD flags = 0,
		__in_opt DWORD messageType = CMSG_SIGNED
	)
	{
		int rc = NO_ERROR;
		//printf("Start encodeStream in=%s out=%s\n", inFileName, outFileName);

		HCRYPTPROV hCryptProv = NULL;
		DWORD dwKeySpec = 0;

		// Get a handle to a cryptographic provider from pSignerCert. 
		if (!(CryptAcquireCertificatePrivateKey(
			pSignerCert,
			0,
			NULL,
			&hCryptProv,
			&dwKeySpec,
			NULL))) {

			printf("Error:CryptAcquireCertificatePrivateKey() failed %x=\n",
				GetLastError());
			return GetLastError();
		}

		//printf("OK. CryptAcquireCertificatePrivateKey\n");

		HANDLE hInFile = CreateFile(
			inFileName,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hInFile == INVALID_HANDLE_VALUE) {
			CryptReleaseContext(hCryptProv, 0);
			return GetLastError();
		}

		HANDLE hOutFile = CreateFile(
			outFileName,
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hOutFile == INVALID_HANDLE_VALUE) {
			CloseHandle(hInFile);
			CryptReleaseContext(hCryptProv, 0);
			return GetLastError();
		}

		CMSG_SIGNER_ENCODE_INFO signerEncodeInfo;

		memset(&signerEncodeInfo, 0, sizeof(CMSG_SIGNER_ENCODE_INFO));
		signerEncodeInfo.cbSize = sizeof(CMSG_SIGNER_ENCODE_INFO);
		signerEncodeInfo.pCertInfo = pSignerCert->pCertInfo;
		signerEncodeInfo.hCryptProv = hCryptProv;
		signerEncodeInfo.dwKeySpec = dwKeySpec;
		signerEncodeInfo.HashAlgorithm.pszObjId = (LPSTR)szOID_RSA_MD5;
		signerEncodeInfo.pvHashAuxInfo = NULL;

		CMSG_SIGNER_ENCODE_INFO signerEncodeInfoArray[1];
		signerEncodeInfoArray[0] = signerEncodeInfo;

		CERT_BLOB signerCertBlob;
		signerCertBlob.cbData = pSignerCert->cbCertEncoded;

		signerCertBlob.pbData = pSignerCert->pbCertEncoded;
		CERT_BLOB signerCertBlobArray[1];
		signerCertBlobArray[0] = signerCertBlob;

		CMSG_SIGNED_ENCODE_INFO signedMsgEncodeInfo;
		memset(&signedMsgEncodeInfo, 0, sizeof(CMSG_SIGNED_ENCODE_INFO));
		signedMsgEncodeInfo.cbSize = sizeof(CMSG_SIGNED_ENCODE_INFO);
		signedMsgEncodeInfo.cSigners = 1;
		signedMsgEncodeInfo.rgSigners = signerEncodeInfoArray;
		signedMsgEncodeInfo.cCertEncoded = 1;
		signedMsgEncodeInfo.rgCertEncoded = signerCertBlobArray;

		CMSG_STREAM_INFO streamInfo;
		memset(&streamInfo, 0, sizeof(streamInfo));

		// BER_ENCODING
		streamInfo.cbContent = 0xffffffff;
		streamInfo.pfnStreamOutput = encodeCallback;
		// Set the output file handle to pvArg  
		streamInfo.pvArg = &hOutFile;

		// Open a message to encode.
		if (!(hCryptMsg == CryptMsgOpenToEncode(
			encodingType,
			flags,
			messageType,  //CMSG_SIGNED
			&signedMsgEncodeInfo,
			NULL,
			&streamInfo))) {

			//Error
			printf("Error: CryptMsgOpenToEncode %x\n", GetLastError());

			CloseHandle(hInFile);
			CloseHandle(hOutFile);

			return GetLastError();
		}

		const DWORD bufferSize = 256;

		byte buffer[bufferSize];
		DWORD readSize = 0;
		BOOL isLast = FALSE;

		while (ReadFile(hInFile,
			buffer,
			bufferSize,
			&readSize, NULL)) {

			if (readSize < bufferSize) {
				isLast = TRUE;
			}

			if (!(CryptMsgUpdate(
				this->hCryptMsg,
				buffer,
				readSize,
				isLast
			))) {
				//Error  
				printf("Encode MsgUpdate failed.\n");

				rc = GetLastError();
				break;
			}
			if (isLast) {
				break;
			}
		}
		CryptReleaseContext(hCryptProv, 0);
		CloseHandle(hOutFile);
		CloseHandle(hInFile);

		//printf("End, encodeStream\n");
		return rc;
	}


private:
	//This is a static callback function, which can be called
	//from the decodeStream method

	static BOOL WINAPI decodeCallback(
		const void *pvArg,
		BYTE *pbData,
		DWORD cbData,
		BOOL fFinal)
	{
		BOOL rc = TRUE;
		DWORD writtenBytes = 0; //-1;
		//Write decoded data to the file of outFileName    
		HANDLE hFile = *((HANDLE *)pvArg);
		if (!WriteFile(
			hFile,
			pbData,
			cbData,
			&writtenBytes,
			NULL) ||
			(writtenBytes != cbData)) {
			rc = FALSE;
		}
		return rc;
	}


public:
	/**
		*
		*/
		//2009/03/18
	int decodeStream(
		__in const TCHAR* inFileName,
		__in const TCHAR* outFileName,
		__in_opt DWORD encodingType = (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING),
		__in_opt DWORD flags = 0,
		__in_opt DWORD messageType = 0,
		__in_opt HCRYPTPROV hCryptProv = NULL //NULL indidates the default provider
	)
	{
		int rc = NO_ERROR;

		//printf("Start, decodeStream in=%s out=%s\n", inFileName, outFileName);

		HANDLE hInFile = CreateFile(
			inFileName,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hInFile == INVALID_HANDLE_VALUE) {
			return GetLastError();
		}

		//printf("OK, Opened a file %s\n", inFileName);

		HANDLE hOutFile = CreateFile(
			outFileName,
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hOutFile == INVALID_HANDLE_VALUE) {
			CloseHandle(hInFile);
			printf(("Failed to open a file:%s\n"), outFileName);
			return GetLastError();
		}
		//printf("OK, Opened an outpuf file %s\n", outFileName);

		CMSG_STREAM_INFO streamInfo;
		memset(&streamInfo, 0, sizeof(streamInfo));

		streamInfo.cbContent = 0xffffffff;
		streamInfo.pfnStreamOutput = decodeCallback;

		streamInfo.pvArg = &hOutFile;
		if (!(hCryptMsg == CryptMsgOpenToDecode(
			encodingType,
			flags,
			messageType,
			hCryptProv,
			NULL,
			&streamInfo))) {
			//Failed 
			printf("Failed, CryptMsgOpenToDecode %x\n", GetLastError());

			CloseHandle(hInFile);
			CloseHandle(hOutFile);

			return GetLastError();
		}

		//printf("OK, CryptMsgOpenToDecode\n");

		const DWORD bufferSize = 256;

		byte buffer[bufferSize];
		DWORD readSize;
		BOOL isLast = FALSE;

		while (ReadFile(hInFile,
			buffer,
			bufferSize,
			&readSize, NULL)) {

			if (readSize < bufferSize) {
				isLast = TRUE;
			}

			if (!(CryptMsgUpdate(
				hCryptMsg,
				buffer,
				readSize,  // size of the encoded BLOB
				isLast     // last call
			))) {

				printf("Decode MsgUpdate failed.\n");
				rc = GetLastError();
				break;
			}
			if (isLast) {
				break;
			}
		}


		CloseHandle(hOutFile);
		CloseHandle(hInFile);

		//printf("End, decodeStream\n");

		return rc;
	}



public:
	int getCertificate(__in DWORD index,
		__out PCCERT_CONTEXT* ppCertContext)
	{
		int rc = NO_ERROR;

		BYTE* certData = NULL;
		size_t certDataSize = 0;
		if ((rc = getCertificate(index, &certData, &certDataSize)) == NO_ERROR) {

			*ppCertContext = CertCreateCertificateContext(
				X509_ASN_ENCODING,
				certData,
				certDataSize);
		}
		return rc;
	}


public:
	int getCertificate(__in DWORD index,
		__out BYTE** certData,
		__out size_t* certDataSize)
	{
		int rc = NO_ERROR;

		// Get the size of the encoded certificate.
		DWORD size = 0;
		if (CryptMsgGetParam(hCryptMsg,
			CMSG_CERT_PARAM, index,
			NULL, &size)) {

			// OK, allocate the buffer of size for encoded certificate.
			BYTE* pbCert = new BYTE[size];

			if (CryptMsgGetParam(hCryptMsg, CMSG_CERT_PARAM,
				index, pbCert, &size)) {
				//OK
				*certData = pbCert;
				*certDataSize = size;
			}
			else {
				delete[] pbCert;

				rc = GetLastError();
			}

		}
		else {
			rc = GetLastError();
		}
		return rc;
	}

public:
	int getCertCount(__out DWORD* certCount)
	{
		int rc = NO_ERROR;
		DWORD dwNumRead = sizeof(DWORD);

		if (CryptMsgGetParam(hCryptMsg, CMSG_CERT_COUNT_PARAM, 0,
			certCount, &dwNumRead) != TRUE) {
			rc = GetLastError();
		}
		return rc;
	}

public:
	int getSignerInfo(__out PCMSG_SIGNER_INFO* ppInfo)
	{
		int rc = NO_ERROR;
		DWORD size = 0;
		// Get size.
		if (CryptMsgGetParam(hCryptMsg,
			CMSG_SIGNER_INFO_PARAM,
			0,
			NULL,
			&size)) {

			// OK, allocate memory of size
			PCMSG_SIGNER_INFO pInfo = (PCMSG_SIGNER_INFO)new BYTE[size];
			if (CryptMsgGetParam(hCryptMsg,
				CMSG_SIGNER_INFO_PARAM,
				0,
				(PVOID)pInfo,
				&size)) {
				//OK
				*ppInfo = pInfo;
			}
			else {
				delete[] pInfo;
				rc = GetLastError();
			}
		}
		else {
			rc = GetLastError();
		}
		return rc;
	}

public:
	int getRecipientCount(DWORD* count)
	{
		int rc = NO_ERROR;

		DWORD data = 0;
		DWORD size = sizeof(data);

		if (CryptMsgGetParam(
			this->hCryptMsg,
			CMSG_CMS_RECIPIENT_COUNT_PARAM,
			0,
			&data,
			&size)) {

			*count = data;
		}
		else {
			rc = GetLastError();
		}
		return rc;
	}

public:
	int getRecipientInfo(__out BYTE** ppInfo, __out size_t* infoSize)
	{
		int rc = NO_ERROR;
		DWORD size = 0;
		// Get size.
		if (CryptMsgGetParam(
			this->hCryptMsg,
			CMSG_RECIPIENT_INFO_PARAM,
			0,
			NULL,
			&size)) {
			// OK, allocate memory of size
			BYTE* pInfo = new BYTE[size];
			if (CryptMsgGetParam(
				this->hCryptMsg,
				CMSG_RECIPIENT_INFO_PARAM,
				0,
				(PVOID)pInfo,
				&size)) {
				//OK
				*ppInfo = pInfo;
				*infoSize = size;
			}
			else {
				delete[] pInfo;
				rc = GetLastError();
			}
		}
		else {
			rc = GetLastError();
		}
		return rc;
	}


public:
	// 2009/03/23
	int sign(
		__in HCRYPTPROV hCryptProv,
		__in DWORD dwKeySpec,
		__in PCCERT_CONTEXT pSignerCert,
		__in DWORD encodingType,
		__in DWORD dwFlags,
		__in BYTE* pbContent,
		__in DWORD cbContent,

		__out BYTE** ppSignedData,
		__out DWORD* pSignedDataSize)
	{
		int rc = NO_ERROR;

		// Initialize CRYPT_ALGORITHM_IDENTIFIER.
		CRYPT_ALGORITHM_IDENTIFIER hashAlgorithm;
		memset(&hashAlgorithm, 0, sizeof(hashAlgorithm));
		//hashAlgorithm.pszObjId = (LPSTR)szOID_RSA_MD5;
		//hashAlgorithm.pszObjId = (LPSTR)szOID_RSA_SHA1RSA;
		hashAlgorithm.pszObjId = (LPSTR)szOID_RSA_SHA256RSA;
		//memcpy(&hashAlgorithm, &pSignerCert->pCertInfo->SignatureAlgorithm, sizeof(CRYPT_ALGORITHM_IDENTIFIER));

		// Initialize CMSG_SIGNER_ENCODE_INFO 
		CMSG_SIGNER_ENCODE_INFO signerEncodeInfo;
		memset(&signerEncodeInfo, 0, sizeof(signerEncodeInfo));
		signerEncodeInfo.cbSize = sizeof(signerEncodeInfo);
		signerEncodeInfo.pCertInfo = pSignerCert->pCertInfo;
		signerEncodeInfo.hCryptProv = hCryptProv;
		signerEncodeInfo.dwKeySpec = dwKeySpec;  //AT_KEYEXCHANGE;
		signerEncodeInfo.HashAlgorithm = hashAlgorithm;
		signerEncodeInfo.pvHashAuxInfo = NULL;

		// For only a single signer.
		CMSG_SIGNER_ENCODE_INFO signerEncodeInfoArray[1];
		signerEncodeInfoArray[0] = signerEncodeInfo;

		// Initialize CMSG_SIGNED_ENCODE_INFO.
		CERT_BLOB signerCertBlob;
		signerCertBlob.cbData = pSignerCert->cbCertEncoded;
		signerCertBlob.pbData = pSignerCert->pbCertEncoded;

		CERT_BLOB signerCertBlobArray[1];
		signerCertBlobArray[0] = signerCertBlob;

		CMSG_SIGNED_ENCODE_INFO signedMsgEncodeInfo;
		memset(&signedMsgEncodeInfo, 0, sizeof(signedMsgEncodeInfo));
		signedMsgEncodeInfo.cbSize = sizeof(signedMsgEncodeInfo);
		signedMsgEncodeInfo.cSigners = 1;
		signedMsgEncodeInfo.rgSigners = signerEncodeInfoArray;
		signedMsgEncodeInfo.cCertEncoded = 1;
		signedMsgEncodeInfo.rgCertEncoded = signerCertBlobArray;
		signedMsgEncodeInfo.rgCrlEncoded = NULL;
		//printf("cbContent=%d\n", cbContent);

		
		// Get the size of signed message.
		DWORD cbSignedBlob = 0;

		if ((cbSignedBlob = CryptMsgCalculateEncodedLength(
			encodingType,
			dwFlags,
			CMSG_SIGNED,
			&signedMsgEncodeInfo,
			NULL,
			//(LPSTR)szOID_RSA_signedData,
			cbContent)) <= 0) {

			printf("Error1 %x\n", GetLastError());

			//Error
			return GetLastError();
		}
		

	//	DWORD cbSignedBlob = 4096;
		//getchar();
		// Open a message.
		if ((this->hCryptMsg = CryptMsgOpenToEncode(
			encodingType,
			dwFlags,
			CMSG_SIGNED,
			&signedMsgEncodeInfo,
			NULL,
			NULL)) == NULL) {

			printf("Error2 %x\n", GetLastError());

			// Error
			return GetLastError();
		}

		// Update the message

		if (!CryptMsgUpdate(
			this->hCryptMsg,
			pbContent,
			cbContent,
			TRUE)) {
			printf("Error3 %x\n", GetLastError());

			//Error close(); this->hCryptMsg
			return GetLastError();
		}

		BYTE* pbSignedBlob = (BYTE *)new BYTE[cbSignedBlob];

		// Get the content.

		if (CryptMsgGetParam(
			this->hCryptMsg,
			CMSG_CONTENT_PARAM,
			0,
			pbSignedBlob,
			&cbSignedBlob)) {

			FILE* fp = fopen("cer.bin", "wb");
			fwrite(pbSignedBlob, 1, cbSignedBlob, fp);
			fclose(fp);
			//OK
			*ppSignedData = pbSignedBlob;
			*pSignedDataSize = cbSignedBlob;
		}
		else {
			printf("Error4 %x\n", GetLastError());

			delete[] pbSignedBlob;
			rc = GetLastError();
		}
		return rc;
	}


public:
	//2009/03/23
	int envelop(
		__in HCRYPTPROV hCryptProv,

		__in PCCERT_CONTEXT pRecipientCert,  // receiver's certificate
		__in DWORD encodingType,
		__in DWORD dwFlags,

		__in BYTE* pbSignedBlob,
		__in DWORD cbSignedBlob,

		__out BYTE** ppEnvelopedData,
		__out DWORD* pEnvelopedDataSize)
	{
		int rc = NO_ERROR;

		//DWORD contentEncryptAlgSize;
		CRYPT_ALGORITHM_IDENTIFIER contentEncryptAlgorithm;

		// Only a single recipient.
		PCERT_INFO recipientCertArray[1];

		recipientCertArray[0] = pRecipientCert->pCertInfo;

		memset(&contentEncryptAlgorithm, 0, sizeof(contentEncryptAlgorithm));

		// Initialize the necessary members. This particular OID does not
		contentEncryptAlgorithm.pszObjId = (LPSTR)szOID_RSA_RC4;

		// Initialize the CMSG_ENVELOPED_ENCODE_INFO structure.
		CMSG_ENVELOPED_ENCODE_INFO envelopedEncodeInfo;

		memset(&envelopedEncodeInfo, 0, sizeof(envelopedEncodeInfo));

		envelopedEncodeInfo.cbSize = sizeof(envelopedEncodeInfo);

		envelopedEncodeInfo.hCryptProv = hCryptProv;
		envelopedEncodeInfo.ContentEncryptionAlgorithm = contentEncryptAlgorithm;
		envelopedEncodeInfo.pvEncryptionAuxInfo = NULL;
		envelopedEncodeInfo.cRecipients = 1;
		envelopedEncodeInfo.rgpRecipients = recipientCertArray;

		// Get the size of the encoded message BLOB.

		DWORD cbEncodedBlob;

		if ((cbEncodedBlob = CryptMsgCalculateEncodedLength(
			encodingType,
			dwFlags,
			CMSG_ENVELOPED,
			&envelopedEncodeInfo,
			(LPSTR)szOID_RSA_signedData,
			cbSignedBlob)) <= 0) {
			// Error
			return GetLastError();
		}

		BYTE* pbEncodedBlob = new BYTE[cbEncodedBlob];

		if ((hCryptMsg = CryptMsgOpenToEncode(
			encodingType,
			dwFlags,
			CMSG_ENVELOPED,
			&envelopedEncodeInfo,
			(LPSTR)szOID_RSA_signedData,
			NULL)) = NULL) {
			//Error
			delete[] pbEncodedBlob;
			return GetLastError();
		}

		// Update the message
		if (!CryptMsgUpdate(
			hCryptMsg,
			pbSignedBlob,
			cbSignedBlob,
			TRUE)) {
			//Error
			delete[] pbEncodedBlob;
			return GetLastError();
		}

		// Get the content.

		if (CryptMsgGetParam(
			hCryptMsg,
			CMSG_CONTENT_PARAM,
			0,
			pbEncodedBlob,
			&cbEncodedBlob)) {

			*ppEnvelopedData = pbEncodedBlob;
			*pEnvelopedDataSize = cbEncodedBlob;
		}
		else {
			//Error
			delete[] pbEncodedBlob;
			rc = GetLastError();
		}
		return rc;
	}
};