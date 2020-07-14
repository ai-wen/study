// crypt.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "csp.hpp"

long InstallCert(const char * containerName, const char * cspProvider, long keySpec, unsigned char * valuecert, unsigned long valuecertLen);

#include <atlenc.h>

#include "HookCryptMsgAPI.h"
//#include "CryptMsgApiHook.h"

int main()
{

	//CryptMsgApiHook::InitHook();
	
	HookCryptMsgAPI::CryptMsgApiHook();

	BYTE byCert[4096] = { 0 };
	int certLen = 4096;

	const char * cert = "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";
	//const char * cert = "MIIFCDCCA/CgAwIBAgIQVAEBCX2gC3W0fGohwqE2LzANBgkqhkiG9w0BAQsFADBgMQswCQYDVQQGEwJWTjE6MDgGA1UECgwxQ8OUTkcgVFkgQ+G7lCBQSOG6pk4gVknhu4ROIFRIw5RORyBORVdURUwtVEVMRUNPTTEVMBMGA1UEAxMMTkVXVEVMLUNBIHYyMB4XDTE5MDIxMzA4MDIzMFoXDTIwMDIwODA4MDIzMFowZjELMAkGA1UEBhMCVk4xNzA1BgNVBAMMLkPDlE5HIFRZIEPhu5QgUEjhuqZOIENI4buuIEvDnSBT4buQIE5FV0NBIFRFU1QxHjAcBgoJkiaJk/IsZAEBDA5NU1Q6MDEyMzQ1Njc4OTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKBILwikayZla7zQ4e5zB+aIhLdFxqnhOzQCDTdfp6ZOd2KELhA3Ws9RQBTuE8EcUpty+XYO9wNgWtZy6mVt+rAr9KMNaXUX0yoICpmtlFqGr8M+7MnMyMjHqh31n6A7FKTP6pjJE97L/Wyo+NNB7trCenfzUSas/qjOjH20Y6lbHVz2ra5IxWc5O+KmZKNlSvlXgjMyEfv6pBgaoMUloGJjAf+gCajrOmGv4DJlrcq6dxTBb/GORIJ23NMdk2szf5W7wxMeP7RnIt4uS6UJqG9qcLZD9m+w0hbkLlfV16XSC/rTXYvvB5GA9WLOsKiZ22YwFew1MmZ1MnUKF5s3CPUCAwEAAaOCAbYwggGyMAwGA1UdEwEB/wQCMAAwHwYDVR0jBBgwFoAUgvDxIe/+Tlg0cW/5jEIPPyHdVZswagYIKwYBBQUHAQEEXjBcMC0GCCsGAQUFBzAChiFodHRwOi8vcHViLm5ld2NhLnZuL25ld3RlbC1jYS5jcnQwKwYIKwYBBQUHMAGGH2h0dHA6Ly9vY3NwMi5uZXdjYS52bi9yZXNwb25kZXIwGwYDVR0RBBQwEoEQc3VwcG9ydEBuZXdjYS52bjBeBgNVHSAEVzBVMFMGDCsGAQQBge0DAQkDATBDMCMGCCsGAQUFBwIBFhdodHRwOi8vcHViLm5ld2NhLnZuL3JwYTAcBggrBgEFBQcCAjAQDA5PU19OZXdfMU1fVGVzdDA0BgNVHSUELTArBggrBgEFBQcDAgYIKwYBBQUHAwQGCisGAQQBgjcKAwwGCSqGSIb3LwEBBTAzBgNVHR8ELDAqMCigJqAkhiJodHRwOi8vY3JsMi5uZXdjYS52bi9uZXd0ZWwtY2EuY3JsMB0GA1UdDgQWBBReXyV5OSTHxO6j8jWF4XmBEOes+jAOBgNVHQ8BAf8EBAMCBPAwDQYJKoZIhvcNAQELBQADggEBACNQZoD+UeTMXhi9HcWiZjVwTuwy2btHD3Z+LVUSxUBMo++Lvbqb/2hOvzt/RMO14QMo0gGoRD0cnTjfjy1+4cMPp1HliW52kjyE9B5flBjM+bF9Cw7b6sLIsN34Gb2zD15U35QQJKtnmNpSV1437CMkRJOmlTpZ7MxnIUFh5W6eH7j2ymQWPJSQh3/XC1yEiyfRTg9+QaVc6dCQMHhEeQJq1Kas38nCN3wXCpaJOmKN3fPUmakKiVOXs0clyfgD91BxrJILQLvNjRuC0ROY2ZTdzAIS8xLxDRh80lrtjDreYLOjeVWrbMl1wqgMdLBzF7iYFKMcXfkAzWDODU5bCNA=";
	Base64Decode((LPCSTR)cert, strlen(cert), byCert, &certLen);


	//InstallCert("{F71DDD25-AE7C-44A8-8E42-F7C550DDAA7A}", "Microsoft Base Cryptographic Provider v1.0", AT_KEYEXCHANGE, byCert, certLen);
	



	DWORD dwKeyspce = AT_SIGNATURE;
	//AT_KEYEXCHANGE AT_SIGNATURE

	//Microsoft Base Cryptographic Provider v1.0 Microsoft Enhanced RSA and AES Cryptographic Provider
	HCRYPTPROV hpro = 0;
	
	//if (!CryptAcquireContextA(&hpro, "{5ED27999-FFC7-4F1C-9916-9BB6BD65DC7F}", (LPCSTR)"Microsoft Base Cryptographic Provider v1.0", PROV_RSA_FULL, 0))//CRYPT_MACHINE_KEYSET
	//	return 0;

	if (!CryptAcquireContext(&hpro, TEXT("{5A816861-3F79-458D-A39A-411DA1D91DFD}"), TEXT("Longmai mToken GM3000 CSP V1.1"), PROV_RSA_FULL, 0))//CRYPT_MACHINE_KEYSET
		return 0;

	//if (!CryptAcquireContextA(&hpro, "E9C4E26B-4924-4465-9ABA-1405AF331710", (LPCSTR)"EnterSafe ePass2003 CSP v1.0", PROV_RSA_FULL, 0))//CRYPT_MACHINE_KEYSET CERT_SET_KEY_PROV_HANDLE_PROP_ID
	//{
	//	DWORD dw = GetLastError();
	//	return 0;
	//}



	//if (!CryptAcquireContextA(&hpro, "{F71DDD25-AE7C-44A8-8E42-F7C550DDAA7A}", (LPCSTR)"Microsoft Base Cryptographic Provider v1.0", PROV_RSA_FULL, 0))//CRYPT_MACHINE_KEYSET
	//{
	//	DWORD dw = GetLastError();
	//	return 0;
	//}

	//if (!CryptAcquireContextA(&hpro, "{3B1CB736-7CC0-4D8E-8626-0143E31DD3CD}", (LPCSTR)"Microsoft Base Cryptographic Provider v1.0", PROV_RSA_FULL, 0))//CRYPT_MACHINE_KEYSET
	//	return 0;

	
	
	
	//if (!CryptAcquireContextA(&hpro, "test", (LPCSTR)"EnterSafe ePass3003 CSP v1.0", PROV_RSA_FULL, CERT_SET_KEY_PROV_HANDLE_PROP_ID))//CRYPT_MACHINE_KEYSET CERT_SET_KEY_PROV_HANDLE_PROP_ID
	//{
	//	DWORD dw = GetLastError();
	//	return 0;
	//}


	
	//if (!CryptAcquireContextA(&hpro, NULL, (LPCSTR)"EnterSafe ePass3003 CSP v1.0", PROV_RSA_FULL, 0))//CRYPT_MACHINE_KEYSET
	//	return 0;


	//CERT_SET_KEY_PROV_HANDLE_PROP_ID

	//if (!CryptAcquireContextA(&hpro, "{F41B1FA1-4B30-4610-838A-BEACF3D51CB1}", (LPCSTR)"Microsoft Base Smart Card Crypto Provider", PROV_RSA_FULL, 0))//CRYPT_MACHINE_KEYSET
	//	return 0;


	/*
	BYTE	btParamData[2048] = { 0 };
	DWORD	dwParamLen = 2048;
	memset(btParamData, 0, 2048);

	HCRYPTHASH hHash;
	if (CryptCreateHash(hpro, CALG_SHA_256, 0, 0, &hHash))
	{
		printf("Hash object created. \n");
	}
	else
	{
		return 0;
	}

	if (CryptHashData(hHash, btParamData, dwParamLen, 0))
	{
		printf("The data buffer has been hashed.\n");
	}
	else
	{
		return 0;
	}
	
	
	if (!CryptGetProvParam(hpro, PP_NAME, btParamData, &dwParamLen, 0))
	{
		DWORD dw = GetLastError();
		printf("1\n");
		return 0;

	}

	if (!CryptGetProvParam(hpro, PP_KEYSET_TYPE, btParamData, &dwParamLen, 0))
	{

		DWORD dw = GetLastError();
		printf("1\n");

		return 0;
	}
	*/

	HCERTSTORE hstrore = CertOpenSystemStore(NULL, "MY");//CA  MY ROOT SPC
	if(0 == hstrore)
		return 0;

	PCCERT_CONTEXT pcercon = CertEnumCertificatesInStore(hstrore, NULL);//这个参数必须先置为NULL才能获取第一个证书 


	CryptMsg csp;
	//int rc = csp.open();
	//if (rc)
	//	return 0;

	BYTE bContent[10] = "123456789";
	DWORD dwContentLen = 10;

	BYTE ppSignedData[2048] = { 0 };
	DWORD pSignedDataSize = 2048;

	
	csp.sign(hpro, dwKeyspce, pcercon, (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING), 0,
		bContent, dwContentLen,(BYTE**)&ppSignedData, &pSignedDataSize);

	csp.close();
	//csp.encodeStream();

	CertCloseStore(hstrore,0 );

	CryptReleaseContext(hpro, 0);

	return 0;
}



long InstallCert(const char * containerName, const char * cspProvider, long keySpec,unsigned char * valuecert, unsigned long valuecertLen)
{
	HCERTSTORE hCertStore = NULL;
	PCCERT_CONTEXT pCertContext;
	HCRYPTPROV     hProv = NULL;

	unsigned char* pCertBuffer = NULL;
	const unsigned char **pp = NULL;
	long nCertSize = 0;

	hCertStore = CertOpenStore(CERT_STORE_PROV_SYSTEM_W,
		0,
		hProv,
		CERT_STORE_NO_CRYPT_RELEASE_FLAG |
		CERT_SYSTEM_STORE_CURRENT_USER,
		L"MY");
	if (NULL == hCertStore)
	{
		return 2;
	}

	
	pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING,
		valuecert,
		valuecertLen);
	if (pCertContext == NULL)
	{
		return 3;
	}

	
	CRYPT_KEY_PROV_INFO KeyProvInfo;

	ZeroMemory((PVOID)&KeyProvInfo, sizeof(CRYPT_KEY_PROV_INFO));

	WCHAR wszContainerName[MAX_PATH] = { 0 };
	WCHAR wszCSPName[MAX_PATH] = { 0 };

	//Get the container name
	MultiByteToWideChar(CP_ACP, 0, containerName, strlen(containerName) + 1, wszContainerName, MAX_PATH);
	MultiByteToWideChar(CP_ACP, 0, cspProvider, strlen(cspProvider) + 1, wszCSPName, MAX_PATH);


	nCertSize = pCertContext->cbCertEncoded;
	pCertBuffer = new unsigned char[nCertSize];
	memcpy(pCertBuffer, pCertContext->pbCertEncoded, nCertSize);
	pp = (const unsigned char **)&pCertBuffer;


		KeyProvInfo.pwszContainerName = wszContainerName;
		KeyProvInfo.pwszProvName = wszCSPName;
		KeyProvInfo.dwProvType = PROV_RSA_FULL;
		KeyProvInfo.dwFlags = 0;
		KeyProvInfo.dwKeySpec = keySpec;
	
		

		//CERT_SHA256_HASH_PROP_ID
	if (!CertSetCertificateContextProperty(pCertContext,
		CERT_KEY_PROV_INFO_PROP_ID,
		0,
		(const void *)&KeyProvInfo))
	{
		return 4;
	}

	/*
	CERT_KEY_CONTEXT KeyConText;
	ZeroMemory((PVOID)&KeyConText, sizeof(CERT_KEY_CONTEXT));
	KeyConText.cbSize = sizeof(CERT_KEY_CONTEXT);
	KeyConText.dwKeySpec = keySpec;

	if (!CertSetCertificateContextProperty(pCertContext,
		CERT_KEY_SPEC_PROP_ID,
		0,
		(const void *)&KeyConText))
	{
		return 4;
	}
	*/

	if (!CertAddCertificateContextToStore(hCertStore,
		pCertContext,
		CERT_STORE_ADD_REPLACE_EXISTING,
		NULL))
	{
		return 5;
	}

	if (pCertContext != NULL)
	{
		CertFreeCertificateContext(pCertContext);
	}

	if (hCertStore != NULL)
	{
		CertCloseStore(hCertStore, CERT_CLOSE_STORE_FORCE_FLAG);
	}

	return 0;
}



