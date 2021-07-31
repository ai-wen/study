
# csp-------------------------------------------------------

## [CSP 证书请求](https://docs.microsoft.com/zh-cn/windows/win32/seccrypto/example-c-program-making-a-certificate-request)
https://docs.microsoft.com/zh-cn/windows/win32/seccrypto/request-sample-in-c-

其他扩展的 C 代码示例
验证 CTL
使用 Ctl 验证签名消息
示例 C 程序：证书验证操作
示例 C 程序：使用密钥标识符
示例 C 程序：创建证书链
示例 C 程序：发出证书请求
示例 C 程序： ASN 1 编码和解码
示例 C 程序：使用 CertOIDToAlgId 和 CertCompareIntegerBlob
示例 C 程序：验证 PE 文件的签名
修改密钥容器访问权限


## [P10请求工具 Certreq.exe 和 Certutil.exe ](https://docs.microsoft.com/zh-cn/windows/win32/seccertenroll/pkcs--10-request)
https://docs.microsoft.com/zh-cn/windows/win32/seccertenroll/sample-requests

### 查看支持的HASH算法 certutil -oid 1 | findstr pwszCNGAlgid | findstr /v CryptOIDInfo
### 查看支持的对称算法certutil -oid 2 | findstr pwszCNGAlgid
### 查看CSP名列表 certutil –csplist

### 生成P10请求文件  certreq -new xxx.inf  xxx.req

### 查看P10 请求文件 certutil xxx.req
### D:/OpenSSL-Win32/bin/openssl.exe req -in xxx.req -noout -text

```conf https://docs.microsoft.com/en-us/windows-server/administration/windows-commands/certreq_1
[Version]
Signature=$Windows NT$

[Strings]
szOID_SUBJECT_ALT_NAME2 = 2.5.29.17

[NewRequest]
Subject="CN=John Smith,CN=Users,DC=Contoso,DC=com"
HashAlgorithm = sha1
KeyAlgorithm = RSA
KeyContainer = Container
KeyLength = 1024
KeySpec = AT_SIGNATURE
;KeyUsage = 0xa0
;KeyUsageProperty = 0x3
ProviderName = "Longmai mToken GM3000 CSP V1.1"
ProviderType = 1
RequestType=pkcs10
UseExistingKeySet = true

[Extensions]
%szOID_SUBJECT_ALT_NAME2%={text}
_continue_   = IPAddress=10.0.0.1&
_continue_   = DNS=host.domain.com&
```


```conf
[Version]
Signature=$Windows NT$

[Strings]
szOID_SUBJECT_ALT_NAME2 = 2.5.29.17

[NewRequest]
Subject="CN=John Smith,CN=Users,DC=Contoso,DC=com"
Exportable = true
HashAlgorithm = sha1
;KeyAlgorithm = RSA
;KeyLength = 1024
KeySpec = AT_SIGNATURE
RequestType=pkcs10

[Extensions]
%szOID_SUBJECT_ALT_NAME2%={text}
_continue_   = IPAddress=10.0.0.1&
_continue_   = DNS=host.domain.com&
```



```cpp
#include <string.h>
#include <windows.h>
#include <Wincrypt.h>
#pragma comment(lib, "crypt32.lib")

#include <atlenc.h>

#define MAX_ITEM_NUM  30

//自行修改
DWORD CreatSubj(const char* Subject, PCERT_NAME_BLOB pSubjNameBlob)
{
	static BYTE  pbNameEncoded[4096] = { 0 };
	DWORD cbNameEncoded = sizeof(pbNameEncoded);
	CERT_RDN_ATTR rgNameAttr[] = {
		{(LPSTR)szOID_COMMON_NAME, CERT_RDN_PRINTABLE_STRING,strlen("This certificate user"),(BYTE*)"This certificate user" } // CN
	};
	CERT_RDN rgRDN[] = { 1, &rgNameAttr[0] };
	CERT_NAME_INFO Name = { 1, rgRDN };

	if (!CryptEncodeObject(
		PKCS_7_ASN_ENCODING | X509_ASN_ENCODING,    // Encoding type
		X509_NAME,           // Structure type
		&Name,               // Address of CERT_NAME_INFO structure
		pbNameEncoded,       // pbEncoded
		&cbNameEncoded))     // pbEncoded size
	{
		return GetLastError();
	}
	pSubjNameBlob->cbData = cbNameEncoded;
	pSubjNameBlob->pbData = pbNameEncoded;

	return 0;
}

void CreatDNSEntry(const char* DNS, PCERT_ALT_NAME_ENTRY entry, unsigned int *enterOffsetNum)
{
	static wchar_t byIP[MAX_ITEM_NUM][100] = { 0 };
	int dnsNum = 0;
	char * p = strtok((char*)DNS, ",");
	while (p)
	{
		if (strlen(p) > 0)
		{
			//wchar_t* pstr = (wchar_t*)malloc(strlen(p) * 3);
			memset(byIP[dnsNum], 0, 100);
			MultiByteToWideChar(CP_ACP, 0, p, -1, byIP[dnsNum], strlen(p) * 3);
			entry[*enterOffsetNum+dnsNum].dwAltNameChoice = CERT_ALT_NAME_DNS_NAME;
			entry[*enterOffsetNum+dnsNum].pwszDNSName = byIP[dnsNum];
			dnsNum++;
		}
		p = strtok(NULL, ",");
	}
	*enterOffsetNum = dnsNum;

	return ;
}

void CreatIPv4Entry(const char* IPv4, PCERT_ALT_NAME_ENTRY entry, unsigned int *enterOffsetNum)
{
	static BYTE byIP[MAX_ITEM_NUM][4] = {0};

	char tmpIPBuf[MAX_ITEM_NUM][18] = {0};
	int ipNum = 0, ipNumB = 0;
	char * p = strtok((char*)IPv4, ",");
	while (p)
	{
		if (strlen(p) > 0)
		{
			strcpy(tmpIPBuf[ipNum++], p);
		}
		if (ipNum >= MAX_ITEM_NUM)
			break;
		p = strtok(NULL, ",");
	}

	for (size_t i = 0; i < ipNum; i++)
	{
		int ipdotNum = 0;
		char* tmpIP = strtok(tmpIPBuf[i], ".");
		while (tmpIP)
		{
			if (strlen(tmpIP) <= 0)
				break;
			byIP[ipNumB][ipdotNum++] = atoi(tmpIP);
			if (ipdotNum > 3)
				break;
			tmpIP = strtok(NULL, ".");
		}
		if (ipdotNum == 4)
		{
			entry[*enterOffsetNum + ipNumB].dwAltNameChoice = CERT_ALT_NAME_IP_ADDRESS;
			entry[*enterOffsetNum + ipNumB].IPAddress.cbData = 4;
			entry[*enterOffsetNum + ipNumB].IPAddress.pbData = byIP[ipNumB];

			ipNumB++;
		}
		else
		{
			//ip 地址格式有误
		}
	}
	*enterOffsetNum = ipNumB;
	return ;
}

void CreatIPv6Entry(const char* IPv6, PCERT_ALT_NAME_ENTRY entry, unsigned int *enterOffsetNum)
{
	//IPv6 地址的长度为 128 位，由八个 16 位字段组成，相邻字段用冒号分隔。
	//IPv6地址的压缩方式
	//零压缩法可以用来缩减其长度。如果几个连续段位的值都是0，那么这些0就可以简单的以::来表示。 零压缩法压缩一次，后面的0不能再被压缩。
	//fe80::1dc5:3246:4581:102e%9
	//fe80::24a3:5275:5d45:811b%7

	//big-endian,  

	static BYTE byIP[MAX_ITEM_NUM][16] = { 0 };

	char tmpIPBuf[MAX_ITEM_NUM][64] = { 0 };
	BYTE* iptmp = NULL;

	int ipNum = 0, ipNumB=0;
	char * p = strtok((char*)IPv6, ",");
	while (p)
	{
		if (strlen(p) > 0)
		{
			strcpy(tmpIPBuf[ipNum++], p);
		}
		if (ipNum >= MAX_ITEM_NUM)
			break;
		p = strtok(NULL, ",");
	}

	for (size_t i = 0; i < ipNum; i++)
	{
		iptmp = byIP[ipNumB];
		int ipColonNum = 0;
		char* tmpIP = strtok(tmpIPBuf[i], ":");
		while (tmpIP)
		{
			if (4 != strlen(tmpIP))
				break;

			char bC[3] = { 0 };
			bC[0] = tmpIP[0];
			bC[1] = tmpIP[1];
			*iptmp++ = atoi(tmpIP);
			bC[0] = tmpIP[2];
			bC[1] = tmpIP[3];
			*iptmp++ = atoi(tmpIP);
			ipColonNum++;
			if (ipColonNum > 7)
				break;
			tmpIP = strtok(NULL, ":");
		}

		if (ipColonNum == 8)
		{
			entry[*enterOffsetNum + ipNumB].dwAltNameChoice = CERT_ALT_NAME_IP_ADDRESS;
			entry[*enterOffsetNum + ipNumB].IPAddress.cbData = 16;
			entry[*enterOffsetNum + ipNumB].IPAddress.pbData = byIP[ipNumB];

			ipNumB++;
		}
		else
		{
			//ip 地址格式有误
		}
	}

	*enterOffsetNum = ipNumB;

	return ;
}

DWORD CreatAttr(PCERT_ALT_NAME_ENTRY pEntries, unsigned int entryNum, PCRYPT_ATTRIBUTE pattr)
{
	DWORD dwRet = 0;
	CERT_ALT_NAME_INFO certinfo = { entryNum, pEntries };

	DWORD  cbEncoded = 0;
	BYTE*  pbinfoEncoded = NULL;
	
	CERT_EXTENSION  ext = { 0 };
	ext.fCritical = 0;
	ext.pszObjId = (LPSTR)szOID_SUBJECT_ALT_NAME2;

	CERT_EXTENSIONS exts = { 1, &ext };

	static BYTE  pbextsEncoded[4096] = {0};
	static CRYPT_ATTR_BLOB    rgValue = {0};

	do
	{
		if (!CryptEncodeObject(X509_ASN_ENCODING, X509_ALTERNATE_NAME, &certinfo, pbinfoEncoded, &cbEncoded))
		{
			dwRet = GetLastError(); break;
		}
		pbinfoEncoded = (BYTE*)malloc(cbEncoded);
		if (!CryptEncodeObject(X509_ASN_ENCODING, X509_ALTERNATE_NAME, &certinfo, pbinfoEncoded, &cbEncoded))
		{
			dwRet = GetLastError(); break;
		}

		ext.Value.cbData = cbEncoded;
		ext.Value.pbData = pbinfoEncoded;

        /*
        //添加DNS扩展到自签名证书
        CERT_NAME_BLOB name = { cbEncoded, buf };
        PCCERT_CONTEXT cert = CertCreateSelfSignCertificate(0, &name, 0, &keyProvInfo,  &sigAlg,0,&certExpirationDate,
        &exts);
        */
		if (!CryptEncodeObject( X509_ASN_ENCODING, X509_EXTENSIONS, &exts, NULL,&cbEncoded))
		{
			dwRet = GetLastError(); break;
		}

		if (cbEncoded > sizeof(pbextsEncoded))
		{
			dwRet = TYPE_E_BUFFERTOOSMALL; break;
		}

		if (!CryptEncodeObject(X509_ASN_ENCODING, X509_EXTENSIONS, &exts, pbextsEncoded, &cbEncoded))
		{
			dwRet = GetLastError(); break;
		}

		rgValue.cbData = cbEncoded;
		rgValue.pbData = pbextsEncoded;

		pattr->pszObjId = (LPSTR)szOID_CERT_EXTENSIONS;
		pattr->cValue = 1;
		pattr->rgValue = &rgValue;

	} while (0);
	

	if (pbinfoEncoded)
		free(pbinfoEncoded);
	
	return dwRet;
}

DWORD CreatP10(const char* Subject, const char* DNS, const char* IPv4, const char* IPv6, char *outBase64P10, int base64Len)
{	
	DWORD dwRet = 0;
	CERT_REQUEST_INFO  CertReqInfo = { 0 };
	
	CERT_NAME_BLOB  SubjNameBlob = {0};
	CRYPT_ATTRIBUTE extern_attr = { 0 };

	unsigned int enterOffsetNum = 0;
	unsigned int entrNum = 0;
	unsigned int dnsNum = 0, ipv4Num = 0, ipv6Num = 0;
	PCERT_ALT_NAME_ENTRY pEntries = NULL;
	
	CertReqInfo.dwVersion = CERT_REQUEST_V1;

	HCRYPTPROV  hCryptProv = 0;
	CERT_PUBLIC_KEY_INFO*  pbPublicKeyInfo = NULL;
	DWORD  cbPublicKeyInfo = 0;

	CRYPT_ALGORITHM_IDENTIFIER  SigAlg = { 0 };
	CRYPT_OBJID_BLOB  Parameters = { 0 };


	BYTE*  pbSignedEncodedCertReq = NULL;
	DWORD  cbEncodedCertReqSize = 0;

	do
	{
		dwRet = CreatSubj(Subject, &SubjNameBlob);
		if (0 != dwRet)
			break;

		CertReqInfo.Subject = SubjNameBlob;

		for (int i = strlen(DNS) - 1; i >= 0; i--)
		{
			if (DNS[i] == ',')
				entrNum++;
		}
		entrNum++;
		for (int i = strlen(IPv4) - 1; i >= 0; i--)
		{
			if (IPv4[i] == ',')
				entrNum++;
		}
		entrNum++;
		for (int i = strlen(IPv6) - 1; i >= 0; i--)
		{
			if (IPv6[i] == ',')
				entrNum++;
		}
		entrNum++;
		pEntries = (PCERT_ALT_NAME_ENTRY)malloc(sizeof(CERT_ALT_NAME_ENTRY)*entrNum);

		enterOffsetNum = 0;
		CreatDNSEntry(DNS, pEntries, &enterOffsetNum);
		dnsNum = enterOffsetNum;

		CreatIPv4Entry(IPv4, pEntries, &enterOffsetNum);
		ipv4Num = enterOffsetNum;

		enterOffsetNum = dnsNum + ipv4Num;
		CreatIPv6Entry(IPv6, pEntries, &enterOffsetNum);
		ipv6Num = enterOffsetNum;

		dwRet = CreatAttr(pEntries, dnsNum + ipv4Num + ipv6Num,&extern_attr);
		if (0 != dwRet)
			break;

		CertReqInfo.cAttribute = 1;
		CertReqInfo.rgAttribute = &extern_attr;


		if (!CryptAcquireContext( &hCryptProv,  L"Container",  L"Longmai mToken GM3000 CSP V1.1", PROV_RSA_FULL,NULL)) 
		{
			dwRet = GetLastError(); break;
		}

		if (!CryptExportPublicKeyInfo(hCryptProv, AT_SIGNATURE,  PKCS_7_ASN_ENCODING | X509_ASN_ENCODING,NULL, &cbPublicKeyInfo))
		{
			dwRet = GetLastError(); break;
		}
		pbPublicKeyInfo = (CERT_PUBLIC_KEY_INFO*)malloc(cbPublicKeyInfo);
		if (!CryptExportPublicKeyInfo(hCryptProv, AT_SIGNATURE, PKCS_7_ASN_ENCODING | X509_ASN_ENCODING, pbPublicKeyInfo, &cbPublicKeyInfo))
		{
			dwRet = GetLastError(); break;
		}

		CertReqInfo.SubjectPublicKeyInfo = *pbPublicKeyInfo;

		SigAlg.pszObjId = (LPSTR)szOID_OIWSEC_sha1RSASign;
		SigAlg.Parameters = Parameters;

		if (!CryptSignAndEncodeCertificate(hCryptProv, AT_KEYEXCHANGE,
			PKCS_7_ASN_ENCODING | X509_ASN_ENCODING,X509_CERT_REQUEST_TO_BE_SIGNED,  
			&CertReqInfo, &SigAlg,  NULL, 
			NULL,&cbEncodedCertReqSize)) 
		{
			dwRet = GetLastError(); break;
		}
		pbSignedEncodedCertReq = (BYTE*)malloc(cbEncodedCertReqSize);

		if (!CryptSignAndEncodeCertificate(hCryptProv, AT_KEYEXCHANGE,
			PKCS_7_ASN_ENCODING | X509_ASN_ENCODING, X509_CERT_REQUEST_TO_BE_SIGNED,
			&CertReqInfo, &SigAlg, NULL,
			pbSignedEncodedCertReq, &cbEncodedCertReqSize))
		{
			dwRet = GetLastError(); break;
		}

		/*
		//每76字符换行，且最后一行不足76也换行, 比 ATL_BASE64_FLAG_NOPAD 模式多一字节
		if (!Base64Encode(pbSignedEncodedCertReq, cbEncodedCertReqSize, outBase64P10, &base64Len,ATL_BASE64_FLAG_NONE))
		{
			dwRet = 1; break;
		}

		base64Len = 4096; //每76字符换行，最后一行不足76不换行
		if (!Base64Encode(pbSignedEncodedCertReq, cbEncodedCertReqSize, outBase64P10, &base64Len, ATL_BASE64_FLAG_NOPAD))
		{
			dwRet = 1; break;
		}
		*/
		//不换行不填充
		if (!Base64Encode(pbSignedEncodedCertReq, cbEncodedCertReqSize, outBase64P10, &base64Len, ATL_BASE64_FLAG_NOCRLF))
		{
			dwRet = 1; break;
		}


	} while (0);
	
	if (pEntries)
		free(pEntries);
	if (pbPublicKeyInfo)
		free(pbPublicKeyInfo);
	if (pbSignedEncodedCertReq)
		free(pbSignedEncodedCertReq);

	return dwRet;
}


int main()
{
	char strDNS[] = "www.demo.com,demo.com,expor.com,expor.cn";
	char strIPv4[] = "192.168.0.1,192.255.0.1,192.168.255.1,0.168.0.1,0.168.0.";
	char strIPv6[] = "fe80:0000:0000:24a3:5275:5d45:811b:811b,0000:0000:0000:24a3:5275:5d45:0000:0000,0000:0000:0000:24a3:5275:5d45:0000:";

	char szCSR[4096] = {0};
	int csrLen = 4096;

	DWORD ret = CreatP10(NULL, strDNS, strIPv4, strIPv6, szCSR, csrLen);

	return 0;
}
```


## [csp com组件](https://docs.microsoft.com/en-us/windows/win32/seccertenroll/mapping-xenroll-dll-to-certenroll-dll)
## IEnroll::createFilePKCS10WStr method (xenroll.h)  [win32api](https://docs.microsoft.com/zh-cn/windows/win32/seccertenroll/certificate-request-functions)

CertEnroll.dll 替代 Xenroll.dll
#include <xenroll.h>
#pragma comment(lib,"Uuid.lib") //Xenroll.dll
#include <certenroll.h>         // CertEnroll.dll

https://social.msdn.microsoft.com/Forums/en-US/a8c921a0-36a9-4ebd-a7f3-4d664b3d7187/scep-protocol-with-crypto-api
https://docs.microsoft.com/en-us/windows/win32/api/xenroll/nn-xenroll-ienroll4

IEnroll4*      CertEnroll = NULL;
CRYPT_DATA_BLOB      MyBlob = { 0, NULL };
hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
hr = CoCreateInstance(CLSID_CEnroll2,NULL,CLSCTX_INPROC_SERVER,IID_IEnroll4,(void **)&CertEnroll );
hr = CertEnroll->put_ProviderType( PROV_RSA_SCHANNEL);
hr = CertEnroll->put_ProviderNameWStr(L"Microsoft RSA SChannel Cryptographic Provider" );
hr = CertEnroll->put_KeySpec( AT_KEYEXCHANGE );
hr = CertEnroll->put_EnableSMIMECapabilities( FALSE );
hr = CertEnroll->put_GenKeyFlags( 1024 << 16 );
hr = CertEnroll->put_MyStoreFlags(CERT_SYSTEM_STORE_LOCAL_MACHINE);
hr = CertEnroll->put_HashAlgorithmWStr(L"1.2.840.113549.1.1.4");
hr = CertEnroll->createPKCS10WStr(L"CN=HeinBollo", L"", &MyBlob);



















# openssl --------------------------------------------------------------

[csr add san](https://www.ssls.com/knowledgebase/how-to-fill-in-the-san-fields-in-the-csr/)

```conf
#req.conf
[req]
distinguished_name = req_distinguished_name
req_extensions = v3_req
prompt = no
[req_distinguished_name]
C = US
ST = VA
L = City
O = YourOrganization
OU = YourOrganizationUnit
CN = www.example.com
[v3_req]
keyUsage = keyEncipherment, dataEncipherment
extendedKeyUsage = serverAuth
subjectAltName = @alt_names
[alt_names]
DNS.1 = www.example.com
DNS.2 = example.com
DNS.3 = sub.example.com
DNS.4 = sub2.example.net
```
## 默认情况下，多域证书不支持www子域。如果您需要使用一个多域证书来保护www.example.com和example.com，则应在证书激活期间指定两个主机名。

### 生成P10 openssl req -new -sha256 -newkey rsa:2048 -nodes -keyout example.key  -out example.csr  -config req.conf -subj "/C=CN/ST=sh/L=sh/O=信息科技/OU=IT/CN=demo.com"

### 查看p10 D:/OpenSSL-Win32/bin/openssl.exe req -in example.csr -noout -text

```conf
openssl.exe req -in example.csr -noout -text
Can't open C:\Program Files (x86)\Common Files\SSL/openssl.cnf for reading, No such file or directory
8260:error:02001003:system library:fopen:No such process:crypto\bio\bss_file.c:74:fopen('C:\Program Files (x86)\Common Files\SSL/openssl.cnf','r')
8260:error:2006D080:BIO routines:BIO_new_file:no such file:crypto\bio\bss_file.c:81:
Certificate Request:
    Data:
        Version: 1 (0x0)
        Subject: C = US, ST = VA, L = City, O = YourOrganization, OU = YourOrganizationUnit, CN = www.example.com
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
                Public-Key: (2048 bit)
                Modulus:
                    00:d1:84:99:2b:97:a8:fa:08:1d:d1:54:9b:41:4d:
                    f0:08:4f:40:1d:e1:7e:78:1a:73:98:9b:44:71:1e:
                    a4:4e:b1:b9:fc:d6:1d:67:65:16:b4:e0:5e:21:49:
                    f3:74:14:c1:c5:11:01:b4:da:76:c0:22:73:4b:a3:
                    63:b4:6a:95:1e:44:c7:77:26:55:7a:0f:7f:a2:b7:
                    74:a6:b2:7f:5c:cb:e0:b1:36:54:29:b0:91:09:89:
                    02:c4:7a:ca:fb:1c:31:3a:ef:96:f0:a1:38:4d:e8:
                    9a:2b:b5:34:49:3b:b7:cc:fb:1e:47:e6:d1:05:34:
                    c2:52:aa:db:f6:e7:c8:2f:92:73:39:3a:16:5b:26:
                    0f:52:f8:a0:c8:5a:cb:78:63:3d:3b:4b:67:83:2d:
                    1d:77:12:51:dc:25:66:2f:8a:8d:28:f2:f0:cd:78:
                    90:97:18:a2:3b:df:b0:80:5c:eb:de:8b:2f:7f:fe:
                    94:d3:67:db:27:11:48:78:66:71:2e:f6:b9:0c:d3:
                    40:ee:13:6c:08:f7:5b:83:58:54:0f:30:8d:f7:5a:
                    5e:99:c3:15:00:39:81:41:08:bc:4c:2f:14:19:ef:
                    73:0f:f2:31:c6:aa:b1:fb:92:4c:6d:ff:e5:6c:59:
                    d1:47:b4:e7:c9:3a:0b:70:54:ae:e6:d7:83:7d:4d:
                    c8:25
                Exponent: 65537 (0x10001)
        Attributes:
        Requested Extensions:
            X509v3 Key Usage:
                Key Encipherment, Data Encipherment
            X509v3 Extended Key Usage:
                TLS Web Server Authentication
            X509v3 Subject Alternative Name:
                DNS:www.example.com, DNS:example.com, DNS:sub.example.com, DNS:sub2.example.net
    Signature Algorithm: sha256WithRSAEncryption
         71:bc:da:84:e2:6a:34:7a:59:94:1a:91:b5:8b:69:d5:fd:9c:
         63:5f:c9:b0:c3:70:53:ec:a1:e2:d0:15:67:12:df:14:8a:a4:
         e5:79:e2:8d:6b:04:4c:fa:e9:22:fb:04:7d:7b:87:2b:eb:97:
         a5:30:3a:f3:6c:ff:49:1f:67:61:8e:cb:77:5a:a1:9f:1d:9c:
         81:05:3c:9b:5f:02:da:8f:5b:eb:82:cf:d7:89:dd:57:96:a7:
         b5:4c:f9:08:86:16:bb:09:bf:60:b7:39:15:1d:a4:85:6d:73:
         43:76:b3:82:dd:28:45:40:3f:94:e4:32:6a:1b:d2:c0:08:f7:
         96:3e:48:80:cd:d9:3e:9f:7d:c5:b7:2a:5e:5c:bf:bf:3b:dc:
         7d:22:85:c4:b8:01:c3:f7:e4:c4:7b:f8:33:eb:0d:e3:d5:d8:
         c4:65:fc:2b:bf:f3:6c:b7:c5:43:1b:f5:76:a7:98:9e:10:c0:
         cc:6f:a8:73:71:c7:4a:46:fc:a7:14:2e:25:b1:96:c0:81:06:
         46:68:77:71:2e:fe:57:5f:b8:93:5a:a5:3b:26:ec:ad:6b:dd:
         aa:a7:07:12:e5:58:f4:f8:2a:f8:60:04:fc:2a:8e:cd:b5:26:
         92:7b:fc:10:fa:8d:f5:ee:f9:74:d5:63:da:0d:bd:48:ce:60:
         4c:e1:8f:59
```

```conf
Data:
        Version: 1 (0x0)
        Subject: CN = This certificate user
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
                Public-Key: (1024 bit)
                Modulus:
                    00:d7:13:47:78:66:13:d7:6f:c3:89:d4:22:9c:04:
                    bb:e3:bc:89:41:f8:2f:fe:11:83:cb:42:6d:d6:58:
                    7c:61:b1:24:2d:1b:57:68:95:30:89:f4:17:df:4b:
                    5c:2d:83:e5:b0:7e:a5:36:3f:0c:9d:35:a8:98:35:
                    aa:b3:6a:da:3d:0f:5b:74:6c:12:fc:e9:a8:b7:90:
                    b1:2a:b9:bd:16:cb:66:16:2c:02:b6:e3:27:3f:a8:
                    a8:61:44:84:75:9f:f1:51:b4:ea:dd:e0:26:72:7c:
                    9b:71:e0:b3:69:0b:0d:57:75:ed:60:57:61:0b:ec:
                    2d:f5:3f:41:05:7d:4e:ce:11
                Exponent: 65537 (0x10001)
        Attributes:
            a0:00
```

```conf
openssl.exe req -help
Usage: req [options]
Valid options are:
 -help               Display this summary
 -inform PEM|DER     Input format - DER or PEM
 -outform PEM|DER    Output format - DER or PEM
 -in infile          Input file
 -out outfile        Output file
 -key val            Private key to use
 -keyform format     Key file format
 -pubkey             Output public key
 -new                New request
 -config infile      Request template file
 -keyout outfile     File to send the key to
 -passin val         Private key password source
 -passout val        Output file pass phrase source
 -rand val           Load the file(s) into the random number generator
 -newkey val         Specify as type:bits
 -pkeyopt val        Public key options as opt:value
 -sigopt val         Signature parameter in n:v form
 -batch              Do not ask anything during request generation
 -newhdr             Output "NEW" in the header lines
 -modulus            RSA modulus
 -verify             Verify signature on REQ
 -nodes              Don't encrypt the output key
 -noout              Do not output REQ
 -verbose            Verbose output
 -utf8               Input characters are UTF8 (default ASCII)
 -nameopt val        Various certificate name options
 -reqopt val         Various request text options
 -text               Text form of request
 -x509               Output a x509 structure instead of a cert request
                     (Required by some CA's)
 -subj val           Set or modify request subject
 -subject            Output the request's subject
 -multivalue-rdn     Enable support for multivalued RDNs
 -days +int          Number of days cert is valid for
 -set_serial val     Serial number to use
 -extensions val     Cert extension section (override value in config file)
 -reqexts val        Request extension section (override value in config file)
 -*                  Any supported digest
 -engine val         Use engine, possibly a hardware device
 -keygen_engine val  Specify engine to be used for key generation operations
```