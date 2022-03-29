#ifndef _GM3000KSP_H_
#define _GM3000KSP_H_
#pragma once

#include <windows.h>
#include <wincrypt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <bcrypt.h>
#include <ncrypt.h>
#include <ncrypt_provider.h>
#include <bcrypt_provider.h>

#include <vector> 
#include <map> 
using namespace std;

//error handling
#ifndef NT_SUCCESS
#define NT_SUCCESS(status) (status >= 0)
#endif

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS                  ((NTSTATUS)0x00000000L)
#define STATUS_NOT_SUPPORTED            ((NTSTATUS)0xC00000BBL)
#define STATUS_BUFFER_TOO_SMALL         ((NTSTATUS)0xC0000023L)
#define STATUS_INSUFFICIENT_RESOURCES   ((NTSTATUS)0xC000009AL)
#define STATUS_INTERNAL_ERROR           ((NTSTATUS)0xC00000E5L)
#define STATUS_INVALID_SIGNATURE        ((NTSTATUS)0xC000A000L)
#endif

#ifndef STATUS_INVALID_PARAMETER
#define STATUS_INVALID_PARAMETER         ((NTSTATUS)0xC000000DL)
#endif



#define GM3000_KSP_INTERFACE_VERSION BCRYPT_MAKE_INTERFACE_VERSION(1,0) //version of the sample KSP interface
#define GM3000_KSP_VERSION 0x00010000                         //version of the sample KSP

//#define GM3000_KSP_PROVIDER_NAME			L"Longmai mToken GM3000 Key Storage Provider" //KSP名
//#define GM3000_KSP_PROVIDER_ALIASE_NAME		L"Longmai mToken GM3000 CSP V1.1"   //与CAPI中的私有csp名保持一致，通过这个别名找到cng
#define GM3000_KSP_PROVIDER_NAME			L"Longmai mToken GM3000 CSP V1.1"
#define GM3000_KSP_BINARY					L"GM3000CSP_CNG.dll"



#ifdef __cplusplus
extern "C" {
#endif

    DWORD WINAPI GM3000EnumKsp(void);
    DWORD WINAPI GM3000RegKsp(__in LPCWSTR pszProviderName); //注册与私有csp相同的别名
    DWORD WINAPI GM3000UnRegKsp(__in LPCWSTR pszProviderName);

    NTSTATUS WINAPI GetKeyStorageInterface(
        __in   LPCWSTR pszProviderName,
        __out  NCRYPT_KEY_STORAGE_FUNCTION_TABLE** ppFunctionTable,
        __in   DWORD dwFlags);


#define MAXUSHORT   128  //ksp名最大长度
    SECURITY_STATUS WINAPI GM3000KSPOpenProvider(
        __out   NCRYPT_PROV_HANDLE* phProvider,
        __in    LPCWSTR pszProviderName,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPOpenKey(
        __inout NCRYPT_PROV_HANDLE hProvider,
        __out   NCRYPT_KEY_HANDLE* phKey,
        __in    LPCWSTR pszKeyName,
        __in_opt DWORD  dwLegacyKeySpec,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPCreatePersistedKey(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __out   NCRYPT_KEY_HANDLE* phKey,
        __in    LPCWSTR pszAlgId,
        __in_opt LPCWSTR pszKeyName,
        __in    DWORD   dwLegacyKeySpec,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPGetProviderProperty(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    LPCWSTR pszProperty,
        __out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
        __in    DWORD   cbOutput,
        __out   DWORD* pcbResult,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPGetKeyProperty(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    NCRYPT_KEY_HANDLE hKey,
        __in    LPCWSTR pszProperty,
        __out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
        __in    DWORD   cbOutput,
        __out   DWORD* pcbResult,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPSetProviderProperty(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    LPCWSTR pszProperty,
        __in_bcount(cbInput) PBYTE pbInput,
        __in    DWORD   cbInput,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPSetKeyProperty(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __inout NCRYPT_KEY_HANDLE hKey,
        __in    LPCWSTR pszProperty,
        __in_bcount(cbInput) PBYTE pbInput,
        __in    DWORD   cbInput,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPFinalizeKey(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    NCRYPT_KEY_HANDLE hKey,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPDeleteKey(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __inout NCRYPT_KEY_HANDLE hKey,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPFreeProvider(
        __in    NCRYPT_PROV_HANDLE hProvider);

    SECURITY_STATUS WINAPI GM3000KSPFreeKey(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    NCRYPT_KEY_HANDLE hKey);

    SECURITY_STATUS WINAPI GM3000KSPFreeBuffer(
        __deref PVOID   pvInput);

    SECURITY_STATUS WINAPI GM3000KSPEncrypt(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    NCRYPT_KEY_HANDLE hKey,
        __in_bcount(cbInput) PBYTE pbInput,
        __in    DWORD   cbInput,
        __in    VOID* pPaddingInfo,
        __out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
        __in    DWORD   cbOutput,
        __out   DWORD* pcbResult,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPDecrypt(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    NCRYPT_KEY_HANDLE hKey,
        __in_bcount(cbInput) PBYTE pbInput,
        __in    DWORD   cbInput,
        __in    VOID* pPaddingInfo,
        __out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
        __in    DWORD   cbOutput,
        __out   DWORD* pcbResult,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPIsAlgSupported(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    LPCWSTR pszAlgId,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPEnumAlgorithms(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    DWORD   dwAlgOperations,
        __out   DWORD* pdwAlgCount,
        __deref_out_ecount(*pdwAlgCount) NCryptAlgorithmName** ppAlgList,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPEnumKeys(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in_opt LPCWSTR pszScope,
        __deref_out NCryptKeyName** ppKeyName,
        __inout PVOID* ppEnumState,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPImportKey(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in_opt NCRYPT_KEY_HANDLE hImportKey,
        __in    LPCWSTR pszBlobType,
        __in_opt NCryptBufferDesc* pParameterList,
        __out   NCRYPT_KEY_HANDLE* phKey,
        __in_bcount(cbData) PBYTE pbData,
        __in    DWORD   cbData,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPExportKey(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    NCRYPT_KEY_HANDLE hKey,
        __in_opt NCRYPT_KEY_HANDLE hExportKey,
        __in    LPCWSTR pszBlobType,
        __in_opt NCryptBufferDesc* pParameterList,
        __out_bcount_part_opt(cbOutput, *pcbResult) PBYTE pbOutput,
        __in    DWORD   cbOutput,
        __out   DWORD* pcbResult,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPSignHash(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    NCRYPT_KEY_HANDLE hKey,
        __in_opt    VOID* pPaddingInfo,
        __in_bcount(cbHashValue) PBYTE pbHashValue,
        __in    DWORD   cbHashValue,
        __out_bcount_part_opt(cbSignaturee, *pcbResult) PBYTE pbSignature,
        __in    DWORD   cbSignaturee,
        __out   DWORD* pcbResult,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPVerifySignature(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    NCRYPT_KEY_HANDLE hKey,
        __in_opt    VOID* pPaddingInfo,
        __in_bcount(cbHashValue) PBYTE pbHashValue,
        __in    DWORD   cbHashValue,
        __in_bcount(cbSignaturee) PBYTE pbSignature,
        __in    DWORD   cbSignaturee,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPPromptUser(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in_opt NCRYPT_KEY_HANDLE hKey,
        __in    LPCWSTR  pszOperation,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPNotifyChangeKey(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __inout HANDLE* phEvent,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPSecretAgreement(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    NCRYPT_KEY_HANDLE hPrivKey,
        __in    NCRYPT_KEY_HANDLE hPubKey,
        __out   NCRYPT_SECRET_HANDLE* phAgreedSecret,
        __in    DWORD   dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPDeriveKey(
        __in        NCRYPT_PROV_HANDLE   hProvider,
        __in_opt    NCRYPT_SECRET_HANDLE hSharedSecret,
        __in        LPCWSTR              pwszKDF,
        __in_opt    NCryptBufferDesc* pParameterList,
        __out_bcount_part_opt(cbDerivedKey, *pcbResult) PUCHAR pbDerivedKey,
        __in        DWORD                cbDerivedKey,
        __out       DWORD* pcbResult,
        __in        ULONG                dwFlags);

    SECURITY_STATUS WINAPI GM3000KSPFreeSecret(
        __in    NCRYPT_PROV_HANDLE hProvider,
        __in    NCRYPT_SECRET_HANDLE hSharedSecret);

#ifdef __cplusplus
}
#endif

#endif