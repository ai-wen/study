// opensslsign.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include <windows.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/pkcs12.h>
#include <openssl/applink.c>

extern "C"{
#include "mtoken_api.h"
#include "sha2.h"
#include "aes.h"
#include "Cipher_IDs.h"
#include "des.h"
}

#pragma comment(lib, "libcrypto.lib")
//#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib,"mTokenCryptoID.lib")

#define  ADMIN_PIN_ID   1
#define  USER_PIN_ID	2

#define  DEFAULT_MC_KEY "12345678123456781234567812345678"

/*
#define  FID_MF	 0x3F00	//"MF""



#define  DEFAULT_EA_KEY "12345678123456781234567812345678"

#define FID_DIR1 0x1001

#define ROOT_NAME "App"

#define FID_PUB1 0x5001
#define FID_PUB2 0x5002


#define FID_PRI1 0x6001
#define FID_PRI2 0x6002
*/
static int e_dev_auth(HANDLE hDev, int keyId, unsigned char* keyValue)
{
    unsigned char iv[16] = { 0 };
    unsigned char ack[64] = { 0 };
    unsigned char challenge[32];
    ULONG ret = 0;

    int length = 32;
    ret = mToken_GetChallenge(hDev, length, challenge);
    if (ret != 0) return ret;

    Aes_Init(keyValue, 32);
    Aes_Run(AES_ENCRYPT, AES256_ECB, challenge, ack, 32, iv);

    ret = mToken_DeviceAuth(hDev, keyId, ack, 32);
    if (ret != 0) return ret;

    return ret;
}
static int e_verifyPin(HANDLE hDev, int pinId, char* pinValue)
{
    int ret = 0;

    unsigned char challenge[32];
    unsigned char ack[32] = { 0 };
    unsigned char iv[32] = { 0 };
    int ack_len = 32;

    ret = mToken_GetChallenge(hDev, 16, challenge);
    if (ret != 0)
        return ret;

    unsigned char pin[64] = { 0 };
    strncpy((char*)pin, pinValue, 32);

    unsigned char key[32] = { 0 };
    sha2(pin, strlen((char*)pin), key, 0);

    des3_context ctx = { 0 };
    des3_set3key_enc(&ctx, key);
    des3_crypt_cbc(&ctx, DES_ENCRYPT, 16, iv, challenge, ack);

    ret = mToken_VerifyPin(hDev, pinId, 0, ack, 16);

    return ret;
}
/*
int verifyPin(HANDLE hDev, int pinId, char* pinValue)
{
    int ret = 0;

    unsigned char challenge[32];
    unsigned char ack[32] = { 0 };
    unsigned char iv[32] = { 0 };
    int ack_len = 32;

    struFILE sFile = { 0 };
    ret = mToken_SelectFileById(hDev, FID_MF, &sFile);
    if (ret != 0)
        return ret;

    ret = mToken_SelectChildDFById(hDev, FID_DIR1, &sFile);
    if (ret != 0)
        return ret;

    ret = mToken_GetChallenge(hDev, 16, challenge);
    if (ret != 0)
        return ret;

    unsigned char pin[64] = { 0 };
    strncpy((char*)pin, pinValue, 32);

    unsigned char key[32] = { 0 };
    sha2(pin, strlen((char*)pin), key, 0);

    Aes_Init(key, 16);
    Aes_Run(AES_ENCRYPT, AES128_ECB, challenge, ack, 16, iv);

    //2021.4 new
    int mode = 0;
    ret = mToken_VerifyPin(hDev, pinId, mode, ack, 16);
    return ret;
}*/

void mTokenSign(unsigned char* sig_buf, unsigned char* out_buf)
{
    // TODO: Add your control notification handler code here
    int ret;
    char userPinVal[32] = "12345678";

    HANDLE m_hDevHandle;
    char devNames[1024] = { 0 };
    ULONG nameLen = 1024;

    if (0 != mToken_EnumDev((LPSTR)"mToken", devNames, &nameLen))
    {
        nameLen = 1024;
        if (0 != mToken_EnumDev((LPSTR)"mFIDO", devNames, &nameLen))
        {
            return;
        }
    }
   
    if (0 != mToken_ConnectDev(devNames, &m_hDevHandle))
    {
        return;
    }

    ret = e_dev_auth(m_hDevHandle, KEY_MC_ID, (unsigned char*)DEFAULT_MC_KEY);
    if (0 != ret)
    {
        return;
    }

    ret = e_verifyPin(m_hDevHandle, USER_PIN_ID, userPinVal);
    if(ret)
        return;

    int   sig_len = 2048/8;
    int   out_len = 1024;

    //doPss(CIPHER_ID_SHA256, 1024, testData1, dataLen1, testData1);

    //ret = mToken_AsymGenKeypair(m_hDevHandle, FID_PUB1, FID_PRI1, algRSAIDs[i], 0, CIPHER_KEY_USAGE_SIGNVERIFY | CIPHER_KEY_USAGE_ENDECRYPY);
    //if (ret)
        //   return;

    ret = mToken_AsymSign(m_hDevHandle, 0x4500, 0, sig_buf, sig_len, out_buf, &out_len);
    if (ret)
        return;
    
}


static int doPSS(unsigned int hType, unsigned int rsamodulLen,
    const unsigned char* src, unsigned int srcLen, unsigned char* EM);

void opensslSign(const unsigned char* sig_buf, unsigned char* out_buf)
{
    const char* file = "2048-1.pfx", * pswd = "1";
    unsigned char out_buf2[1024] = { 0 };
    int   sig_len = 1024;
    int   out_len = 1024;
    EVP_PKEY* pkey = NULL;
    FILE* fp = NULL;
    X509* x509 = NULL;
    PKCS12* p12 = NULL;
    STACK_OF(X509)* ca = NULL;

    int pkey_type = 0;
    EVP_PKEY_CTX* pkctx = NULL;

    const EVP_MD* md = EVP_sha256();

    OpenSSL_add_all_algorithms();

    RSA* rsa = NULL;

    unsigned int nLen = 0;

    unsigned char mHash[64] = { 0 };
    int hash_len = 0;
    unsigned char EM[256] = { 0 };
    int rr = 0;

    do
    {
        if (!(fp = fopen(file, "rb")))
            break;

        p12 = d2i_PKCS12_fp(fp, NULL);
        if (!p12)
            break;

        if (!PKCS12_parse(p12, pswd, &pkey, &x509, &ca))
            break;

        if (pkey == NULL)
            break;

#if 0
        pkey_type = EVP_PKEY_id(pkey);

        if (!(pkctx = EVP_PKEY_CTX_new(pkey, NULL)))
            break;

        if (EVP_PKEY_sign_init(pkctx) <= 0) 
            break;

        //if (EVP_PKEY_verify_init(pkctx) <= 0)
        //    break;

        if (md && !EVP_PKEY_CTX_set_signature_md(pkctx, md))
            break;

        if (pkey_type == EVP_PKEY_RSA)
        {
            if (!EVP_PKEY_CTX_set_rsa_padding(pkctx, RSA_PKCS1_PSS_PADDING))
                break;
        }
        else if (pkey_type == EVP_PKEY_EC)
        {
            /*
            if (!EVP_PKEY_CTX_set_ec_scheme(pkctx, OBJ_txt2nid(alg) == NID_sm2sign ?
                NID_sm_scheme : NID_secg_scheme)) {
                break;
            }
            */
        }

        //待签名数据必须经过指定的hash算法处理后传入。即EVP_PKEY_sign内部认为原文长度与hash长度不一致就报错
        if (EVP_PKEY_sign(pkctx, out_buf, (size_t*)&out_len, sig_buf, 33) <= 0) {
            break;
        }

       // if (EVP_PKEY_verify(pkctx, sig_buf, 32, inbuf, inlen) <= 0) {
       //     break;
       // }

#endif









        rsa = EVP_PKEY_get1_RSA(pkey);
        if (rsa == NULL)
            break;

#if 0
        sig_len = RSA_size(rsa);
        out_len = RSA_private_encrypt(sig_len, sig_buf, out_buf, rsa, RSA_NO_PADDING);//RSA_NO_PADDING  RSA_PKCS1_PSS_PADDING
        if (sig_len != out_len)
            break;
#else
        sig_len = RSA_size(rsa);

     

        hash_len = 32;
        sha2(sig_buf, sig_len, mHash, 0);
        
       //rr =  RSA_padding_add_PKCS1_PSS(rsa, EM, mHash, EVP_sha256(), hash_len);


        doPSS(CIPHER_ID_SHA256, 2048, sig_buf, sig_len, EM);

        //编码过程：先调 hash；
        //再将 hash 值传入 RSA_padding_add_PKCS1_PSS 进行编码；
        //之后再调 RSA_private_encrypt 对编码的值进行加密。
        out_len = RSA_private_encrypt(sig_len, EM, out_buf, rsa, RSA_NO_PADDING);//RSA_NO_PADDING  RSA_PKCS1_PSS_PADDING
        if (sig_len != out_len)
            break;

        //验证过程：先调 RSA_public_decrypt 解码；
       //再 hash 原文值；
       //将 hash 值传入 RSA_verify_PKCS1_PSS 验证。

        out_len = RSA_public_decrypt(sig_len, out_buf, out_buf2, rsa, RSA_NO_PADDING);
        if (sig_len != out_len)
            break;

        if (0 == memcmp(out_buf2, EM, sig_len))
        {
            printf("ok\n");
        }

        //sha2(out_buf2, sig_len, mHash, 0);

        rr = RSA_verify_PKCS1_PSS(rsa, mHash, EVP_sha256(), EM, hash_len);

        if (rr != 1)
            break;
#endif

    } while (0);


    if (fp)
        fclose(fp);
    if (pkey)
        EVP_PKEY_free(pkey);
    if (p12)
        PKCS12_free(p12);


}

int main(int argc, char* argv[])
{
   
    unsigned char sig_buf[1024];
    unsigned char out_buf1[1024] = {0};
    unsigned char out_buf2[1024] = { 0 };

    for (size_t i = 0; i < sizeof(sig_buf); i++)
        sig_buf[i] = i + 1;
    //memset(sig_buf, 1, 1024);

    opensslSign(sig_buf, out_buf1);
    
    for (size_t i = 0; i < sizeof(sig_buf); i++)
        sig_buf[i] = i + 1;

    mTokenSign(sig_buf, out_buf2);


    if (0 == memcmp(out_buf1, out_buf2, 2048 / 8))
    {
        printf("ok\n");

    }

    return 0;
}



#if 1
#include <time.h>
void getRandom(unsigned char* out, unsigned int Len)
{
    srand(time(NULL));
    for (unsigned int i = 0; i < Len; i++)
        out[i] = rand() % 0xFF;
}

void doHash(unsigned int hType, const unsigned char* src, unsigned int srcLen, unsigned char* des, unsigned int& hash_len)
{
    if (NULL == src || 0 == srcLen || NULL == des)
    {
        hash_len = 0;
        return;
    }
    switch (hType)
    {
    case CIPHER_ID_MD5:
        hash_len = 16;
        break;
    case CIPHER_ID_SHA1:
        hash_len = 20;
        break;
    case CIPHER_ID_SHA224:
        hash_len = 28;
        sha2(src, srcLen, des, 1);
        break;
    case CIPHER_ID_SHA256:
        hash_len = 32;
        sha2(src, srcLen, des, 0);
        break;
    case CIPHER_ID_SHA384:
        hash_len = 48;
        break;
    case CIPHER_ID_SHA512:
        hash_len = 64;
        break;
    default:
        break;
    }

}

static int PKCS1_MGF1(unsigned int hType, unsigned char* DB, unsigned int dblen, 
    const unsigned char* seed, unsigned int mdlen)
{
    unsigned int i, outlen = 0;
    unsigned char cnt[4];
    unsigned char md[64];
    unsigned char tmp[64 + 4] = { 0 };

    for (i = 0; outlen < dblen; i++) 
    {
        cnt[0] = (unsigned char)((i >> 24) & 255);
        cnt[1] = (unsigned char)((i >> 16) & 255);
        cnt[2] = (unsigned char)((i >> 8)) & 255;
        cnt[3] = (unsigned char)(i & 255);

        memcpy(tmp, seed, mdlen);
        memcpy(tmp+ mdlen, cnt, 4);

        if (outlen + mdlen <= dblen) 
        {
            doHash(hType, tmp, mdlen+4, DB + outlen, mdlen);
            outlen += mdlen;
        }
        else 
        {
            doHash(hType, tmp, mdlen + 4, md, mdlen);
            memcpy(DB + outlen, md, dblen - outlen);
            outlen = dblen;
        }
    }

    return 0;
}


static int doPSS(unsigned int hType, unsigned int rsamodulLen,
    const unsigned char* src, unsigned int srcLen, unsigned char* EM)
{
    static unsigned char padding[256] = { 0 };

    int i;
    int ret = 0;
    int maskedDBLen, MSBits, emLen;
    unsigned char* H, salt[64] = {0}, * p;
    unsigned char mHash[64] = { 0 };
    unsigned int hashLen = 0;

    doHash(hType, src, srcLen, mHash, hashLen);

    MSBits = (rsamodulLen - 1) & 0x7;
    emLen = rsamodulLen/8;
    if (MSBits == 0) 
    {
        *EM++ = 0;
        emLen--;
    }

    if (emLen < (hashLen + hashLen + 2)) 
    {
        goto err;
    }

    if (hashLen > 0)
    {
        getRandom(salt, hashLen);
    }

    maskedDBLen = emLen - hashLen - 1;
    H = EM + maskedDBLen;

    memset(padding, 0, 8);
    memcpy(padding + 8, mHash, hashLen);
    memcpy(padding + 8 + hashLen, salt, hashLen);

    doHash(hType, padding, 8 + hashLen + hashLen, H, hashLen);

    /* Generate dbMask in place then perform XOR on it */
    if (PKCS1_MGF1(hType,EM, maskedDBLen, H, hashLen))
        goto err;

    /*
     * Initial PS XORs with all zeroes which is a NOP so just update pointer.
     * Note from a test above this value is guaranteed to be non-negative.
     */
    p = EM;
    p += emLen - hashLen - hashLen - 2;
    *p++ ^= 0x1;
    if (hashLen > 0) 
    {
        for (i = 0; i < hashLen; i++)
            *p++ ^= salt[i];
    }
    if (MSBits)
        EM[0] &= 0xFF >> (8 - MSBits);

    /* H is already in place so just set final 0xbc */

    EM[emLen - 1] = 0xbc;

    ret = 1;

err:

    return ret;

}
#endif
