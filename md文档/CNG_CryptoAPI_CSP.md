# [CNG](https://docs.microsoft.com/zh-cn/windows/win32/seccng/cng-portal)

# [CSP](https://docs.microsoft.com/zh-cn/windows/win32/seccrypto/cryptography-portal)
[工具1](https://docs.microsoft.com/zh-cn/windows/win32/seccrypto/tools-to-sign-files-and-check-signatures)
[工具2](https://docs.microsoft.com/zh-cn/windows/win32/seccrypto/tools-to-create-view-and-manage-certificates)




# Trusted Platform Module (TPM)
[how-windows-uses-the-tpm](https://docs.microsoft.com/zh-cn/windows/security/information-protection/tpm/how-windows-uses-the-tpm)

# [CNGProvider](https://github.com/skycipher/CNGProvider)

# [Windows Cryptography API Next Generation (CNG) 的 C++ Helper 类，包括 BCRYPT 和 NCRYPT](https://github.com/vladp72/hcrypt)

Use BCrypt* functions when you have the key in the memory of your application.
Use NCrypt* functions when you store the key in a KSP (Key Storage Provider).

The BCrypt family of function are classified as Cryptographic Primitives, while the NCrypt family of functions are classified as Key Storage and Retrieval.
BCrypt 系列函数被归类为 Cryptographic Primitives，而 NCrypt 系列函数被归类为密钥存储和检索。

The primary difference is that the BCrypt functions are used when dealing only with ephemeral keys, while the NCrypt functions are used when persistent keys are required.
主要区别在于，仅在处理临时密钥时使用 BCrypt 函数，而在需要持久密钥时使用 NCrypt 函数。

In practice, the BCrypt functions are typically used for hashing and symmetric encryption, while the NCrypt functions are used for public/private key encryption and decryption, public/private key signing and verification, and shared secret (e.g. DH and ECDH) negotiation.
在实践中，BCrypt 函数通常用于散列和对称加密，而 NCrypt 函数用于公钥/私钥加密和解密、公钥/私钥签名和验证以及共享密钥（例如 DH 和 ECDH）协商。

While some public/private key operations can be done with BCrypt functions, they can only be used with ephemeral keys and are therefore of limited use.
虽然可以使用 BCrypt 函数完成一些公钥/私钥操作，但它们只能与临时密钥一起使用，因此用途有限。

Persistent keys are stored in key containers specific to each user (or to the system). This is a security measure to ensure that users can't view each other's private keys.
持久密钥存储在特定于每个用户（或系统）的密钥容器中。这是一种确保用户无法查看彼此的私钥的安全措施。

In general, you'll want to use the following functions for the following operations:
通常，您需要将以下函数用于以下操作：
BCryptHashData：用于散列和 HMAC（MD5、SHA1、SHA256、SHA384、SHA512）
相关：BCryptCreateHash、BCryptFinishHash、BCryptDestroyHash
BCryptEncrypt：对称密钥加密（DES、3DES、AES）。
相关：BCryptGenerateSymmetricKey、BCryptDestroyKey
BCryptDecrypt：对称密钥解密（DES、3DES、AES）。
相关：BCryptGenerateSymmetricKey、BCryptDestroyKey
NCryptEncrypt：非对称密钥加密（RSA）
NCryptDecrypt：非对称密钥解密（RSA）
NCryptSignHash：非对称密钥签名（RSA、DSA、ECDSA）
NCryptVerifySignature：非对称密钥签名验证（RSA、DSA、ECDSA）
NCryptSecretAgreement：非对称密钥秘密共享（DH、ECDH）
相关：NCryptDeriveKey




# [Design and Implementation of KSP on the Next Generation Cryptography API](https://www.sciencedirect.com/science/article/pii/S1875389212015775)
从结构上看，CNG可以分为两个截然不同的基本类别，包括密码算法提供商Cryptographic alg Provider(CAP)和KSP。
With good seamless connectivity and higher safety,
KSP (Key Storage Providers) is the inexorable trend of security requirements and development to take the place of CSP (Cryptographic Service Provider). 
But the study on KSP has just started in our country, and almost no reports of its implementation can be found. 
Based on the analysis of function modules and the architecture of Cryptography API (Next Generation (CNG)), 
this paper discusses the design and implementation of KSP (key storage providers) based on smart card in detail, 
and an example is also presented to illustrate how to use KSP in Windows Vista.
具有良好的无缝连接性和更高的安全性，
KSP(密钥存储提供商)取代CSP(密码服务提供商)是安全需求和发展的必然趋势。
但是，我国对KSP的研究才刚刚起步，几乎没有关于其实施的报道。
在分析下一代加密API(CNG)的功能模块和体系结构的基础上，详细讨论了基于智能卡的KSP (key storage providers，密钥存储提供商)的设计和实现，并给出了一个在Windows Vista中使用KSP的例子。

Application programmer (such as IE〟 Outlook)       Security protocol(Such as TLS, WLAN)
                |_______________________________________________|
                                    |
               ___________________ CNG___________________________
               |                                                |                 
     __________CAP___________                                  KSP
     |                      |                                   |
Third party CAP  Microsoft Primitive Provider                   |
                (BCrypt DLL)                   _________________|___________________________
                                              |                             |               |
                                        Microsoft Software          Microsoft Smart Card    Third party KSP
                                        Key Storage Provider        Key Storage Provider
                                        (NCrypt DLL）               (SCKSP DLL)
                            _________________|__________________________
                            |                   |                       |
                Card minidriver KPS       Soft implementation of KSPs   Third party smart cards
  


# Cryptographic Provider Development Kit 开发包中有如何实现CNG的代码
[csp cng smartcard 开发文档](cpdk_help.chm)
Cryptography API (CryptoAPI) cryptographic service providers (CSPs) 
Cryptography Next Generation (CNG) cryptographic algorithm providers 
Cryptography Next Generation (CNG) key storage providers 
Smart Card Providers 

Topic Description 
CryptoAPI Providers
 Information you can use to create a cryptographic service provider (CSP). A CSP implements one or more cryptographic algorithms.
 
CNG Providers
 Information you can use to create a CNG cryptographic algorithm provider or key storage provider. An algorithm provider implements one or more cryptographic algorithms. A key storage provider stores CNG keys.
 
Smart Card Providers
 Information about smart cards and how to create, test and sign your smart card CSP. A smart card cryptographic service provider (CSP) implements cryptographic standards and algorithms on a smart card.
 





https://docs.microsoft.com/en-us/windows/win32/seccng/cng-portal
https://docs.microsoft.com/en-us/windows/win32/api/bcrypt/#functions

# [CNG 相关](https://docs.microsoft.com/en-us/windows/win32/seccng/about-cng)
Features
    For information about the features of CNG, see CNG Features.
Cryptographic Primitives
    For more information about the cryptographic primitives in CNG, see Cryptographic Primitives.
Key Storage and Retrieval
    For more information about key storage and retrieval in CNG, see Key Storage and Retrieval.
Key Import and Export
    For more information about importing and exporting keys, see Key Import and Export.
Data Protection API: Next Generation
    CNG provides a set of APIs that can be used to easily encrypt and decrypt content to authorization principals across multiple computers. For more information, see CNG DPAPI.


c:\windows\system32\BCRYPT.DLL
BCryptAddContextFunction
BCryptAddContextFunctionProvider
BCryptCloseAlgorithmProvider
BCryptConfigureContext
BCryptConfigureContextFunction
BCryptCreateContext
BCryptCreateHash
BCryptCreateMultiHash
BCryptDecrypt
BCryptDeleteContext
BCryptDeriveKey
BCryptDeriveKeyCapi
BCryptDeriveKeyPBKDF2
BCryptDestroyHash
BCryptDestroyKey
BCryptDestroySecret
BCryptDuplicateHash
BCryptDuplicateKey
BCryptEncrypt
BCryptEnumAlgorithms
BCryptEnumContextFunctionProviders
BCryptEnumContextFunctions
BCryptEnumContexts
BCryptEnumProviders
BCryptEnumRegisteredProviders
BCryptExportKey
BCryptFinalizeKeyPair
BCryptFinishHash
BCryptFreeBuffer
BCryptGenRandom
BCryptGenerateKeyPair
BCryptGenerateSymmetricKey
BCryptGetFipsAlgorithmMode
BCryptGetProperty
BCryptHash
BCryptHashData
BCryptImportKey
BCryptImportKeyPair
BCryptKeyDerivation
BCryptOpenAlgorithmProvider
BCryptProcessMultiOperations
BCryptQueryContextConfiguration
BCryptQueryContextFunctionConfiguration
BCryptQueryContextFunctionProperty
BCryptQueryProviderRegistration
BCryptRegisterConfigChangeNotify
BCryptRegisterProvider
BCryptRemoveContextFunction
BCryptRemoveContextFunctionProvider
BCryptResolveProviders
BCryptSecretAgreement
BCryptSetAuditingInterface
BCryptSetContextFunctionProperty
BCryptSetProperty
BCryptSignHash
BCryptUnregisterConfigChangeNotify
BCryptUnregisterProvider
BCryptVerifySignature


c:\windows\system32\BCRYPTPRIMITIVES.DLL
GetAsymmetricEncryptionInterface
GetCipherInterface
GetHashInterface
GetKeyDerivationInterface
GetRngInterface
GetSecretAgreementInterface
GetSignatureInterface
MSCryptConvertRsaPrivateBlobToFullRsaBlob
ProcessPrng
ProcessPrngGuid
