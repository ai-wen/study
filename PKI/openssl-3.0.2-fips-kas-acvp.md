# 编译启用 fips

using Visual Studio, open a Developer Command Prompt and issue the following commands to build OpenSSL.

perl Configure debug-VC-WIN32  shared no-asm enable-fips --prefix="G:\\project\\7mTokenCryptoID\\4.Development\\fips\\include\\release\\vector\\openss-fips" --openssldir="G:\\project\\7mTokenCryptoID\\4.Development\\fips\\include\\release\\vector\\openss-fips\\ssl"  

nmake 
nmake install


G:\\project\\7mTokenCryptoID\\4.Development\\fips\\include\\release\\vector\\openss-fips\\ssl\\openssl.cnf

[provider_sect]
fips = fips_sect

下面拷贝 fipsmodule.cnf 的内容
[fips_sect] 
activate = 1
conditional-errors = 1
security-checks = 1
module-mac = CA:A6:7B:72:6D:2E:84:44:D6:3B:64:FD:82:AF:F9:A9:65:B2:B5:7E:91:A9:D1:72:D3:97:FD:C2:DC:D4:A7:54



# 使用

OSSL_PROVIDER* fipsp = OSSL_PROVIDER_load(NULL, "fips"); //"default"
if (fipsp)
{
    libctx = OSSL_LIB_CTX_new();
    // SSL_CTX* ctx = SSL_CTX_new_ex(libctx, NULL, TLS_method());
    
    // EVP_set_default_properties(libctx, "fips = yes");
}


# KAS acvp NIST.SP.800-56Ar3

// openssl-fips.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#pragma comment(lib,"libcrypto.lib")
#pragma comment(lib,"libssl.lib")

#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/bn.h>
#include <openssl/ecdsa.h>
#include <openssl/types.h>
#include <openssl/kdf.h>

#include <openssl/param_build.h>
#include <openssl/provider.h>
#include <openssl/ssl.h>

/*
[
   {
     {
        "ephemeralPublicServerX": "00DEEAADA4FFB969F6BB556E3D180F117A8DE34D354B8FDBFDFCEC9E7DC31E642DD0903D493DEFFB6207F8BC648F644A92CFC7E8B3D2A797240A66A852177449A542",
            "ephemeralPublicServerY" : "00F680336E81831B0FBC34DD33A73F55A7261AEF1B4642FF7907423098EC7D62635E969AC7BB2C51A971A5B61C4A172F42B7A0B5F138E754FF9D53BD148FCDBBC046",
            "ephemeralPrivateIut" : "00DCB445312B57B05666A048B206E214DBA027C9CABFE8C7A9A7F0C7AA2DDB1DD88F76CA9919D64B9AA85A62231C64E86E09842A51DDE876A64E5F0877F158B1C8C4",
            "ephemeralPublicIutX" : "01EB41CBB204878DFA3A76AB1C670BCF0D481BCE9D1986A9C75E12163CF6D6A6764E51585177EFF3506F0F0288479F68C8657793D2D8091878D61D6056F095742E4C",
            "ephemeralPublicIutY" : "00FCF86CCF77338032788CE794A66CB3B46930C8566BFE7C5F7A5791FFC036D8626966BD921C51ADC4B26F714AB3ED483C2C842BE70C77CFDCA6159945AFCECF655E",
            "tcId" : 25,
            "kdfParameter" : {
            "kdfType": "oneStep",
                "algorithmId" : "294CDFC6C19533D0DB91747CA9812394"
        },
            "dkm" : "612BB2944E3E14771C068315E61D0E871F087EDC90DDC2487B4C61F6BE8E82B6"
    }
],
"domainParameterGenerationMode": "P-521",
    "scheme" : "ephemeralUnified",
    "kasRole" : "responder",
    "l" : 256,
    "iutId" : "6131623263336434613162326333643461316232633364346131623263336434",
    "serverId" : "434156536964",
    "kdfConfiguration" : {
    "kdfType": "oneStep",
        "saltMethod" : "default",
        "fixedInfoPattern" : "algorithmId||l||uPartyInfo||vPartyInfo",
        "fixedInfoEncoding" : "concatenation",
        "auxFunction" : "SHA2-512"
}
*/


static const unsigned char ecdh_privd[] = {
    0x00, 0xDC, 0xB4, 0x45, 0x31, 0x2B, 0x57, 0xB0, 0x56, 0x66, 0xA0, 0x48, 0xB2, 0x06, 0xE2, 0x14,
    0xDB, 0xA0, 0x27, 0xC9, 0xCA, 0xBF, 0xE8, 0xC7, 0xA9, 0xA7, 0xF0, 0xC7, 0xAA, 0x2D, 0xDB, 0x1D,
    0xD8, 0x8F, 0x76, 0xCA, 0x99, 0x19, 0xD6, 0x4B, 0x9A, 0xA8, 0x5A, 0x62, 0x23, 0x1C, 0x64, 0xE8,
    0x6E, 0x09, 0x84, 0x2A, 0x51, 0xDD, 0xE8, 0x76, 0xA6, 0x4E, 0x5F, 0x08, 0x77, 0xF1, 0x58, 0xB1,
    0xC8, 0xC4
};


static const unsigned char ecdh_pub[] = {
    0x04,
    0x01, 0xEB, 0x41, 0xCB, 0xB2, 0x04, 0x87, 0x8D, 0xFA, 0x3A, 0x76, 0xAB, 0x1C, 0x67, 0x0B, 0xCF,
    0x0D, 0x48, 0x1B, 0xCE, 0x9D, 0x19, 0x86, 0xA9, 0xC7, 0x5E, 0x12, 0x16, 0x3C, 0xF6, 0xD6, 0xA6,
    0x76, 0x4E, 0x51, 0x58, 0x51, 0x77, 0xEF, 0xF3, 0x50, 0x6F, 0x0F, 0x02, 0x88, 0x47, 0x9F, 0x68,
    0xC8, 0x65, 0x77, 0x93, 0xD2, 0xD8, 0x09, 0x18, 0x78, 0xD6, 0x1D, 0x60, 0x56, 0xF0, 0x95, 0x74,
    0x2E, 0x4C,
    0x00, 0xFC, 0xF8, 0x6C, 0xCF, 0x77, 0x33, 0x80, 0x32, 0x78, 0x8C, 0xE7, 0x94, 0xA6, 0x6C, 0xB3,
    0xB4, 0x69, 0x30, 0xC8, 0x56, 0x6B, 0xFE, 0x7C, 0x5F, 0x7A, 0x57, 0x91, 0xFF, 0xC0, 0x36, 0xD8,
    0x62, 0x69, 0x66, 0xBD, 0x92, 0x1C, 0x51, 0xAD, 0xC4, 0xB2, 0x6F, 0x71, 0x4A, 0xB3, 0xED, 0x48,
    0x3C, 0x2C, 0x84, 0x2B, 0xE7, 0x0C, 0x77, 0xCF, 0xDC, 0xA6, 0x15, 0x99, 0x45, 0xAF, 0xCE, 0xCF,
    0x65, 0x5E
};

static const unsigned char ecdh_peer_pub[] = {
    0x04,
    0x00, 0xDE, 0xEA, 0xAD, 0xA4, 0xFF, 0xB9, 0x69, 0xF6, 0xBB, 0x55, 0x6E, 0x3D, 0x18, 0x0F, 0x11,
    0x7A, 0x8D, 0xE3, 0x4D, 0x35, 0x4B, 0x8F, 0xDB, 0xFD, 0xFC, 0xEC, 0x9E, 0x7D, 0xC3, 0x1E, 0x64,
    0x2D, 0xD0, 0x90, 0x3D, 0x49, 0x3D, 0xEF, 0xFB, 0x62, 0x07, 0xF8, 0xBC, 0x64, 0x8F, 0x64, 0x4A,
    0x92, 0xCF, 0xC7, 0xE8, 0xB3, 0xD2, 0xA7, 0x97, 0x24, 0x0A, 0x66, 0xA8, 0x52, 0x17, 0x74, 0x49,
    0xA5, 0x42,
    0x00, 0xF6, 0x80, 0x33, 0x6E, 0x81, 0x83, 0x1B, 0x0F, 0xBC, 0x34, 0xDD, 0x33, 0xA7, 0x3F, 0x55,
    0xA7, 0x26, 0x1A, 0xEF, 0x1B, 0x46, 0x42, 0xFF, 0x79, 0x07, 0x42, 0x30, 0x98, 0xEC, 0x7D, 0x62,
    0x63, 0x5E, 0x96, 0x9A, 0xC7, 0xBB, 0x2C, 0x51, 0xA9, 0x71, 0xA5, 0xB6, 0x1C, 0x4A, 0x17, 0x2F,
    0x42, 0xB7, 0xA0, 0xB5, 0xF1, 0x38, 0xE7, 0x54, 0xFF, 0x9D, 0x53, 0xBD, 0x14, 0x8F, 0xCD, 0xBB,
    0xC0, 0x46
};



static int SSKDF_hash_kdm(const EVP_MD* kdf_md,
    const unsigned char* z, size_t z_len,
    const unsigned char* info, size_t info_len,
    unsigned int append_ctr,
    unsigned char* derived_key, size_t derived_key_len);

static int self_test_ka()
{
    int ret = 0;
    OSSL_LIB_CTX* libctx = NULL;
    OSSL_PROVIDER* fipsp = OSSL_PROVIDER_load(NULL, "fips"); //"default"  "fips"
    if (fipsp)
    {
        libctx = OSSL_LIB_CTX_new();
    }

    EVP_PKEY* pkey = NULL, * peerkey = NULL;

    EVP_PKEY_CTX* kactx = NULL;
    EVP_PKEY_CTX* dctx = NULL;

    BN_CTX* bnctx = BN_CTX_new_ex(libctx);
    OSSL_PARAM_BLD* bld = OSSL_PARAM_BLD_new();
    OSSL_PARAM* params = NULL;
    OSSL_PARAM* params_peer = NULL;

    unsigned char secret[256];
    size_t secret_len = sizeof(secret);

    BIGNUM* priv = BN_bin2bn(ecdh_privd, sizeof(ecdh_privd), NULL);


    do
    {
        ret = OSSL_PARAM_BLD_push_utf8_string(bld, "group", "secp521r1", 0); //secp521r1  prime256v1
        ret = OSSL_PARAM_BLD_push_BN(bld, "priv", priv);
        ret = OSSL_PARAM_BLD_push_octet_string(bld, "pub", ecdh_pub, sizeof(ecdh_pub));
        params = OSSL_PARAM_BLD_to_param(bld);

        ret = OSSL_PARAM_BLD_push_utf8_string(bld, "group", "secp521r1", 0);
        ret = OSSL_PARAM_BLD_push_octet_string(bld, "pub", ecdh_peer_pub, sizeof(ecdh_peer_pub));
        params_peer = OSSL_PARAM_BLD_to_param(bld);
       

        kactx = EVP_PKEY_CTX_new_from_name(libctx, "EC", "");//"RSA-PSS" "RSA" "SM2"
        if (EVP_PKEY_fromdata_init(kactx) <= 0
            || EVP_PKEY_fromdata(kactx, &pkey, EVP_PKEY_KEYPAIR, params) <= 0)
        {
            break;
        }

        
        if (EVP_PKEY_fromdata_init(kactx) <= 0
            || EVP_PKEY_fromdata(kactx, &peerkey, EVP_PKEY_KEYPAIR, params_peer) <= 0)
        {
            break;
        }
        

        dctx = EVP_PKEY_CTX_new_from_pkey(libctx, pkey, NULL);
        if (!dctx)
        {
            ret = 5;
            break;
        }

        if (EVP_PKEY_derive_init(dctx) <= 0)
        {
            ret = 6;
            break;
        }

        if (EVP_PKEY_derive_set_peer(dctx, peerkey) <= 0)
        {
            ret = 6;
            break;
        }

       
        if (EVP_PKEY_derive(dctx, secret, &secret_len) <= 0)
        {
            ret = 7;
            break;
        }

        /*
        if (secret_len != sizeof(ecdh_secret_expected)
            || memcmp(secret, ecdh_secret_expected, sizeof(ecdh_secret_expected)) != 0)
        {
            ret = 7;
            break;
        }
        */

    } while (0);

    //"algorithmId||l||uPartyInfo||vPartyInfo"
    //l     big-endi The length of the derived keying material in bits, MUST be represented in 32 bits for ACVP testing.

    static const unsigned char sskdf_otherinfo[] = {
        //"algorithmId"：294CDFC6C19533D0DB91747CA9812394
        0x29, 0x4C, 0xDF, 0xC6, 0xC1, 0x95, 0x33, 0xD0, 0xDB, 0x91, 0x74, 0x7C, 0xA9, 0x81, 0x23, 0x94,
        //"l": 00000100   big-endi
        0x00, 0x00, 0x01, 0x00,
        //"serverId" :434156536964
        0x43, 0x41, 0x56, 0x53, 0x69, 0x64,
        //"ephemeralPublicServerX" :        
        //"ephemeralPublicServerY" :
        0x00, 0xDE, 0xEA, 0xAD, 0xA4, 0xFF, 0xB9, 0x69, 0xF6, 0xBB, 0x55, 0x6E, 0x3D, 0x18, 0x0F, 0x11,
        0x7A, 0x8D, 0xE3, 0x4D, 0x35, 0x4B, 0x8F, 0xDB, 0xFD, 0xFC, 0xEC, 0x9E, 0x7D, 0xC3, 0x1E, 0x64,
        0x2D, 0xD0, 0x90, 0x3D, 0x49, 0x3D, 0xEF, 0xFB, 0x62, 0x07, 0xF8, 0xBC, 0x64, 0x8F, 0x64, 0x4A,
        0x92, 0xCF, 0xC7, 0xE8, 0xB3, 0xD2, 0xA7, 0x97, 0x24, 0x0A, 0x66, 0xA8, 0x52, 0x17, 0x74, 0x49,
        0xA5, 0x42,
        0x00, 0xF6, 0x80, 0x33, 0x6E, 0x81, 0x83, 0x1B, 0x0F, 0xBC, 0x34, 0xDD, 0x33, 0xA7, 0x3F, 0x55,
        0xA7, 0x26, 0x1A, 0xEF, 0x1B, 0x46, 0x42, 0xFF, 0x79, 0x07, 0x42, 0x30, 0x98, 0xEC, 0x7D, 0x62,
        0x63, 0x5E, 0x96, 0x9A, 0xC7, 0xBB, 0x2C, 0x51, 0xA9, 0x71, 0xA5, 0xB6, 0x1C, 0x4A, 0x17, 0x2F,
        0x42, 0xB7, 0xA0, 0xB5, 0xF1, 0x38, 0xE7, 0x54, 0xFF, 0x9D, 0x53, 0xBD, 0x14, 0x8F, 0xCD, 0xBB,
        0xC0, 0x46,
        //"iutId"6131623263336434613162326333643461316232633364346131623263336434
        0x61, 0x31, 0x62, 0x32, 0x63, 0x33, 0x64, 0x34, 
        0x61, 0x31, 0x62, 0x32, 0x63, 0x33, 0x64, 0x34, 
        0x61, 0x31, 0x62, 0x32, 0x63, 0x33, 0x64, 0x34, 
        0x61, 0x31, 0x62, 0x32, 0x63, 0x33, 0x64, 0x34,
        //"ephemeralPublicIutX" :
        //"ephemeralPublicIutY" :
        0x01, 0xEB, 0x41, 0xCB, 0xB2, 0x04, 0x87, 0x8D, 0xFA, 0x3A, 0x76, 0xAB, 0x1C, 0x67, 0x0B, 0xCF,
        0x0D, 0x48, 0x1B, 0xCE, 0x9D, 0x19, 0x86, 0xA9, 0xC7, 0x5E, 0x12, 0x16, 0x3C, 0xF6, 0xD6, 0xA6,
        0x76, 0x4E, 0x51, 0x58, 0x51, 0x77, 0xEF, 0xF3, 0x50, 0x6F, 0x0F, 0x02, 0x88, 0x47, 0x9F, 0x68,
        0xC8, 0x65, 0x77, 0x93, 0xD2, 0xD8, 0x09, 0x18, 0x78, 0xD6, 0x1D, 0x60, 0x56, 0xF0, 0x95, 0x74,
        0x2E, 0x4C,
        0x00, 0xFC, 0xF8, 0x6C, 0xCF, 0x77, 0x33, 0x80, 0x32, 0x78, 0x8C, 0xE7, 0x94, 0xA6, 0x6C, 0xB3,
        0xB4, 0x69, 0x30, 0xC8, 0x56, 0x6B, 0xFE, 0x7C, 0x5F, 0x7A, 0x57, 0x91, 0xFF, 0xC0, 0x36, 0xD8,
        0x62, 0x69, 0x66, 0xBD, 0x92, 0x1C, 0x51, 0xAD, 0xC4, 0xB2, 0x6F, 0x71, 0x4A, 0xB3, 0xED, 0x48,
        0x3C, 0x2C, 0x84, 0x2B, 0xE7, 0x0C, 0x77, 0xCF, 0xDC, 0xA6, 0x15, 0x99, 0x45, 0xAF, 0xCE, 0xCF,
        0x65, 0x5E
    };


    // "dkm" : "612BB2944E3E14771C068315E61D0E871F087EDC90DDC2487B4C61F6BE8E82B6"

    unsigned char dkm[128];
    
    unsigned int append_ctr = 0; // 800-56c rev-1 Section 4. One - Step Key Derivation using H(x) = hash(x)
                 //append_ctr = 1; //  X9.63
     
    ret = SSKDF_hash_kdm(EVP_sha512(), secret, secret_len, sskdf_otherinfo, sizeof(sskdf_otherinfo), append_ctr, dkm, 64);



    BN_CTX_free(bnctx);
    EVP_PKEY_free(pkey);
    EVP_PKEY_free(peerkey);
    EVP_PKEY_CTX_free(kactx);
    EVP_PKEY_CTX_free(dctx);
    OSSL_PARAM_free(params_peer);
    OSSL_PARAM_free(params);
    OSSL_PARAM_BLD_free(bld);
    BN_free(priv);

#if 0
    EVP_KDF* kdf = NULL;
    EVP_KDF_CTX* ctx = NULL;
    OSSL_PARAM* paramskdf = NULL;
    kdf = EVP_KDF_fetch(libctx, "SSKDF", ""); //SSKDF
    ctx = EVP_KDF_CTX_new(kdf);

    OSSL_PARAM_BLD_push_utf8_string(bld, "digest", "SHA512", 6);
    OSSL_PARAM_BLD_push_octet_string(bld, "key", secret, secret_len);
    OSSL_PARAM_BLD_push_octet_string(bld, "info", sskdf_otherinfo, sizeof(sskdf_otherinfo));
    paramskdf = OSSL_PARAM_BLD_to_param(bld);

    do
    {
        if (EVP_KDF_derive(ctx, dkm, 32, paramskdf) <= 0)
        {
            ret = 8;
            break;
        }
        //"dkm" : "612BB2944E3E14771C068315E61D0E871F087EDC90DDC2487B4C61F6BE8E82B6"
        /*
        if (memcmp(out, sskdf_expected, sizeof(sskdf_expected)) != 0)
        {
            ret = 9;
            break;
        }
        */
    } while (0);


    EVP_KDF_free(kdf);
    EVP_KDF_CTX_free(ctx);
#endif

    return ret;
}

int self_test_kdf();
int main()
{
    int l = 256;
    //return self_test_kdf();
    return self_test_ka();
}



#if 0


static const char hkdf_digest[] = "SHA256";
static const unsigned char hkdf_secret[] = { 's', 'e', 'c', 'r', 'e', 't' };
static const unsigned char hkdf_salt[] = { 's', 'a', 'l', 't' };
static const unsigned char hkdf_info[] = { 'l', 'a', 'b', 'e', 'l' };
static const unsigned char hkdf_expected[] = {
    0x2a, 0xc4, 0x36, 0x9f, 0x52, 0x59, 0x96, 0xf8,
    0xde, 0x13
};
static const char sskdf_digest[] = "SHA224";
static const unsigned char sskdf_secret[] = {
    0x6d, 0xbd, 0xc2, 0x3f, 0x04, 0x54, 0x88, 0xe4,
    0x06, 0x27, 0x57, 0xb0, 0x6b, 0x9e, 0xba, 0xe1,
    0x83, 0xfc, 0x5a, 0x59, 0x46, 0xd8, 0x0d, 0xb9,
    0x3f, 0xec, 0x6f, 0x62, 0xec, 0x07, 0xe3, 0x72,
    0x7f, 0x01, 0x26, 0xae, 0xd1, 0x2c, 0xe4, 0xb2,
    0x62, 0xf4, 0x7d, 0x48, 0xd5, 0x42, 0x87, 0xf8,
    0x1d, 0x47, 0x4c, 0x7c, 0x3b, 0x18, 0x50, 0xe9
};
static const unsigned char sskdf_otherinfo[] = {
    0xa1, 0xb2, 0xc3, 0xd4, 0xe5, 0x43, 0x41, 0x56,
    0x53, 0x69, 0x64, 0x3c, 0x83, 0x2e, 0x98, 0x49,
    0xdc, 0xdb, 0xa7, 0x1e, 0x9a, 0x31, 0x39, 0xe6,
    0x06, 0xe0, 0x95, 0xde, 0x3c, 0x26, 0x4a, 0x66,
    0xe9, 0x8a, 0x16, 0x58, 0x54, 0xcd, 0x07, 0x98,
    0x9b, 0x1e, 0xe0, 0xec, 0x3f, 0x8d, 0xbe
};
static const unsigned char sskdf_expected[] = {
    0xa4, 0x62, 0xde, 0x16, 0xa8, 0x9d, 0xe8, 0x46,
    0x6e, 0xf5, 0x46, 0x0b, 0x47, 0xb8
};


static int self_test_kdf( )
{
    OSSL_LIB_CTX* libctx = NULL;
    OSSL_PROVIDER* fipsp = OSSL_PROVIDER_load(NULL, "fips"); //"default"  "fips"
    if (fipsp)
    {
        libctx = OSSL_LIB_CTX_new();
    }
    int ret = 0;
    unsigned char out[128];
    EVP_KDF* kdf = NULL; 
    EVP_KDF_CTX* ctx = NULL;
    BN_CTX* bnctx = NULL;
    OSSL_PARAM* params = NULL;
    OSSL_PARAM_BLD* bld = NULL;

    bld = OSSL_PARAM_BLD_new();

#if 0
    kdf = EVP_KDF_fetch(libctx, "HKDF", ""); //SSKDF
    ctx = EVP_KDF_CTX_new(kdf);
    bnctx = BN_CTX_new_ex(libctx);

    OSSL_PARAM_BLD_push_utf8_string(bld, "digest", "SHA256", 6);
    OSSL_PARAM_BLD_push_octet_string(bld, "key", hkdf_secret, sizeof(hkdf_secret));
    OSSL_PARAM_BLD_push_octet_string(bld, "salt", hkdf_salt, sizeof(hkdf_salt));
    OSSL_PARAM_BLD_push_octet_string(bld, "info", hkdf_info, sizeof(hkdf_info));

    params = OSSL_PARAM_BLD_to_param(bld);

    if (EVP_KDF_derive(ctx, out, sizeof(hkdf_expected), params) <= 0)
        goto err;

    if (memcmp(out, hkdf_expected, sizeof(hkdf_expected)) != 0)
        goto err;
#else
    kdf = EVP_KDF_fetch(libctx, "SSKDF", ""); //SSKDF
    ctx = EVP_KDF_CTX_new(kdf);
    bnctx = BN_CTX_new_ex(libctx);

    OSSL_PARAM_BLD_push_utf8_string(bld, "digest", "SHA224", 6);
    OSSL_PARAM_BLD_push_octet_string(bld, "key", sskdf_secret, sizeof(sskdf_secret));
    OSSL_PARAM_BLD_push_octet_string(bld, "info", sskdf_otherinfo, sizeof(sskdf_otherinfo));

    params = OSSL_PARAM_BLD_to_param(bld);

    if (EVP_KDF_derive(ctx, out, sizeof(sskdf_expected), params) <= 0)
        goto err;

    if (memcmp(out, sskdf_expected, sizeof(sskdf_expected)) != 0)
        goto err;

#endif

    ret = 1;

err:
    EVP_KDF_free(kdf);
    EVP_KDF_CTX_free(ctx);
    BN_CTX_free(bnctx);
    OSSL_PARAM_free(params);
    OSSL_PARAM_BLD_free(bld);

    return ret;
}

#endif // 0







#define SSKDF_MAX_INLEN (1<<30)
/*
 * Refer to https://csrc.nist.gov/publications/detail/sp/800-56c/rev-1/final
 * Section 4. One-Step Key Derivation using H(x) = hash(x)
 * Note: X9.63 also uses this code with the only difference being that the
 * counter is appended to the secret 'z'.
 * i.e.
 *   result[i] = Hash(counter || z || info) for One Step OR
 *   result[i] = Hash(z || counter || info) for X9.63.
 */
static int SSKDF_hash_kdm(const EVP_MD* kdf_md,
    const unsigned char* z, size_t z_len,
    const unsigned char* info, size_t info_len,
    unsigned int append_ctr,
    unsigned char* derived_key, size_t derived_key_len)
{
    int ret = 0, hlen;
    size_t counter, out_len, len = derived_key_len;
    unsigned char c[4];
    unsigned char mac[EVP_MAX_MD_SIZE];
    unsigned char* out = derived_key;
    EVP_MD_CTX* ctx = NULL, * ctx_init = NULL;

    if (z_len > SSKDF_MAX_INLEN || info_len > SSKDF_MAX_INLEN
        || derived_key_len > SSKDF_MAX_INLEN
        || derived_key_len == 0)
        return 0;

    hlen = EVP_MD_get_size(kdf_md);
    if (hlen <= 0)
        return 0;
    out_len = (size_t)hlen;

    ctx = EVP_MD_CTX_create();
    ctx_init = EVP_MD_CTX_create();
    if (ctx == NULL || ctx_init == NULL)
        goto end;

    if (!EVP_DigestInit(ctx_init, kdf_md))
        goto end;

    for (counter = 1;; counter++) {
        c[0] = (unsigned char)((counter >> 24) & 0xff);
        c[1] = (unsigned char)((counter >> 16) & 0xff);
        c[2] = (unsigned char)((counter >> 8) & 0xff);
        c[3] = (unsigned char)(counter & 0xff);

        if (!(EVP_MD_CTX_copy_ex(ctx, ctx_init)
            && (append_ctr || EVP_DigestUpdate(ctx, c, sizeof(c)))
            && EVP_DigestUpdate(ctx, z, z_len)
            && (!append_ctr || EVP_DigestUpdate(ctx, c, sizeof(c)))
            && EVP_DigestUpdate(ctx, info, info_len)))
            goto end;
        if (len >= out_len) {
            if (!EVP_DigestFinal_ex(ctx, out, NULL))
                goto end;
            out += out_len;
            len -= out_len;
            if (len == 0)
                break;
        }
        else {
            if (!EVP_DigestFinal_ex(ctx, mac, NULL))
                goto end;
            memcpy(out, mac, len);
            break;
        }
    }
    ret = 1;
end:
    EVP_MD_CTX_destroy(ctx);
    EVP_MD_CTX_destroy(ctx_init);
    OPENSSL_cleanse(mac, sizeof(mac));
    return ret;
}