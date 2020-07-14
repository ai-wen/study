#include "Pkcs7WithOpenssl.h"


#include <string.h>

#include <openssl/pkcs7.h>
#include <openssl/x509.h>
#include <openssl/obj_mac.h>
#include <openssl/bio.h>
#pragma comment(lib,"libeay32.lib")


Pkcs7WithOpenssl::Pkcs7WithOpenssl()
{
	
}


Pkcs7WithOpenssl::~Pkcs7WithOpenssl()
{
}
/*
int GetDigestNID(ULONG ulDigestMethod)
{
	int type = NID_sha1;
	switch (ulDigestMethod)
	{
	case SGD_MD5:
		type = NID_md5;
		break;
	case SGD_SHA1:
		type = NID_sha1;
		break;
	case SGD_SHA256:
		type = NID_sha256;
		break;
	case SGD_SHA384:
		type = NID_sha384;
		break;
	case SGD_SHA512:
		type = NID_sha512;
		break;
	case SGD_SM3:
		type = NID_sm3WithSM2;
		break;

	}
	return type;
}
*/
/*
	PKCS7* p7 = PKCS7_new();
	//1、设置信封类型
	PKCS7_set_type(p7, NID_pkcs7_enveloped);

	//2、设置加密算法
	const EVP_CIPHER *evp_cipher = EVP_des_cbc();
	PKCS7_set_cipher(p7, evp_cipher);

	//3、设置接收者证书，获取公钥用于加密对称密钥
	PKCS7_RECIP_INFO *p7recipinfo = PKCS7_add_recipient(p7, x509_Cert);

	//4、完成数字信封的运算
	BIO *p7bio = PKCS7_dataInit(p7, NULL);
	char *instr = "How Are You!";
	BIO_write(p7bio, instr, strlen(instr));
	BIO_flush(p7bio);
	PKCS7_dataFinal(p7, p7bio);

	//5、转换PKCS7结构体为DER编码
	int derLen = i2d_PKCS7(p7, NULL);
	const unsigned char * der = (unsigned char*)malloc(derLen);
	unsigned char * derTmp = (unsigned char*)der;
	derLen = i2d_PKCS7(p7, &derTmp);
	BIO_free(p7bio);
	PKCS7_free(p7);
	*/
DWORD MakeSignedPKCS7(BYTE *pCert, ULONG ulCertLen,
	BYTE *pPlainText, ULONG ulPlainTextLen,
	BYTE *pSignedData, ULONG ulSignedDataLen,
	ULONG ulDetached, //ulDetached 1不带原文，0带原文
	ULONG ulDigestMethod,
	BYTE *SignEnvelopeData, long *SignEnvelopeDataLen)
{
	ULONG ulRslt = 1;
	BYTE *pOut = NULL;

	PKCS7 *pkcs7 = NULL;
	X509 *cert = NULL;
	BIO* p7bio = NULL;
	do
	{
		//1
		pkcs7 = PKCS7_new();
		if (!pkcs7)
			break;
		//2
		if (!PKCS7_set_type(pkcs7, NID_pkcs7_signed))
			break;
		//3
		cert = d2i_X509(NULL, (const BYTE **)&pCert, ulCertLen);
		PKCS7_add_certificate(pkcs7, cert);

		//4
		PKCS7_SIGNER_INFO* info = PKCS7_SIGNER_INFO_new();
		

		if (!X509_NAME_set(&info->issuer_and_serial->issuer,
			X509_get_issuer_name(cert)))
			break;

		if (!ASN1_INTEGER_set(info->version, 1))
			break;

		ASN1_INTEGER_free(info->issuer_and_serial->serial);
		if (!(info->issuer_and_serial->serial =
			ASN1_INTEGER_dup(X509_get_serialNumber(cert))))
			break;

		//szOID_RSA_RSA						"1.2.840.113549.1.1.1"
		//szOID_RSA_SHA256RSA					"1.2.840.113549.1.1.11"
		//szOID_NIST_sha256                   "2.16.840.1.101.3.4.2.1"

		//info->digest_alg->algorithm = OBJ_nid2obj(NID_sha256);
		info->digest_alg->algorithm = OBJ_txt2obj("2.16.840.1.101.3.4.2.1",1);
		if (info->digest_alg->parameter == NULL)
			info->digest_alg->parameter = ASN1_TYPE_new();
		ASN1_TYPE_set(info->digest_alg->parameter, V_ASN1_NULL, 0);

		info->digest_enc_alg->algorithm = OBJ_txt2obj("1.2.840.113549.1.1.1", 1);
		if (info->digest_enc_alg->parameter == NULL)
			info->digest_enc_alg->parameter = ASN1_TYPE_new();
		ASN1_TYPE_set(info->digest_enc_alg->parameter, V_ASN1_NULL, 0);

		if (!ASN1_OCTET_STRING_set(info->enc_digest, pSignedData, ulSignedDataLen))
			break;

		PKCS7_add_signer(pkcs7, info);

		//5
		PKCS7_content_new(pkcs7, NID_pkcs7_data);
		PKCS7_set_detached(pkcs7, ulDetached);

		PKCS7 *p7_data = pkcs7->d.sign->contents;

		PKCS7_set_type(p7_data, NID_pkcs7_data);
		if (ulDetached == 1)
		{
			if (p7_data->d.data != NULL)
				ASN1_OCTET_STRING_free(p7_data->d.data);
			p7_data->d.data = NULL;
		}

		PKCS7_set_detached(p7_data, ulDetached);
		if ((ulDetached == 0) && (pPlainText != NULL) && (ulPlainTextLen > 0))
		{
			p7bio = PKCS7_dataInit(p7_data, NULL);
			ulRslt = BIO_write(p7bio, pPlainText, ulPlainTextLen);//加入原始数据，
			BIO_flush(p7bio);
			ulRslt = PKCS7_dataFinal(p7_data, p7bio); //处理数据
		}


		pOut = SignEnvelopeData;
		*SignEnvelopeDataLen = i2d_PKCS7(pkcs7, &pOut);

		ulRslt = 0;

	} while (0);


	if (cert)		X509_free(cert);
	if (pkcs7)		PKCS7_free(pkcs7);
	if (p7bio)		BIO_free(p7bio);

	return ulRslt;
}

ULONG SOF_MakeSignDataToPKCS7(ULONG ulDetached, ULONG digestMethod/*SHA256*/,
	BYTE * hashData, ULONG hashDataLen, 
	BYTE *signData, ULONG signDatalen,
	BYTE	*pcert, ULONG certLen,
	BYTE *OutData, ULONG *OutDataLen)
{
	ULONG ulRslt = 0;
	BYTE pubKey[1024] = { 0 };
	ULONG pubKeyLen = 1024;

	//1、数据哈希
	//2、哈希数据签名
	//3、证书数据

	BYTE	*signEnvelopeData = NULL;
	long	signEnvelopeDataLen = signDatalen + 10240;
	signEnvelopeData = new BYTE[signEnvelopeDataLen];
	memset(signEnvelopeData, 0, signEnvelopeDataLen);

	ulRslt = MakeSignedPKCS7(pcert, certLen,
		hashData, hashDataLen,
		signData,signDatalen,
		ulDetached,//原文数据是否分离
		digestMethod, //哈希算法
		signEnvelopeData,&signEnvelopeDataLen);

	if (ulRslt != 0)
	{
		if (signEnvelopeData != NULL)
		{
			delete[] signEnvelopeData;
			signEnvelopeData = NULL;
		}
		
		return 1;
	}

	if(OutData)
		memcpy(OutData, signEnvelopeData, signEnvelopeDataLen);
	*OutDataLen = signEnvelopeDataLen;

	if (signEnvelopeData != NULL)
	{
		delete[] signEnvelopeData;
		signEnvelopeData = NULL;
	}
	
	return 0;
}



/*
ULONG SOF_VerifyDigestDataToPKCS7(HANDLE devHandle, BYTE *pPkcs7Data, ULONG Pkcs7DataLen, BYTE *OriginalText, ULONG OriGinalTextLen, ULONG ulDetached)
{
	ULONG ulRslt = SAR_OK;
	CmToken_Device_Instance *instance = g_mToken.Find_Instance(devHandle);
	if (instance == NULL)
	{
		RETVAL(SOR_NotInitializeErr);
	}

	BYTE cert[MAX_CERT_LEN] = { 0 };
	ULONG certLen = MAX_CERT_LEN;

	BYTE signData[1024] = { 0 };
	ULONG signDataLen = 1024;

	BYTE plainText[4096] = { 0 };
	ULONG plainTextLen = sizeof(plainText);

	BYTE pub[1024] = { 0 };

	ECCSIGNATUREBLOB eccsign = { 0 };

	ulRslt = ParseSignedPKCS7(pPkcs7Data, Pkcs7DataLen, cert, &certLen,
		signData, &signDataLen, plainText, &plainTextLen);
	if (ulRslt != 0)
	{
		RETVAL(ulRslt);
	}

	BYTE pubKey[1024] = { 0 };
	ULONG pubKeyLen = 1024;
	ULONG ulAlgo;

	ulRslt = get_public_from_cert(cert, certLen, pubKey, &ulAlgo);
	if (ulRslt != SAR_OK)
	{
		RETVAL(ulRslt);
	}

	ulRslt = ValidSignDigestAlgo(instance, ulAlgo);
	if (ulRslt != SAR_OK)
	{
		RETVAL(SOR_NotSupportYetErr);
	}

	if (OriGinalTextLen > 4096)
	{
		return RETVAL(SOR_IndataLenErr);
	}

	BYTE hashData[256] = { 0 };
	ULONG hashDataLen = 256;

	if (ulDetached == 0)
	{
		memcpy(plainText, OriginalText, OriGinalTextLen);

		ulRslt = instance->DigestData(pubKey, pubKeyLen, plainText, OriGinalTextLen, hashData, &hashDataLen);
		if (ulRslt != SAR_OK)
		{
			RETVAL(ulRslt);
		}
	}
	else
	{
		if (OriGinalTextLen > 256)
		{
			return RETVAL(SOR_IndataLenErr);
		}

		memcpy(hashData, OriginalText, OriGinalTextLen);
		hashDataLen = OriGinalTextLen;
	}

	DBG_LOGBUFFER(hashData, hashDataLen, "DigestData");
	DBG_LOGBUFFER(signData, signDataLen, "Signature data");

	ulRslt = instance->VerifyDataEx(cert, certLen, hashData, hashDataLen, signData, signDataLen);
	if (ulRslt != SAR_OK)
	{
		RETVAL(ulRslt);
	}

	RETVAL(ulRslt);
}

int Openssl_PKCS7_Verify(unsigned char* signature_msg, unsigned int length)
{
	int message_length = 0;

	unsigned char* p_signature_msg = signature_msg;

	PKCS7* p7 = d2i_PKCS7(NULL, (const unsigned char**)&p_signature_msg, length);

	if (p7 == NULL)
	{
		return 1;
	}

	BIO *pbio = NULL;
	pbio = BIO_new(BIO_s_mem());
	if (pbio == NULL)
	{
		return 1;
	}

	if (!BIO_puts(pbio, "12345678123456781234567812345678"))
	{
		BIO_free(pbio);
		return 1;
	}

	STACK_OF(PKCS7_SIGNER_INFO) *sk = PKCS7_get_signer_info(p7);

	int signCount = sk_PKCS7_SIGNER_INFO_num(sk);

	for (int i = 0; i < signCount; i++)
	{
		PKCS7_SIGNER_INFO *signInfo = sk_PKCS7_SIGNER_INFO_value(sk, i);
		X509 *cert = PKCS7_cert_from_signer_info(p7, signInfo);

		if (PKCS7_signatureVerify(pbio, p7, signInfo, cert) != 1)
		{
			printf("signature verify error.\n");

			return 1;
		}

		if (cert) X509_free(cert);
	}

	if (p7)	PKCS7_free(p7);
	if (pbio) BIO_free_all(pbio);

	return 0;
}*/


