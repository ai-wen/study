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