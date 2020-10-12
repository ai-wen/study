// paPfx.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<stdio.h> 
#include<windows.h> 
#include<wincrypt.h> 
#include<cryptuiapi.h>//需要装PLATFORMSDK 
#include<tchar.h> 

#pragma comment(lib,"crypt32.lib") 

#pragma comment(lib,"cryptui.lib") 

#pragma comment(lib,"E:\\myGit\\crypto\\opensslwin32D\\lib\\libcrypto.lib") 

#define MY_ENCODING_TYPE (PKCS_7_ASN_ENCODING| X509_ASN_ENCODING) 

void MyHandleError(char* s); 

#include <openssl/bio.h> 
#include <openssl/rsa.h> 
#include <openssl/x509.h> 
#include <openssl/pkcs12.h>  
#include <openssl/ossl_typ.h>

void OpenssLParsePkcs12Cert(const char *fileName,const char* pwd)
{
	X509 *x = NULL;
	BIO *in = NULL;
	EVP_PKEY *pkey = NULL;
	PKCS12 *p12 = NULL;
	//unsigned char szPri[256] = { 0 };
	const BIGNUM *pri = NULL;
	int nlen = -1;
	//unsigned char  szPubKey[1024];
	//unsigned char *pszPubKey = szPubKey;
	EC_KEY* ecKey = NULL;

	char szCert[2048] = { 0 };

	ASN1_BIT_STRING * pubkey = NULL;
	const unsigned char **pp = NULL;
	X509* pX509 = NULL;


	OpenSSL_add_all_algorithms();

	in = BIO_new_file(fileName, "r");
	if (in == NULL)
		return;

	//if ((in = BIO_new_mem_buf (m_pCertBuffer, m_nCertSize)) == NULL)
	//	goto RELEA;

	p12 = d2i_PKCS12_bio(in, NULL);
	if (NULL == p12)
		return;

	PKCS12_parse(p12, pwd, &pkey, &x, NULL);
	if (NULL == pkey)
		return;

	PKCS12_SAFEBAG *p12bag = PKCS12_x5092certbag(x);
	char* name = PKCS12_get_friendlyname(p12bag);

	return;
}

long ParsePkcs12Cert(const char *szFileName,const char* szPassword )
{
	long nRet = 0;

	if (NULL == szPassword) szPassword = "";
	

	FILE* fp = NULL;
	if (0 != fopen_s(&fp, szFileName, "rb"))
	{
		return 2;
	}

	if (0 != fseek(fp, 0, SEEK_END))
	{
		fclose(fp);
		return 3;
	}

	unsigned long nFileSize = (unsigned long)ftell(fp);
	unsigned char *pFileBuffer = new unsigned char[nFileSize];
	memset(pFileBuffer, 0 , nFileSize);

	fseek(fp, 0, SEEK_SET);
	fread(pFileBuffer, sizeof(unsigned char), nFileSize, fp);

	fclose(fp);


	WCHAR wPassword[256] = {0};
	int nWideChar = 0;
	nWideChar = MultiByteToWideChar(CP_ACP, 0, szPassword, -1, wPassword, nWideChar);
	MultiByteToWideChar(CP_ACP, 0, szPassword, -1, wPassword, nWideChar);

	//将证书数据导入临时store
	CRYPT_DATA_BLOB blob;
	memset(&blob, 0, sizeof(blob));
	blob.pbData = pFileBuffer;
	blob.cbData = nFileSize;

	HCERTSTORE hCertStore = PFXImportCertStore(&blob, wPassword, CRYPT_EXPORTABLE);//
	if(hCertStore == NULL)
	{
		//Fix the bug in winxp 
		if(wcslen(wPassword) == 0)
		{
			hCertStore = PFXImportCertStore(&blob, NULL, CRYPT_EXPORTABLE);
			if(hCertStore == NULL)
				return GetLastError();
		}
		else
		{
			return GetLastError();
		}
	}


	char pszNameString[256];
	DWORD dwPropId=0; 

	PCCERT_CONTEXT  pCertContext = NULL;
	while(pCertContext = CertEnumCertificatesInStore(hCertStore, pCertContext))
	{
		//打印证书名称 
		if(CertGetNameString(pCertContext, CERT_NAME_RDN_TYPE, 0, NULL, pszNameString, 128)) 
			printf("\n %s\n",pszNameString); 

		//遍历指定证书所有属性标识 

		while(dwPropId=CertEnumCertificateContextProperties( pCertContext, dwPropId))//dwPropId值必须先置为0 
		{ 
			//循环开始执行,属性值被找到 
			printf("Property%d found-> ",dwPropId); 

			//------------------------------------------------ ------------------- 
			//Indicatethekindofpropertyfound. 
			switch(dwPropId) 
			{ 
			case CERT_FRIENDLY_NAME_PROP_ID: 
				{ 
					printf("FRIENDLY_NAME:"); 

					char szName[100]={0};
					DWORD dwNameLen=100;
					CRYPT_DATA_BLOB  Friendly_Name_Blob = {100,(BYTE*)szName };
					//CertSetCertificateContextProperty(pCertContext, CERT_FRIENDLY_NAME_PROP_ID, 0, &Friendly_Name_Blob));
					CertGetCertificateContextProperty(pCertContext, CERT_FRIENDLY_NAME_PROP_ID, szName, &dwNameLen);
					
					//mbstowcs(wsName, s, sizeof(s));
					wcstombs(szName,(const wchar_t *)szName,dwNameLen);
					
					printf("%s",szName);

					break; 
				} 
			case CERT_SIGNATURE_HASH_PROP_ID:
				{ 
					printf("Signaturehashidentifier"); 
					break; 
				} 
			case CERT_KEY_PROV_HANDLE_PROP_ID: 
				{ 
					printf("KEYPROVEHANDLE"); 
					break; 
				} 
			case CERT_KEY_PROV_INFO_PROP_ID: 
				{ 
					printf("KEYPROVINFOPROPID"); 
					break; 
				} 
			case CERT_SHA1_HASH_PROP_ID: 
				{ 
					printf("SHA1HASHidentifier"); 
					break; 
				} 
			case CERT_MD5_HASH_PROP_ID: 
				{ 
					printf("md5hashidentifier"); 
					break; 
				} 
			case CERT_KEY_CONTEXT_PROP_ID: 
				{ 
					printf("KEYCONTEXTPROPidentifier"); 
					break; 
				} 
			case CERT_KEY_SPEC_PROP_ID: 
				{ 
					printf("KEYSPECPROPidentifier"); 
					break; 
				} 
			case CERT_ENHKEY_USAGE_PROP_ID: 
				{ 
					printf("ENHKEYUSAGEPROPidentifier"); 
					break; 
				} 
			case CERT_NEXT_UPDATE_LOCATION_PROP_ID: 
				{ 
					printf("NEXTUPDATELOCATIONPROPidentifier"); 
					break; 
				} 
			case CERT_PVK_FILE_PROP_ID: 
				{ 
					printf("PVKFILEPROPidentifier"); 
					break; 
				} 
			case CERT_DESCRIPTION_PROP_ID: 
				{ 
					printf("DESCRIPTIONPROPidentifier"); 
					break; 
				} 
			case CERT_ACCESS_STATE_PROP_ID: 
				{ 
					printf("ACCESSSTATEPROPidentifier"); 
					break; 
				} 
			case CERT_SMART_CARD_DATA_PROP_ID: 
				{ 
					printf("SMART_CARDDATAPROPidentifier"); 
					break; 
				} 
			case CERT_EFS_PROP_ID: 
				{ 
					printf("EFSPROPidentifier"); 
					break; 
				} 
			case CERT_FORTEZZA_DATA_PROP_ID: 
				{ 
					printf("FORTEZZADATAPROPidentifier"); 
					break; 
				} 
			case CERT_ARCHIVED_PROP_ID: 
				{ 
					printf("ARCHIVEDPROPidentifier"); 
					break; 
				} 
			case CERT_KEY_IDENTIFIER_PROP_ID: 
				{ 
					printf("KEYIDENTIFIERPROPidentifier"); 
					break;
				} 
			case CERT_AUTO_ENROLL_PROP_ID: 
				{ 
					printf("AUTOENROLLidentifier."); 
					break; 
				} 
			}//End switch. 
			printf("\n"); 
		}//End inner while. 	

		PCCERT_CONTEXT  pDupCertContext; 
		pDupCertContext = CertDuplicateCertificateContext(pCertContext);
		if(pDupCertContext)
		{
			CertDeleteCertificateFromStore(pDupCertContext);
		}
	}

		
	CertFreeCertificateContext(pCertContext);
	CertCloseStore(hCertStore, CERT_CLOSE_STORE_FORCE_FLAG);

	return nRet;
}



void main(void) 
{ 

	OpenssLParsePkcs12Cert("D:\\Git\\test\\VSTest\\paPfx\\12345678.p12","12345678" );
	 ParsePkcs12Cert("G:\\工具\\Key工具\\证书\\TestOrg2-12345678.p12","12345678" );

	 /*
	HCERTSTORE hCertStore; 

	PCCERT_CONTEXT pCertContext=NULL; 

	char pszNameString[256]; 

	char pszStoreName[256]="MY"; 

	DWORD dwPropId=0; 

	char thumb[100]=""; 

	char temstring[10]; 

	pCertContext=NULL; 


	if(hCertStore=CertOpenSystemStore( NULL,pszStoreName)) 
	{ 
		fprintf(stderr,"Thesstorehasbeenopened.\n",pszStoreName); 
	} 
	else 
	{ 
		MyHandleError("The store was not opened."); 
	} 

	//使用CertEnumCertificatesInStore从存储区获取证书,pCertContext必须置为NULL才能找到第一个证书 

	while(pCertContext=CertEnumCertificatesInStore( hCertStore, pCertContext)) 
	{ 

	 //打印证书名称 
	 if(CertGetNameString(pCertContext, CERT_NAME_RDN_TYPE, 0, NULL, pszNameString, 128)) 
			printf("\nCertificatefors\n",pszNameString); 

		//遍历指定证书所有属性标识 

		while(dwPropId=CertEnumCertificateContextProperties( pCertContext, dwPropId))//dwPropId值必须先置为0 
		{ 
			//循环开始执行,属性值被找到 
			printf("Property#dfound-> %d",dwPropId); 

		    //------------------------------------------------ ------------------- 
			//Indicatethekindofpropertyfound. 
			switch(dwPropId) 
			{ 
				case CERT_FRIENDLY_NAME_PROP_ID: 
				{ 
					printf("Displayname:"); 
					break; 
				} 
				case CERT_SIGNATURE_HASH_PROP_ID:
				{ 
					printf("Signaturehashidentifier"); 
					break; 
				} 
				case CERT_KEY_PROV_HANDLE_PROP_ID: 
				{ 
					printf("KEYPROVEHANDLE"); 
					break; 
				} 
				case CERT_KEY_PROV_INFO_PROP_ID: 
				{ 
					printf("KEYPROVINFOPROPID"); 
					break; 
				} 
				case CERT_SHA1_HASH_PROP_ID: 
				{ 
					printf("SHA1HASHidentifier"); 
					break; 
				} 
				case CERT_MD5_HASH_PROP_ID: 
				{ 
					printf("md5hashidentifier"); 
					break; 
				} 
				case CERT_KEY_CONTEXT_PROP_ID: 
				{ 
					printf("KEYCONTEXTPROPidentifier"); 
					break; 
				} 
				case CERT_KEY_SPEC_PROP_ID: 
				{ 
					printf("KEYSPECPROPidentifier"); 
					break; 
				} 
				case CERT_ENHKEY_USAGE_PROP_ID: 
				{ 
					printf("ENHKEYUSAGEPROPidentifier"); 
					break; 
				} 
				case CERT_NEXT_UPDATE_LOCATION_PROP_ID: 
				{ 
					printf("NEXTUPDATELOCATIONPROPidentifier"); 
					break; 
				} 
				case CERT_PVK_FILE_PROP_ID: 
				{ 
					printf("PVKFILEPROPidentifier"); 
					break; 
				} 
				case CERT_DESCRIPTION_PROP_ID: 
				{ 
					printf("DESCRIPTIONPROPidentifier"); 
					break; 
				} 
				case CERT_ACCESS_STATE_PROP_ID: 
				{ 
					printf("ACCESSSTATEPROPidentifier"); 
					break; 
				} 
				case CERT_SMART_CARD_DATA_PROP_ID: 
				{ 
					printf("SMART_CARDDATAPROPidentifier"); 
					break; 
				} 
				case CERT_EFS_PROP_ID: 
				{ 
					printf("EFSPROPidentifier"); 
					break; 
				} 
				case CERT_FORTEZZA_DATA_PROP_ID: 
				{ 
					printf("FORTEZZADATAPROPidentifier"); 
					break; 
				} 
				case CERT_ARCHIVED_PROP_ID: 
				{ 
					printf("ARCHIVEDPROPidentifier"); 
					break; 
				} 
				case CERT_KEY_IDENTIFIER_PROP_ID: 
				{ 
					printf("KEYIDENTIFIERPROPidentifier"); 
					break;
				} 
				case CERT_AUTO_ENROLL_PROP_ID: 
				{ 
					printf("AUTOENROLLidentifier."); 
					break; 
				} 
			}//Endswitch. 
			printf("\n"); 
		}//Endinnerwhile. 
	}//Endouterwhile. 
	//------------------------------------------------ ------------------- 
	//Selectanewcertificatebyusingtheuserinterface. 
	//使用UI选择一个新证书 
	if(!(pCertContext=CryptUIDlgSelectCertificateFromStore( hCertStore, NULL, NULL, NULL,CRYPTUI_SELECT_LOCATION_COLUMN, 0, NULL))) 
	{ 
		MyHandleError("SelectUIfailed."); 
	} 
	else 
	{ 
		//显示名称 
		if(CertGetNameString( pCertContext, CERT_NAME_RDN_TYPE, 0, NULL, pszNameString, 128)) 
		{ 
			printf("\nCertificatefors\n",pszNameString); 
			LPBYTE pEncodedBytes=NULL; 
			LPBYTE pHash; 
			DWORD cbData,i; 
			pHash=NULL; 
			cbData=0; 
			CertGetCertificateContextProperty(pCertContext,CERT_HASH_PROP_ID,NULL,&cbData); 
			
			if(cbData==0) 
			{ 
				MyHandleError("CertGetCertificateContextProperty1f ailed"); 
			} 

			pHash=(LPBYTE)HeapAlloc(GetProcessHeap(),0,cbData) ; 
			if(pHash==NULL) 
			{ 
				MyHandleError("HeapAllocfailed"); 
			} 

			if(!CertGetCertificateContextProperty(pCertContext ,CERT_HASH_PROP_ID,pHash,&cbData)) 
			{ 
				MyHandleError("CertGetCertificateContextProperty2f ailed"); 
			} 

			printf("CERT_HASH_PROP_IDLengthisd\n",cbData); 
			for(i=0;i<cbData;i++) 
			{ 
				sprintf(temstring,"02x",pHash); 
				strcat(thumb,temstring); 
			} 

			printf("Thethumbiss",thumb); 
		} 
		else 
			fprintf(stderr,"CertGetNamefailed.\n"); 
	} 

	//------------------------------------------------ ------------------- 
	//Cleanup.
	CertFreeCertificateContext(pCertContext); 
	CertCloseStore(hCertStore,0); 
	*/
}//Endofmain. 

void MyHandleError( char* psz) 
{ 

	_ftprintf(stderr,TEXT("Anerroroccurredintheprogram .\n")); 

	_ftprintf(stderr,TEXT("s\n"),psz); 

	_ftprintf(stderr,TEXT("Errornumberx.\n"),GetLastError()); 

	_ftprintf(stderr,TEXT("Programterminating.\n")); 

	exit(1); 

}//EndofMyHandleError.