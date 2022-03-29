#include "Gm3000Ksp.h"
#include <middleware\gm_api\skfapi.h>
#include <middleware\mk_lib\mk_logger.h>

static BYTE s_bMD5DigestInfo[] =
{
    0x30, 0x20, 0x30, 0x0C, 0x06, 0x08, 0x2A, 0x86,
    0x48, 0x86, 0xF7, 0x0D, 0x02, 0x05, 0x05, 0x00,
    0x04, 0x10
};

static BYTE s_bShaDigestInfo[] =
{
    0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E,
    0x03, 0x02, 0x1A, 0x05, 0x00, 0x04, 0x14
};

static BYTE s_bSha256DigestInfo[] =
{
    0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
    0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05,
    0x00, 0x04, 0x20
};

static BYTE s_bSha384DigestInfo[] =
{
    0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
    0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02, 0x05,
    0x00, 0x04, 0x30
};

static BYTE s_bSha512DigestInfo[] =
{
    0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
    0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05,
    0x00, 0x04, 0x40
};


SECURITY_STATUS NormalizeNteStatus(__in NTSTATUS NtStatus)
{
    SECURITY_STATUS SecStatus;

    switch (NtStatus)
    {
    case STATUS_SUCCESS:
        SecStatus = ERROR_SUCCESS;
        break;

    case STATUS_NO_MEMORY:
    case STATUS_INSUFFICIENT_RESOURCES:
        SecStatus = NTE_NO_MEMORY;
        break;

    case STATUS_INVALID_PARAMETER:
        SecStatus = NTE_INVALID_PARAMETER;
        break;

    case STATUS_INVALID_HANDLE:
        SecStatus = NTE_INVALID_HANDLE;
        break;

    case STATUS_BUFFER_TOO_SMALL:
        SecStatus = NTE_BUFFER_TOO_SMALL;
        break;

    case STATUS_NOT_SUPPORTED:
        SecStatus = NTE_NOT_SUPPORTED;
        break;

    case STATUS_INTERNAL_ERROR:
    case ERROR_INTERNAL_ERROR:
        SecStatus = NTE_INTERNAL_ERROR;
        break;

    case STATUS_INVALID_SIGNATURE:
        SecStatus = NTE_BAD_SIGNATURE;
        break;

    default:
        SecStatus = NTE_INTERNAL_ERROR;
        break;
    }

    return SecStatus;
}
BOOL UTF8ToAsni(LPSTR lpszStr, LPSTR lpcszStr, DWORD dwSize)
{
    DWORD dwMinSize = MultiByteToWideChar(CP_UTF8, NULL, lpszStr, -1, NULL, 0);
    if (dwSize < dwMinSize)
        return FALSE;
    
    wchar_t* pWide = (wchar_t*)calloc((dwMinSize + 1) * sizeof(wchar_t),1);
    MultiByteToWideChar(CP_UTF8, NULL, lpszStr, -1, pWide, dwMinSize);

    dwMinSize = WideCharToMultiByte(CP_ACP, NULL, pWide, -1, NULL, 0, NULL, FALSE);
    if (dwSize < dwMinSize)
        return FALSE;

    WideCharToMultiByte(CP_ACP, NULL, pWide, -1, lpcszStr, dwSize, NULL, FALSE);
    return TRUE;
}

BOOL UnicodeToAsni(LPCWSTR lpszStr, LPSTR lpcszStr, DWORD dwSize)
{
    DWORD dwMinSize = WideCharToMultiByte(CP_ACP, NULL, lpszStr, -1, NULL, 0, NULL, FALSE);
    if (dwSize < dwMinSize)
        return FALSE;

    WideCharToMultiByte(CP_ACP, NULL, lpszStr, -1, lpcszStr, dwSize, NULL, FALSE);
    return TRUE;
}

PWSTR AlgorithmNames[1] = {
    NCRYPT_KEY_STORAGE_ALGORITHM
};

//
// Definition of ONE class of algorithms supported by the provider...
//
CRYPT_INTERFACE_REG AlgorithmClass = {
    NCRYPT_KEY_STORAGE_INTERFACE,       // ncrypt key storage interface
    CRYPT_LOCAL,                        // Scope: local system only
    1,                                  // One algorithm in the class
    AlgorithmNames                      // The name(s) of the algorithm(s) in the class
};

//
// An array of ALL the algorithm classes supported by the provider...
//
PCRYPT_INTERFACE_REG AlgorithmClasses[1] = {
    &AlgorithmClass
};

//
// Definition of the provider's user-mode binary...
//
CRYPT_IMAGE_REG GM3000KspImage = {
    GM3000_KSP_BINARY,                   // File name of the GM3000 KSP binary
    1,                                  // Number of algorithm classes the binary supports
    AlgorithmClasses                    // List of all algorithm classes available
};

#ifdef  GM3000_KSP_PROVIDER_ALIASE_NAME
PWSTR ALIASENames[1] = {
    GM3000_KSP_PROVIDER_ALIASE_NAME
};

CRYPT_PROVIDER_REG GM3000KSPProvider = {
    1,                  //1,
    ALIASENames,        //An array of null - terminated Unicode strings that contains the aliases of the provider.
    &GM3000KspImage,  // Image that provides user-mode support
    NULL              // Image that provides kernel-mode support (*MUST* be NULL)
};
#else
CRYPT_PROVIDER_REG GM3000KSPProvider = {
    0,                  //1,
    NULL,               //An array of null - terminated Unicode strings that contains the aliases of the provider.
    &GM3000KspImage,  // Image that provides user-mode support
    NULL              // Image that provides kernel-mode support (*MUST* be NULL)
};
#endif //  GM3000_KSP_PROVIDER_ALIASE_NAME

DWORD WINAPI GM3000EnumKsp(void)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    DWORD cbBuffer = 0;
    PCRYPT_PROVIDERS pBuffer = NULL;
    DWORD i = 0;

    TCHAR szBuf[2048] = {0};
    ntStatus = BCryptEnumRegisteredProviders(&cbBuffer, &pBuffer);

    if (NT_SUCCESS(ntStatus))
    {
        if (pBuffer == NULL)
        {
            //wprintf(L"BCryptEnumRegisteredProviders returned a NULL ptr\n");
            return GetLastError();
        }
        else
        {
            for (i = 0; i < pBuffer->cProviders; i++)
            {
                //wprintf(L"%s\n", pBuffer->rgpszProviders[i]);
                wsprintf(szBuf + wcslen(szBuf), TEXT("[%d] %s\n"), i + 1, pBuffer->rgpszProviders[i]);
                //sprintf(szBuf + strlen(szBuf), TEXT("[%d] %s\n"), i + 1, pBuffer->rgpszProviders[i]);
            }
            MessageBox(0, szBuf, TEXT("Key Storage Providers"), MB_OK);
        }
    }
    else
    {
        //wprintf(L"BCryptEnumRegisteredProviders failed with error code 0x%08x\n", ntStatus);
        return ntStatus;
    }

    if (pBuffer != NULL)
    {
        BCryptFreeBuffer(pBuffer);
    }
    return ntStatus;
}


DWORD WINAPI GM3000RegKsp(__in LPCWSTR pszProviderName)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    // Make CNG aware that our provider exists...
    //计算机\HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\Cryptography\Providers\CSP名
    //计算机\HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Cryptography\Providers\CSP名
    ntStatus = BCryptRegisterProvider(
        pszProviderName,//GM3000_KSP_PROVIDER_NAME,
        CRYPT_OVERWRITE,//0, // Flags: fail if provider is already registered  CRYPT_OVERWRITE
        &GM3000KSPProvider
    );

    if (!NT_SUCCESS(ntStatus))
    {
       // wprintf(L"BCryptRegisterProvider failed with error code 0x%08x\n", ntStatus);
        return ntStatus;
    }


    //
    // Add the algorithm name to the priority list of the symmetric cipher algorithm class. 
    // (This makes it visible to BCryptResolveProviders.)
    //
    //计算机\HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\Cryptography\Configuration\Local\Default\00010001\KEY_STORAGE
    //计算机\HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Cryptography\Configuration\Local\Default\00010001\KEY_STORAGE
    ntStatus = BCryptAddContextFunction(
        CRYPT_LOCAL,                    // Scope: local machine only
        0,                              // Application context: default
        NCRYPT_KEY_STORAGE_INTERFACE,   // Algorithm class
        NCRYPT_KEY_STORAGE_ALGORITHM,   // Algorithm name
        CRYPT_PRIORITY_BOTTOM           // Lowest priority
    );
    if (!NT_SUCCESS(ntStatus))
    {
        //wprintf(L"BCryptAddContextFunction failed with error code 0x%08x\n", ntStatus);
        return ntStatus;
    }


    //
    // Identify our new provider as someone who exposesan implementation of the new algorithm.
    //
    ntStatus = BCryptAddContextFunctionProvider(
        CRYPT_LOCAL,                    // Scope: local machine only
        0,                               // Application context: default
        NCRYPT_KEY_STORAGE_INTERFACE,   // Algorithm class
        NCRYPT_KEY_STORAGE_ALGORITHM,   // Algorithm name
        pszProviderName,//GM3000_KSP_PROVIDER_NAME,        // Provider name
        CRYPT_PRIORITY_BOTTOM           // Lowest priority
    );
    if (!NT_SUCCESS(ntStatus))
    {
        //wprintf(L"BCryptAddContextFunctionProvider failed with error code 0x%08x\n", ntStatus);
        return ntStatus;
    }
    return ntStatus;
}

DWORD WINAPI GM3000UnRegKsp(__in LPCWSTR pszProviderName)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    // Tell CNG that this provider no longer supports this algorithm.
    ntStatus = BCryptRemoveContextFunctionProvider(
        CRYPT_LOCAL,                    // Scope: local machine only
        0,                              // Application context: default
        NCRYPT_KEY_STORAGE_INTERFACE,   // Algorithm class
        NCRYPT_KEY_STORAGE_ALGORITHM,   // Algorithm name
        pszProviderName//GM3000_KSP_PROVIDER_NAME         // Provider
    );
    if (!NT_SUCCESS(ntStatus))
    {
        //wprintf(L"BCryptRemoveContextFunctionProvider failed with error code 0x%08x\n", ntStatus);
        return ntStatus;
    }


    // Tell CNG to forget about our provider component.
    ntStatus = BCryptUnregisterProvider(pszProviderName);
    if (!NT_SUCCESS(ntStatus))
    {
        //wprintf(L"BCryptUnregisterProvider failed with error code 0x%08x\n", ntStatus);
        return ntStatus;
    }
    return ntStatus;
}

///////////////////////////////////////////////////////////////////////////////
//
// Ncrypt key storage provider function table
//
///////////////////////////////////////////////////////////////////////////////
NCRYPT_KEY_STORAGE_FUNCTION_TABLE GM3000KSPFunctionTable =
{
    GM3000_KSP_INTERFACE_VERSION,
    GM3000KSPOpenProvider,
    GM3000KSPOpenKey,
    GM3000KSPCreatePersistedKey,
    GM3000KSPGetProviderProperty,
    GM3000KSPGetKeyProperty,
    GM3000KSPSetProviderProperty,
    GM3000KSPSetKeyProperty,
    GM3000KSPFinalizeKey,
    GM3000KSPDeleteKey,
    GM3000KSPFreeProvider,
    GM3000KSPFreeKey,
    GM3000KSPFreeBuffer,
    GM3000KSPEncrypt,
    GM3000KSPDecrypt,
    GM3000KSPIsAlgSupported,
    GM3000KSPEnumAlgorithms,
    GM3000KSPEnumKeys,
    GM3000KSPImportKey,
    GM3000KSPExportKey,
    GM3000KSPSignHash,
    GM3000KSPVerifySignature,
    GM3000KSPPromptUser,
    GM3000KSPNotifyChangeKey,
    GM3000KSPSecretAgreement,
    GM3000KSPDeriveKey,
    GM3000KSPFreeSecret
};

///////////////////////////////////////////////////////////////////////////////
/******************************************************************************
* DESCRIPTION :     Get the GM3000 KSP key storage Interface function
*                   dispatch table
*
* INPUTS :
*            LPCWSTR pszProviderName        Name of the provider (unused)
*            DWORD   dwFlags                Flags (unused)
* OUTPUTS :
*            char    **ppFunctionTable      The key storage interface function
*                                           dispatch table
* RETURN :
*            ERROR_SUCCESS                  The function was successful.
*/
NTSTATUS
WINAPI
GetKeyStorageInterface(
    __in   LPCWSTR pszProviderName,
    __out  NCRYPT_KEY_STORAGE_FUNCTION_TABLE** ppFunctionTable,
    __in   DWORD dwFlags)
{

    UNREFERENCED_PARAMETER(pszProviderName); //忽略编译器警告未引用的参数
    UNREFERENCED_PARAMETER(dwFlags);

    *ppFunctionTable = &GM3000KSPFunctionTable;

    return ERROR_SUCCESS;
}




//provider handle
typedef __struct_bcount(sizeof(GM3000KSP_PROVIDER)) struct _GM3000KSP_PROVIDER
{
    DEVHANDLE           hDev;
    WCHAR               pszName[MAXUSHORT];    //name of the KSP
    CHAR                pSN[MAXUSHORT];
    HAPPLICATION        hApp;
    CHAR                pApp[MAXUSHORT];
    WCHAR               pConName[MAXUSHORT];
    DWORD               dwKeySpec;
    BOOL                bSlient;

}GM3000KSP_PROVIDER;



vector < GM3000KSP_PROVIDER* > g_vcCryptProvs;

static NCRYPT_PROV_HANDLE* AddProv(LPCWSTR pszProviderName, LPSTR pSN=NULL, DEVHANDLE hDev = NULL, LPSTR pApp = NULL, HAPPLICATION hApp = NULL)
{
    GM3000KSP_PROVIDER* pProvider = NULL;
    size_t cbProviderName = 0;
    size_t count = g_vcCryptProvs.size();

    if (NULL != pszProviderName)
    {
        cbProviderName = (wcslen(pszProviderName) + 1) * sizeof(WCHAR);
        if (cbProviderName > MAXUSHORT)
            return false;

        
        for (size_t i = 0; i < count; i++)
        {
            if (0 == wcscmp(pszProviderName, g_vcCryptProvs[i]->pszName))
            {
                return (NCRYPT_PROV_HANDLE*)g_vcCryptProvs[i];
            }
        }

        pProvider = (GM3000KSP_PROVIDER*)HeapAlloc(GetProcessHeap(), 0, sizeof(GM3000KSP_PROVIDER));
        if (NULL == pProvider)
            return false;

        ZeroMemory(pProvider, sizeof(GM3000KSP_PROVIDER));
        CopyMemory(pProvider->pszName, pszProviderName, cbProviderName);

        if(pApp)
        strcpy(pProvider->pApp, pApp);
        if (hApp)
        pProvider->hApp = hApp;
        if (pSN)
        strcpy(pProvider->pSN, pSN);
        if (hDev)
        pProvider->hDev = hDev;

        //加锁
        g_vcCryptProvs.push_back(pProvider);

        return (NCRYPT_PROV_HANDLE*)pProvider;
    }
   
    return NULL;
}

static GM3000KSP_PROVIDER* FindProv(NCRYPT_PROV_HANDLE prov)
{
    size_t count = g_vcCryptProvs.size();
    for (size_t i = 0; i < count; i++)
    {
        if (prov == (NCRYPT_PROV_HANDLE)g_vcCryptProvs[i])
        {
            return g_vcCryptProvs[i];
        }
    }

    return NULL;
}

static void RemoveProv(NCRYPT_PROV_HANDLE prov)
{
    //加锁
    vector < GM3000KSP_PROVIDER* >::iterator it = g_vcCryptProvs.begin();
    for (; it != g_vcCryptProvs.end(); it++)
    {
        if (prov == (NCRYPT_PROV_HANDLE)*it)
        {
          //  SKF_DisConnectDev(((GM3000KSP_PROVIDER*)*it)->hDev);

            HeapFree(GetProcessHeap(), 0, *it);
            g_vcCryptProvs.erase(it);
            return;
        }
    }

    return;
}

static void ClearProvs()
{
    vector < GM3000KSP_PROVIDER* >::iterator it = g_vcCryptProvs.begin();
    for (; it != g_vcCryptProvs.end(); it++)
    {
      //  SKF_DisConnectDev(((GM3000KSP_PROVIDER*)*it)->hDev);
        HeapFree(GetProcessHeap(), 0, *it);
        return;
    }

    g_vcCryptProvs.clear();
    return;
}






#define SHOWMSG(T) {MessageBox(NULL, (T), L"func", MB_OK);}


/*******************************************************************
* DESCRIPTION :     Load and initialize the GM3000 KSP provider
*
* INPUTS :
*            LPCWSTR pszProviderName         Name of the provider
*            DWORD   dwFlags                 Flags (unused)
* OUTPUTS :
*            NCRYPT_PROV_HANDLE *phProvider  The provider handle
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*/
SECURITY_STATUS
WINAPI
GM3000KSPOpenProvider(
    __out   NCRYPT_PROV_HANDLE* phProvider,
    __in    LPCWSTR pszProviderName,
    __in    DWORD   dwFlags)
{
    SECURITY_STATUS status = ERROR_SUCCESS;
    UNREFERENCED_PARAMETER(dwFlags);
    //size_t cbProviderName = 0;

    NCRYPT_PROV_HANDLE* tmpProv = NULL;
    SHOWMSG(L"GM3000KSPOpenProvider")

    do
    {
        if (phProvider == NULL)
        {
            status = NTE_INVALID_PARAMETER;
            break;
        }
        
        tmpProv = AddProv(pszProviderName);
        if (NULL == tmpProv)
        {
            status = NTE_NO_MEMORY;
            break;
        }

        *phProvider = (NCRYPT_PROV_HANDLE)tmpProv;

    } while (0);

#if 0
    ULONG uRet = 0;
    DEVHANDLE hDev = 0;
    char szNameList[256] = { 0 };
    ULONG ulSize = sizeof(szNameList);
    HAPPLICATION hApp = 0;
    char szAppName[256] = { 0 };

    do
    {
        if (phProvider == NULL)
        {
            status = NTE_INVALID_PARAMETER;
            break;
        }


        ulSize = sizeof(szNameList);
        uRet = SKF_EnumDev(true, szNameList, &ulSize);
        if (0 != uRet || 0 == ulSize)
        {
            //no key
            status = NTE_DEVICE_NOT_READY;
            break;
        }

        //使用第一把key 第一个应用

        uRet = SKF_ConnectDev(szNameList, &hDev);
        if (0 != uRet)
        {
            status = uRet;
            break;
        }
        ulSize = sizeof(szAppName);
        uRet = SKF_EnumApplication(hDev, szAppName, &ulSize);
        if (0 != uRet || 0 == ulSize)
        {
            //no APP
            SKF_DisConnectDev(hDev);
            status = NTE_DEVICE_NOT_READY;
            break;
        }

        uRet = SKF_OpenApplication(hDev, szAppName, &hApp);
        if (0 != uRet)
        {
            SKF_DisConnectDev(hDev);
            status = uRet;
            break;
        }

        tmpProv = AddProv(pszProviderName, szNameList, hDev, szAppName, hApp);
        if (NULL == tmpProv)
        {
            status = NTE_NO_MEMORY;
            break;
        }

        *phProvider = (NCRYPT_PROV_HANDLE)tmpProv;

    } while (0);
#endif  
    return status;
}


/******************************************************************************
* DESCRIPTION :     Open a key in the GM3000 key storage provider
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the GM3000 KSP provider
*            LPCWSTR pszKeyName              Name of the key
             DWORD  dwLegacyKeySpec          Flags for legacy key support (unused)
*            DWORD   dwFlags                 Flags (unused)
* OUTPUTS:
*            NCRYPT_KEY_HANDLE               A handle to the opened key
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP provider handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*/
SECURITY_STATUS
WINAPI
GM3000KSPOpenKey(
    __inout NCRYPT_PROV_HANDLE hProvider,
    __out   NCRYPT_KEY_HANDLE* phKey,
    __in    LPCWSTR pszKeyName,
    __in_opt DWORD  dwLegacyKeySpec,
    __in    DWORD   dwFlags)
{
    SECURITY_STATUS Status = ERROR_SUCCESS;
    UNREFERENCED_PARAMETER(dwLegacyKeySpec);
    UNREFERENCED_PARAMETER(dwFlags);

    GM3000KSP_PROVIDER* gmProv = NULL;
    
    do
    {
        gmProv = FindProv(hProvider);
        if (NULL == gmProv)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        if ((phKey == NULL) || (pszKeyName == NULL))
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        DWORD cbKeyName = (wcslen(pszKeyName) + 1) * sizeof(WCHAR);
        if (cbKeyName > MAXUSHORT)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        if (AT_KEYEXCHANGE != dwLegacyKeySpec && AT_SIGNATURE != dwLegacyKeySpec)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }
        if (NCRYPT_SILENT_FLAG == (dwFlags & NCRYPT_SILENT_FLAG))
        {
            gmProv->bSlient = TRUE;
            //验证时不需要弹框
        }
        else
        {
            //每次都弹框
        }

        gmProv->dwKeySpec = dwLegacyKeySpec;

        CopyMemory(gmProv->pConName, pszKeyName, cbKeyName);

    } while (0);

#if 0
    ULONG uRet = 0;
    HCONTAINER hCon = 0;
    char szConName[128] = { 0 };

    do
    {
        gmProv = FindProv(hProvider);
        if (NULL == gmProv)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        if ((phKey == NULL) || (pszKeyName == NULL))
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        if(AT_KEYEXCHANGE != dwLegacyKeySpec && AT_SIGNATURE  != dwLegacyKeySpec)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }
        
        /*
        if (NCRYPT_MACHINE_KEY_FLAG != dwFlags && NCRYPT_SILENT_FLAG != dwFlags)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }
        */

        if (NCRYPT_SILENT_FLAG == (dwFlags & NCRYPT_SILENT_FLAG))
        {
            gmProv->bSlient = TRUE;
           //验证时不需要弹框
        }
        else
        {
            //每次都弹框
        }

        if (!UnicodeToAsni(pszKeyName, szConName, sizeof(szConName)))
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        uRet = SKF_OpenContainer(gmProv->hApp, szConName, &hCon);
        if (0 != uRet)
        {
            Status = uRet;
            break;
        }

        *phKey = (NCRYPT_KEY_HANDLE)hCon;

        //CRYPT_DATA_BLOB DataIn = { 0 };
        //BCRYPT_RSAKEY_BLOB* pCNGRSAKeyBlob = NULL;
        //CryptUnprotectData

    } while (0);
#endif
    return Status;
}

/******************************************************************************
* DESCRIPTION :     Create a new key and stored it into the user profile key storage area
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the GM3000 KSP provider
*            LPCWSTR pszAlgId                Cryptographic algorithm to create the key
*            LPCWSTR pszKeyName              Name of the key
*            DWORD  dwLegacyKeySpec          Flags for legacy key support (unused)
*            DWORD   dwFlags                 0|NCRYPT_OVERWRITE_KEY_FLAG
* OUTPUTS:
*            NCRYPT_KEY_HANDLE               A handle to the opened key
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider handle.
*            NTE_EXISTS                      The key already exists.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*            NTE_NOT_SUPPORTED               The algorithm is not supported.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*/
SECURITY_STATUS
WINAPI
GM3000KSPCreatePersistedKey(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __out   NCRYPT_KEY_HANDLE* phKey,
    __in    LPCWSTR pszAlgId,
    __in_opt LPCWSTR pszKeyName,
    __in    DWORD   dwLegacyKeySpec,
    __in    DWORD   dwFlags)
{
    SECURITY_STATUS       Status = ERROR_SUCCESS;
    NTSTATUS              ntStatus = STATUS_INTERNAL_ERROR;
    DWORD                 dwBitLength = 0;
    
    UNREFERENCED_PARAMETER(dwLegacyKeySpec);

    GM3000KSP_PROVIDER* gmProv = NULL;
    
    ULONG uRet = 0;
    HCONTAINER hCon = 0;
    char szConName[128] = { 0 };
#if 0
    do
    {
        gmProv = FindProv(hProvider);
        if (NULL == gmProv)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        if ((phKey == NULL) || (pszAlgId == NULL))
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        if (NULL == pszKeyName)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        if ((dwFlags & ~(NCRYPT_SILENT_FLAG | NCRYPT_OVERWRITE_KEY_FLAG)) != 0)
        {
            Status = NTE_BAD_FLAGS;
            break;
        }

        if (AT_KEYEXCHANGE != dwLegacyKeySpec && AT_SIGNATURE != dwLegacyKeySpec)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        if (!UnicodeToAsni(pszKeyName, szConName, sizeof(szConName)))
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }
   
        // 创建容器
        
        // If the overwrite flag is not set then check to make sure the key doesn't already exist.
        if (NCRYPT_OVERWRITE_KEY_FLAG == (dwFlags & NCRYPT_OVERWRITE_KEY_FLAG))
        {
            //判断是否可以重写
         //   if (!CryptExportKey(hKey, NULL, PUBLICKEYBLOB, 0, NULL, &dwBlobLen))  //public key blob length
            {
                Status = GetLastError();
                break;
            }
        }

        if (0 == wcscmp(pszAlgId, NCRYPT_RSA_ALGORITHM))
        {
            //生成密钥对
          //  if (!CryptGenKey(gmProv->hProv, dwLegacyKeySpec, 0, &hKey))
            {
                Status = GetLastError();
                break;
            }
        }/*
        else if(0 == wcscmp(pszAlgId, NCRYPT_RSA_SIGN_ALGORITHM))
        {

        }
        
        else if (0 == wcscmp(pszAlgId, NCRYPT_ECDSA_P256_ALGORITHM))
        {

        }
        else if (0 == wcscmp(pszAlgId, NCRYPT_ECDSA_P521_ALGORITHM))
        {

        }
        else if (0 == wcscmp(pszAlgId, NCRYPT_MD5_ALGORITHM))
        {

        }
        else if (0 == wcscmp(pszAlgId, NCRYPT_SHA1_ALGORITHM))
        {

        }
        else if (0 == wcscmp(pszAlgId, NCRYPT_SHA256_ALGORITHM))
        {

        }
        else if (0 == wcscmp(pszAlgId, NCRYPT_SHA384_ALGORITHM))
        {

        }
        else if (0 == wcscmp(pszAlgId, NCRYPT_SHA512_ALGORITHM))
        {

        }*/
        else
        {
            Status = NTE_NOT_SUPPORTED;
            break;
        }

        //*phKey = hKey;

    } while (0);
#endif
    return Status;
}


/******************************************************************************
* DESCRIPTION :  Retrieves the value of a named property for a key storage provider object.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the GM3000 KSP provider
*            LPCWSTR pszProperty             Name of the property
*            DWORD   cbOutput                Size of the output buffer
*            DWORD   dwFlags                 Flags
* OUTPUTS:
*            PBYTE   pbOutput                Output buffer containing the value of the property.  
*                                            If pbOutput is NULL,required buffer size will return in *pcbResult.
*            DWORD * pcbResult               Required size of the output buffer
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP provider handle.
*            NTE_NOT_FOUND                   Cannot find such a property.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BUFFER_TOO_SMALL            Output buffer is too small.
*            NTE_NOT_SUPPORTED               The property is not supported.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*/
SECURITY_STATUS
WINAPI
GM3000KSPGetProviderProperty(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    LPCWSTR pszProperty,
    __out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
    __in    DWORD   cbOutput,
    __out   DWORD* pcbResult,
    __in    DWORD   dwFlags)
{

    SECURITY_STATUS Status = ERROR_SUCCESS;
    return NTE_NOT_SUPPORTED;

    return Status;
}



/******************************************************************************
* DESCRIPTION :  Retrieves the value of a named property for a key storage key object.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a GM3000 KSP provider object
*            NCRYPT_KEY_HANDLE hKey          A handle to a GM3000 KSP key object
*            LPCWSTR pszProperty             Name of the property
*            DWORD   cbOutput                Size of the output buffer
*            DWORD   dwFlags                 Flags
* OUTPUTS:
*            PBYTE   pbOutput                Output buffer containing the value of the property.
*                                            If pbOutput is NULL,required buffer size will return in *pcbResult.    
*            DWORD * pcbResult               Required size of the output buffer
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP provider handle.
*            NTE_NOT_FOUND                   Cannot find such a property.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BUFFER_TOO_SMALL            Output buffer is too small.
*            NTE_NOT_SUPPORTED               The property is not supported.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*/
SECURITY_STATUS
WINAPI
GM3000KSPGetKeyProperty(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    NCRYPT_KEY_HANDLE hKey,
    __in    LPCWSTR pszProperty,
    __out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
    __in    DWORD   cbOutput,
    __out   DWORD* pcbResult,
    __in    DWORD   dwFlags)
{

    SECURITY_STATUS Status = ERROR_SUCCESS;
    return NTE_NOT_SUPPORTED;

    return Status;
}


/******************************************************************************
* DESCRIPTION :  Sets the value for a named property for a CNG key storage provider object.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the GM3000 KSP provider
*            LPCWSTR pszProperty             Name of the property
*            PBYTE   pbInput                 Input buffer containing the value of the property
*            DWORD   cbOutput                Size of the input buffer
*            DWORD   dwFlags                 Flags
*
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP provider handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_NOT_SUPPORTED               The property is not supported.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*/
SECURITY_STATUS
WINAPI
GM3000KSPSetProviderProperty(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    LPCWSTR pszProperty,
    __in_bcount(cbInput) PBYTE pbInput,
    __in    DWORD   cbInput,
    __in    DWORD   dwFlags)
{
    SECURITY_STATUS Status = ERROR_SUCCESS;
    return NTE_NOT_SUPPORTED;
    return Status;
}


/******************************************************************************
* DESCRIPTION :  Set the value of a named property for a key storage
*                key object.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a GM3000 KSP provider
*                                            object
*            NCRYPT_KEY_HANDLE hKey          A handle to a GM3000 KSP key object
*            LPCWSTR pszProperty             Name of the property
*            PBYTE   pbInput                 Input buffer containing the value
*                                            of the property
*            DWORD   cbOutput                Size of the input buffer
*            DWORD   dwFlags                 Flags
*
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider handle or a valid key handle
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_NOT_SUPPORTED               The property is not supported.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*/
SECURITY_STATUS
WINAPI
GM3000KSPSetKeyProperty(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __inout NCRYPT_KEY_HANDLE hKey,
    __in    LPCWSTR pszProperty,
    __in_bcount(cbInput) PBYTE pbInput,
    __in    DWORD   cbInput,
    __in    DWORD   dwFlags)
{

    SECURITY_STATUS         Status = ERROR_SUCCESS;
    return NTE_NOT_SUPPORTED;
    return Status;
}



/******************************************************************************
* DESCRIPTION :     Completes a GM3000 key storage key. The key cannot be used
*                   until this function has been called.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the GM3000 KSP provider
*            NCRYPT_KEY_HANDLE hKey          A handle to a GM3000 KSP key
*            DWORD   dwFlags                 Flags
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*            NTE_BAD_FLAGS                   The dwFlags parameter contains a
*                                            value that is not valid.
*/
SECURITY_STATUS
WINAPI
GM3000KSPFinalizeKey(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    NCRYPT_KEY_HANDLE hKey,
    __in    DWORD   dwFlags)
{
    SECURITY_STATUS Status = ERROR_SUCCESS;
    return NTE_NOT_SUPPORTED;
    return Status;
}


/******************************************************************************
* DESCRIPTION :     Deletes a CNG GM3000 KSP key
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the GM3000 KSP provider
*            NCRYPT_KEY_HANDLE hKey          Handle to a GM3000 KSP key
*            DWORD   dwFlags                 Flags
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider or key handle.
*            NTE_BAD_FLAGS                   The dwFlags parameter contains a
*                                            value that is not valid.
*            NTE_INTERNAL_ERROR              Key file deletion failed.
*/
SECURITY_STATUS
WINAPI
GM3000KSPDeleteKey(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __inout NCRYPT_KEY_HANDLE hKey,
    __in    DWORD   dwFlags)
{
    SECURITY_STATUS Status = ERROR_SUCCESS;

    GM3000KSP_PROVIDER* gmProv = NULL;

    do
    {
        gmProv = FindProv(hProvider);
        if (NULL == gmProv)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        //SKF_CloseContainer((HCONTAINER)hKey);

    } while (0);
    
    return Status;
}



/******************************************************************************
* DESCRIPTION :     Release a handle to the GM3000 KSP provider
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the GM3000 KSP provider
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider handle.
*/
SECURITY_STATUS
WINAPI
GM3000KSPFreeProvider(
    __in    NCRYPT_PROV_HANDLE hProvider)
{
    SECURITY_STATUS Status = ERROR_SUCCESS;

    GM3000KSP_PROVIDER* gmProv = NULL;

    RemoveProv(hProvider);
    
    return Status;
}


/******************************************************************************
* DESCRIPTION :     Free a CNG GM3000 KSP key object
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to the GM3000 KSP provider
*            NCRYPT_KEY_HANDLE hKey          A handle to a GM3000 KSP key
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*/
SECURITY_STATUS
WINAPI
GM3000KSPFreeKey(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    NCRYPT_KEY_HANDLE hKey)
{
    SECURITY_STATUS Status = ERROR_SUCCESS;
    
    return Status;
}



/******************************************************************************
* DESCRIPTION :     free a CNG GM3000 KSP memory buffer object
*
* INPUTS :
*            PVOID   pvInput                 The buffer to free.
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*/
SECURITY_STATUS
WINAPI
GM3000KSPFreeBuffer(
    __deref PVOID   pvInput)
{
    return ERROR_SUCCESS;
}


/******************************************************************************
* DESCRIPTION :  encrypts a block of data.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a GM3000 KSP provider
*                                            object.
*            NCRYPT_KEY_HANDLE hKey          A handle to a GM3000 KSP key object.
*            PBYTE   pbInput                 Plain text data to be encrypted.
*            DWORD   cbInput                 Size of the plain text data.
*            VOID    *pPaddingInfo           Padding information if padding sheme
*                                            is used.
*            DWORD   cbOutput                Size of the output buffer.
*            DWORD   dwFlags                 Flags
* OUTPUTS:
*            PBYTE   pbOutput                Output buffer containing encrypted
*                                            data.  If pbOutput is NULL,
*                                            required buffer size will return in
*                                            *pcbResult.
*            DWORD * pcbResult               Required size of the output buffer
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_BAD_KEY_STATE               The key identified by the hKey
*                                            parameter has not been finalized
*                                            or is incomplete.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider or key handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BUFFER_TOO_SMALL            Output buffer is too small.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*/
SECURITY_STATUS
WINAPI
GM3000KSPEncrypt(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    NCRYPT_KEY_HANDLE hKey,
    __in_bcount(cbInput) PBYTE pbInput,
    __in    DWORD   cbInput,
    __in    VOID* pPaddingInfo,
    __out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
    __in    DWORD   cbOutput,
    __out   DWORD* pcbResult,
    __in    DWORD   dwFlags)
{

    SECURITY_STATUS Status = ERROR_SUCCESS;
    NTSTATUS        ntStatus = STATUS_INTERNAL_ERROR;
    UNREFERENCED_PARAMETER(hProvider);

    return NTE_NOT_SUPPORTED;
    return Status;
}


/******************************************************************************
* DESCRIPTION :  Decrypts a block of data.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a GM3000 KSP provider
*                                            object.
*            NCRYPT_KEY_HANDLE hKey          A handle to a GM3000 KSP key object.
*            PBYTE   pbInput                 Encrypted data blob.
*            DWORD   cbInput                 Size of the encrypted data blob.
*            VOID    *pPaddingInfo           Padding information if padding sheme
*                                            is used.
*            DWORD   cbOutput                Size of the output buffer.
*            DWORD   dwFlags                 Flags
* OUTPUTS:
*            PBYTE   pbOutput                Output buffer containing decrypted
*                                            data.  If pbOutput is NULL,
*                                            required buffer size will return in
*                                            *pcbResult.
*            DWORD * pcbResult               Required size of the output buffer
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_BAD_KEY_STATE               The key identified by the hKey
*                                            parameter has not been finalized
*                                            or is incomplete.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider or key handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BUFFER_TOO_SMALL            Output buffer is too small.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*/

SECURITY_STATUS
WINAPI
GM3000KSPDecrypt(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    NCRYPT_KEY_HANDLE hKey,
    __in_bcount(cbInput) PBYTE pbInput,
    __in    DWORD   cbInput,
    __in    VOID* pPaddingInfo,
    __out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
    __in    DWORD   cbOutput,
    __out   DWORD* pcbResult,
    __in    DWORD   dwFlags)
{

    DWORD BlockLength = 0;
    SECURITY_STATUS Status = ERROR_SUCCESS;
    NTSTATUS    ntStatus = ERROR_SUCCESS;
    UNREFERENCED_PARAMETER(hProvider);

    return NTE_NOT_SUPPORTED;
    return Status;
}


/******************************************************************************
* DESCRIPTION :  Determines if a GM3000 key storage provider supports a
*                specific cryptographic algorithm.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a GM3000 KSP provider
*                                            object
*            LPCWSTR pszAlgId                Name of the cryptographic
*                                            Algorithm in question
*            DWORD   dwFlags                 Flags
* RETURN :
*            ERROR_SUCCESS                   The algorithm is supported.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider or key handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*            NTE_NOT_SUPPORTED               This algorithm is not supported.
*/
SECURITY_STATUS
WINAPI
GM3000KSPIsAlgSupported(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    LPCWSTR pszAlgId,
    __in    DWORD   dwFlags)
{

    SECURITY_STATUS Status = ERROR_SUCCESS;

    return NTE_NOT_SUPPORTED;
    return Status;
}

/******************************************************************************
* DESCRIPTION :  Obtains the names of the algorithms that are supported by
*                the GM3000 key storage provider.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a GM3000 KSP provider
*                                            object.
*            DWORD   dwAlgOperations         The crypto operations that are to
*                                            be enumerated.
*            DWORD   dwFlags                 Flags
*
* OUTPUTS:
*            DWORD * pdwAlgCount             Number of supported algorithms.
*            NCryptAlgorithmName **ppAlgList List of supported algorithms.
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*            NTE_NOT_SUPPORTED               The crypto operations are not supported.
*/
SECURITY_STATUS
WINAPI
GM3000KSPEnumAlgorithms(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    DWORD   dwAlgOperations,
    __out   DWORD* pdwAlgCount,
    __deref_out_ecount(*pdwAlgCount) NCryptAlgorithmName** ppAlgList,
    __in    DWORD   dwFlags)
{
    SECURITY_STATUS Status = ERROR_SUCCESS;
    
    return NTE_NOT_SUPPORTED;
    return Status;
}


/******************************************************************************
* DESCRIPTION :  Obtains the names of the keys that are stored by the provider.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a GM3000 KSP provider
*                                            object
*            LPCWSTR pszScope                Unused
*            NCryptKeyName **ppKeyName       Name of the retrieved key
*            PVOID * ppEnumState             Enumeration state information
*            DWORD   dwFlags                 Flags
*
* OUTPUTS:
*            PVOID * ppEnumState             Enumeration state information that
*                                            is used in subsequent calls to
*                                            this function.
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*            NTE_NOT_SUPPORTED               NCRYPT_MACHINE_KEY_FLAG is not
*                                            supported.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*/
SECURITY_STATUS
WINAPI
GM3000KSPEnumKeys(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in_opt LPCWSTR pszScope,
    __deref_out NCryptKeyName** ppKeyName,
    __inout PVOID* ppEnumState,
    __in    DWORD   dwFlags)
{

    PVOID pEnumState = NULL;
    SECURITY_STATUS Status = ERROR_SUCCESS;
    UNREFERENCED_PARAMETER(pszScope);
  
    return NTE_NOT_SUPPORTED;
    return Status;
}

/******************************************************************************
* DESCRIPTION :  Imports a key into the GM3000 KSP from a memory BLOB.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider     A handle to a GM3000 KSP provider
*                                             object.
*            NCRYPT_KEY_HANDLE hImportKey     Unused
*            LPCWSTR pszBlobType              Type of the key blob.
*            NCryptBufferDesc *pParameterList Additional parameter information.
*            PBYTE   pbData                   Key blob.
*            DWORD   cbData                   Size of the key blob.
*            DWORD   dwFlags                  Flags
*
* OUTPUTS:
*            NCRYPT_KEY_HANDLE *phKey        GM3000 KSP key object imported
*                                            from the key blob.
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*            NTE_NOT_SUPPORTED               The type of the key blob is not
*                                            supported.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*            NTE_INTERNAL_ERROR              Decoding failed.
*/
SECURITY_STATUS
WINAPI
GM3000KSPImportKey(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in_opt NCRYPT_KEY_HANDLE hImportKey,
    __in    LPCWSTR pszBlobType,
    __in_opt NCryptBufferDesc* pParameterList,
    __out   NCRYPT_KEY_HANDLE* phKey,
    __in_bcount(cbData) PBYTE pbData,
    __in    DWORD   cbData,
    __in    DWORD   dwFlags)
{

    DWORD                   cbResult = 0;
    BOOL                    fPkcs7Blob = FALSE;
    BOOL                    fPkcs8Blob = FALSE;
    BOOL                    fPrivateKeyBlob = FALSE;
    LPCWSTR                 pszTmpBlobType = pszBlobType;
    LPWSTR                  pszKeyName = NULL;
    SECURITY_STATUS         Status = ERROR_SUCCESS;
    NTSTATUS                ntStatus = STATUS_INTERNAL_ERROR;

    UNREFERENCED_PARAMETER(hImportKey);

    return NTE_NOT_SUPPORTED;
    return Status;
}

/******************************************************************************
* DESCRIPTION :  Exports a GM3000 key storage key into a memory BLOB.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider     A handle to a GM3000 KSP provider
*                                             object.
*            NCRYPT_KEY_HANDLE hKey           A handle to the GM3000 KSP key
*                                             object to export.
*            NCRYPT_KEY_HANDLE hExportKey     Unused
*            LPCWSTR pszBlobType              Type of the key blob.
*            NCryptBufferDesc *pParameterList Additional parameter information.
*            DWORD   cbOutput                 Size of the key blob.
*            DWORD   dwFlags                  Flags
*
* OUTPUTS:
*            PBYTE pbOutput                  Key blob.
*            DWORD * pcbResult               Required size of the key blob.
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*            NTE_NOT_SUPPORTED               The type of the key blob is not
*                                            supported.
*            NTE_NO_MEMORY                   A memory allocation failure occurred.
*            NTE_INTERNAL_ERROR              Encoding failed.
*/
SECURITY_STATUS
WINAPI
GM3000KSPExportKey(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    NCRYPT_KEY_HANDLE hKey,
    __in_opt NCRYPT_KEY_HANDLE hExportKey,
    __in    LPCWSTR pszBlobType,
    __in_opt NCryptBufferDesc* pParameterList,
    __out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
    __in    DWORD   cbOutput,
    __out   DWORD* pcbResult,
    __in    DWORD   dwFlags)
{

    PBYTE               pbTemp = NULL;
    BOOL                fPkcs7Blob = FALSE;
    BOOL                fPkcs8Blob = FALSE;
    BOOL                fPublicKeyBlob = FALSE;
    BOOL                fPrivateKeyBlob = FALSE;
    NTSTATUS            ntStatus = STATUS_INTERNAL_ERROR;
    SECURITY_STATUS     Status = ERROR_SUCCESS;
    UNREFERENCED_PARAMETER(hExportKey);

    return NTE_NOT_SUPPORTED;
    return Status;
}

/******************************************************************************
* DESCRIPTION :  creates a signature of a hash value.
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a GM3000 KSP provider object
*            NCRYPT_KEY_HANDLE hKey          A handle to a GM3000 KSP key object
*            VOID    *pPaddingInfo           Padding information is padding sheme is used
*                                            A pointer to a structure that contains padding information. 
*                                            The actual type of structure this parameter points to depends on the value of the dwFlags parameter.
*                                            This parameter is only used with asymmetric keys and must be NULL otherwise.*                                            
*            PBYTE  pbHashValue              Hash to sign.
*            DWORD  cbHashValue              Size of the hash.
*            DWORD  cbSignature              Size of the signature
*            DWORD  dwFlags                  Flags
*                                           If the key is a symmetric key, this parameter is not used and should be set to zero.
* OUTPUTS:
*            PBYTE  pbSignature              Output buffer containing signature.
*                                            If pbOutput is NULL, required buffer size will return in *pcbResult.
*            DWORD * pcbResult               Required size of the output buffer.
* RETURN :
*            ERROR_SUCCESS                   The function was successful.
*            NTE_BAD_KEY_STATE               The key identified by the hKey
*                                            parameter has not been finalized or is incomplete.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP provider or key handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BUFFER_TOO_SMALL            Output buffer is too small.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*/
SECURITY_STATUS
WINAPI
GM3000KSPSignHash(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    NCRYPT_KEY_HANDLE hKey,
    __in_opt    VOID* pPaddingInfo,
    __in_bcount(cbHashValue) PBYTE pbHashValue,
    __in    DWORD   cbHashValue,
    __out_bcount_part_opt(cbSignaturee, *pcbResult) PBYTE pbSignature,
    __in    DWORD   cbSignaturee,
    __out   DWORD* pcbResult,
    __in    DWORD   dwFlags)
{
    SECURITY_STATUS     Status = ERROR_SUCCESS;
    GM3000KSP_PROVIDER* gmProv = NULL;
    DWORD               cbTmpSig = 0;
    DWORD               cbTmp = 0;
    ULONG               uRet = 0;
    BCRYPT_PKCS1_PADDING_INFO* PkcsPadInfo = NULL;
    BCRYPT_PSS_PADDING_INFO* PssPadInfo = NULL;

    unsigned int               uiHashAlgo = 0;
    BYTE* pbKeyBlob = NULL;			//signer's public key
    HCRYPTHASH hHash = 0;
    HCRYPTKEY hTempKey = NULL;

    BYTE* pbTmpSignature = NULL;			//signature
    DWORD dwSigLen = 0;
    DWORD dwBlobLen = 0;
    HCRYPTPROV hProv = 0;

    do
    {
        gmProv = FindProv(hProvider);
        if (NULL == gmProv)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        if (pbHashValue == NULL || cbHashValue == 0)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        if (pcbResult == NULL)
        {
            Status = NTE_INVALID_PARAMETER;
            break;
        }

        if (dwFlags & ~(BCRYPT_PAD_PKCS1 | BCRYPT_PAD_PSS | NCRYPT_SILENT_FLAG))
        {
            Status = NTE_BAD_FLAGS;
            break;
        }

        switch (dwFlags)
        {
        case BCRYPT_PAD_PKCS1:
            PkcsPadInfo = (BCRYPT_PKCS1_PADDING_INFO*)pPaddingInfo;
            if (NULL == PkcsPadInfo->pszAlgId)
            {
                uiHashAlgo = 0; //raw
            }
            else if (wcscmp(PkcsPadInfo->pszAlgId, L"MD5") == 0)
            {
                uiHashAlgo = CALG_MD5;
            }
            else if (wcscmp(PkcsPadInfo->pszAlgId, L"SHA1") == 0)
            {
                uiHashAlgo = CALG_SHA1;
            }
            else if (wcscmp(PkcsPadInfo->pszAlgId, L"SHA256") == 0)
            {
                uiHashAlgo = CALG_SHA_256;
            }
            else if (wcscmp(PkcsPadInfo->pszAlgId, L"SHA384") == 0)
            {
                uiHashAlgo = CALG_SHA_384;
            }
            else if (wcscmp(PkcsPadInfo->pszAlgId, L"SHA512") == 0)
            {
                uiHashAlgo = CALG_SHA_512;
            }
            else
            {

            }
            break;
        case BCRYPT_PAD_PSS:
            PssPadInfo = (BCRYPT_PSS_PADDING_INFO*)pPaddingInfo;
            break;
        case NCRYPT_SILENT_FLAG:
            break;
        default:
            break;
        }

        if (pbSignature == NULL)
        {
            //计算签名后的长度
            /*
            NTSTATUS ntStatus = BCryptGetProperty(pKey->hPublicKey, BCRYPT_SIGNATURE_LENGTH,
                (BYTE*)&cbTmpSig, sizeof(DWORD), &cbTmp, 0);
            if (!NT_SUCCESS(ntStatus))
            {
                Status = NormalizeNteStatus(ntStatus);
                break;
            }
            */
            Status = ERROR_SUCCESS;
            *pcbResult = cbTmpSig;
        }
        else
        {

        }
        DWORD       dwFlags = 0;
        if (gmProv->bSlient)
            dwFlags |= CRYPT_SILENT;

        
        //CRYPT_VERIFYCONTEXT 没有传入容器名时使用，表示不使用硬件
        if (!CryptAcquireContext(&hProv, gmProv->pConName, gmProv->pszName, PROV_RSA_FULL, dwFlags))
        {
            Status = GetLastError();
            break;
        }
/*
        if (!CryptSetProvParam(hProv, PP_SIGNATURE_PIN, (const byte*)"12345678", strlen("12345678")))
        {
            Status = GetLastError();
            break;
        }

        if (!CryptGetUserKey(hProv, AT_SIGNATURE,&hTempKey))
        {
            Status = GetLastError();
            break;
        }

        

        if (CryptExportKey( hKey,NULL,PUBLICKEYBLOB,0,NULL,&dwBlobLen))  //public key blob length
        {
            Status = GetLastError();
            break;
        }
    
        if (pbKeyBlob = (BYTE*)new BYTE[dwBlobLen])
        {
            
        }
        else
        {
            Status = NTE_NO_MEMORY;
            break;
        }

        if (!CryptExportKey(hTempKey,NULL,PUBLICKEYBLOB,0,pbKeyBlob,&dwBlobLen))
        {
            Status = GetLastError();
            break;
        }
 */      
        // Create hash
        if (!CryptCreateHash(hProv, uiHashAlgo,0,0,&hHash))
        {
            Status = GetLastError();
            break;
        }
        
        // digest signed data
        if (!CryptHashData( hHash, pbHashValue, cbHashValue,0))
        {
            Status = GetLastError();
            break;
        }
       
        // sign digest with signature key

        dwSigLen = 0;
        if (!CryptSignHash( hHash,AT_SIGNATURE,NULL,0,NULL,&dwSigLen)) //get signature length
        {
            Status = GetLastError();
            break;
        }
        
        // malloc buffer to hold signature
        if (pbTmpSignature = (BYTE*)new BYTE[dwSigLen])
        {
        }
        else
        {
            Status = NTE_NO_MEMORY;
            break;
        }

        if (!CryptSignHash(hHash, AT_SIGNATURE,NULL,0, pbTmpSignature,   &dwSigLen))
        {
            Status = GetLastError();
            break;
        }

    } while (0);

    if (pbTmpSignature)
        delete[] pbTmpSignature;
    if(pbKeyBlob)
        delete[] pbKeyBlob;
    if (hHash)
        CryptDestroyHash(hHash);
    if (hProv)
        CryptReleaseContext(hProv, 0);
    return Status;
}

/******************************************************************************
* DESCRIPTION :  Verifies that the specified signature matches the specified hash
*
* INPUTS :
*            NCRYPT_PROV_HANDLE hProvider    A handle to a GM3000 KSP provider
*                                            object.
*            NCRYPT_KEY_HANDLE hKey          A handle to a GM3000 KSP key object
*            VOID    *pPaddingInfo           Padding information is padding sheme
*                                            is used.
*            PBYTE  pbHashValue              Hash data
*            DWORD  cbHashValue              Size of the hash
*            PBYTE  pbSignature              Signature data
*            DWORD  cbSignaturee             Size of the signature
*            DWORD  dwFlags                  Flags
*
* RETURN :
*            ERROR_SUCCESS                   The signature is a valid signature.
*            NTE_BAD_KEY_STATE               The key identified by the hKey
*                                            parameter has not been finalized
*                                            or is incomplete.
*            NTE_INVALID_HANDLE              The handle is not a valid GM3000 KSP
*                                            provider or key handle.
*            NTE_INVALID_PARAMETER           One or more parameters are invalid.
*            NTE_BAD_FLAGS                   dwFlags contains invalid value.
*/
SECURITY_STATUS
WINAPI
GM3000KSPVerifySignature(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    NCRYPT_KEY_HANDLE hKey,
    __in_opt    VOID* pPaddingInfo,
    __in_bcount(cbHashValue) PBYTE pbHashValue,
    __in    DWORD   cbHashValue,
    __in_bcount(cbSignaturee) PBYTE pbSignature,
    __in    DWORD   cbSignaturee,
    __in    DWORD   dwFlags)
{
    NTSTATUS    ntStatus = STATUS_INTERNAL_ERROR;
    SECURITY_STATUS Status = ERROR_SUCCESS;

    UNREFERENCED_PARAMETER(hProvider);
 

    return NTE_NOT_SUPPORTED;
    return Status;
}



SECURITY_STATUS
WINAPI
GM3000KSPPromptUser(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in_opt NCRYPT_KEY_HANDLE hKey,
    __in    LPCWSTR  pszOperation,
    __in    DWORD   dwFlags)
{
    UNREFERENCED_PARAMETER(hProvider);
    UNREFERENCED_PARAMETER(hKey);
    UNREFERENCED_PARAMETER(pszOperation);
    UNREFERENCED_PARAMETER(dwFlags);
    return NTE_NOT_SUPPORTED;
}


SECURITY_STATUS
WINAPI
GM3000KSPNotifyChangeKey(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __inout HANDLE* phEvent,
    __in    DWORD   dwFlags)
{
    UNREFERENCED_PARAMETER(hProvider);
    UNREFERENCED_PARAMETER(phEvent);
    UNREFERENCED_PARAMETER(dwFlags);
    return NTE_NOT_SUPPORTED;
}


SECURITY_STATUS
WINAPI
GM3000KSPSecretAgreement(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    NCRYPT_KEY_HANDLE hPrivKey,
    __in    NCRYPT_KEY_HANDLE hPubKey,
    __out   NCRYPT_SECRET_HANDLE* phAgreedSecret,
    __in    DWORD   dwFlags)
{
    UNREFERENCED_PARAMETER(hProvider);
    UNREFERENCED_PARAMETER(hPrivKey);
    UNREFERENCED_PARAMETER(hPubKey);
    UNREFERENCED_PARAMETER(phAgreedSecret);
    UNREFERENCED_PARAMETER(dwFlags);
    return NTE_NOT_SUPPORTED;
}

SECURITY_STATUS
WINAPI
GM3000KSPDeriveKey(
    __in        NCRYPT_PROV_HANDLE   hProvider,
    __in_opt    NCRYPT_SECRET_HANDLE hSharedSecret,
    __in        LPCWSTR              pwszKDF,
    __in_opt    NCryptBufferDesc* pParameterList,
    __out_bcount_part_opt(cbDerivedKey, *pcbResult) PUCHAR pbDerivedKey,
    __in        DWORD                cbDerivedKey,
    __out       DWORD* pcbResult,
    __in        ULONG                dwFlags)
{
    UNREFERENCED_PARAMETER(hProvider);
    UNREFERENCED_PARAMETER(hSharedSecret);
    UNREFERENCED_PARAMETER(pwszKDF);
    UNREFERENCED_PARAMETER(pParameterList);
    UNREFERENCED_PARAMETER(pbDerivedKey);
    UNREFERENCED_PARAMETER(cbDerivedKey);
    UNREFERENCED_PARAMETER(pcbResult);
    UNREFERENCED_PARAMETER(dwFlags);
    return NTE_NOT_SUPPORTED;
}

SECURITY_STATUS
WINAPI
GM3000KSPFreeSecret(
    __in    NCRYPT_PROV_HANDLE hProvider,
    __in    NCRYPT_SECRET_HANDLE hSharedSecret)
{
    UNREFERENCED_PARAMETER(hProvider);
    UNREFERENCED_PARAMETER(hSharedSecret);
    return NTE_NOT_SUPPORTED;
}