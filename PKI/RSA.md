RSA_PKCS1_PADDING （参看openssl： ./crypto/rsa/rsa_eay.c ）：  
私钥加密时，使用的是： RSA_padding_add_PKCS1_type_1(…)   pad的是定值 0xff  
公钥加密时，使用的是： RSA_padding_add_PKCS1_type_2(…)   pad的是随机的非零值   
（公钥加密每次结果不同，而私钥加密每次结果相同；对于私钥加密，多半用于签名，相同的数据每次签名结果都相同是很好的）  

# RSA的填充模式
RSA_SSLV23_PADDING（SSLv23填充）  
RSA_NO_PADDING（不填充）  
RSA_PKCS1_OAEP_PADDING （RSAES-OAEP填充，强制使用SHA1，加密使用）  
RSA_X931_PADDING（X9.31填充，签名使用）  
RSA_PKCS1_PSS_PADDING（RSASSA-PSS填充，签名使用）  
RSA_PKCS1_PADDING（RSAES-PKCS1-v1_5/RSASSA-PKCS1-v1_5填充，签名可使用）  
## 主流签名填充有以下2种  
PSS (Probabilistic Signature Scheme)私钥签名流程的一种填充模式。  
PKCS（Public Key Cryptography Standards）是另一种通用的私钥签名填充模式。 因为pad的是定值 0xff（openssl），所以可以复用签名原文和签名。  
而PSS无法从签名中恢复原来的签名。openssl-1.1.x以后默认使用更安全的PSS的RSA签名模式。  


[参考博客](https://blog.csdn.net/mrpre/article/details/79740313)  
SSA = Signature Scheme with Appendix   
PSS = Probabilistic Signature Scheme   
ES = Encryption Schemes   
SSA是填充、封装格式    
PSS是私钥签名流程。   
ES 是公钥加密流程。  

下面是RSASSA-PSS签名使用的填充方式；   
说明：   
1：示例程序没有进行签名操作，只进行了EMSA-PSS的encode和decode。   
2：示例程序固定使用了sha256进行hash计算。   
3：示例程序的输出256字节。  
``` cpp
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
unsigned char tosigned[] = "aaaaaaaaaaaa\n";
unsigned char encoded[256]={0};
unsigned int hash_len;
const EVP_MD *md;

void mgf1(unsigned char *dst, unsigned int dst_len, unsigned char *src, unsigned int src_len)
{
    unsigned char tmp[EVP_MAX_MD_SIZE] = {0}, tmp2[EVP_MAX_MD_SIZE];
    unsigned char *p = dst, *ctr;
    unsigned int mask_len, hash_len = src_len, i;

    memcpy(tmp, src, src_len);
    ctr = tmp + src_len;

    while((int)dst_len > 0)
    {
        SHA256(tmp, src_len + 4, tmp2);
        mask_len = dst_len < hash_len ? dst_len : hash_len;

        for(i = 0; i < mask_len; i++)
        {
            *p++ ^= tmp2[i];
        }
        dst_len -= mask_len;
        ctr[3]++;
    }
}

void decode(unsigned char *rsa2048)
{
    unsigned int encodelen = 256;
    unsigned char mHash[EVP_MAX_MD_SIZE], selfmap[EVP_MAX_MD_SIZE];
    unsigned char tmp[8 + EVP_MAX_MD_SIZE*2]={0};
    unsigned char *map, *salt;

    /*这个函数应该是decode前被调用，而不是decode中*/
    SHA256(tosigned, strlen(tosigned), mHash);
    /*get M' */
    map = rsa2048 + encodelen - 1 - hash_len;
    /*use M' to do xor*/
    mgf1(rsa2048, encodelen - 1 - hash_len, map, hash_len);
    /*recover the salt in encoded buffer*/
    salt = rsa2048 + encodelen - 1 - hash_len*2;

    /*calc self M' */
    memcpy(tmp + 8, mHash, hash_len);
    memcpy(tmp + 8 + hash_len, salt, hash_len);
    SHA256(tmp, 8 + hash_len + hash_len, selfmap);


    if(!memcmp(map, selfmap, hash_len))
    {
        printf("decode SUCCESS\n");
    }

}

void encode(unsigned char *rsa2048)
{
    unsigned char mHash[EVP_MAX_MD_SIZE], map[EVP_MAX_MD_SIZE];
    unsigned char *salt;
    unsigned char tmp[8 + EVP_MAX_MD_SIZE*2]={0};
    unsigned char *p = rsa2048;
    unsigned int rsa_size = 256;

    /*这个函数应该是encode前被调用，而不是decode中*/
    /*step 1 mHash*/
    SHA256(tosigned, strlen(tosigned), mHash);

    /*step 2 radnom salt*/
    salt = (p + rsa_size - 2 - hash_len*2);

    *salt++ = 0x01;

    /*Random*/
    memset(salt, 0x12, hash_len);

    memcpy(tmp + 8, mHash, hash_len);
    memcpy(tmp + 8 + hash_len, salt, hash_len);
    SHA256(tmp, 8 + hash_len+hash_len, p + rsa_size - hash_len - 1);

    mgf1(rsa2048, rsa_size - hash_len - 1, p + rsa_size - hash_len - 1, hash_len);
    rsa2048[rsa_size - 1] = 0xBC;

    /*Set the leftmost 8emLen - emBits bits of the leftmost octet in maskedDB to zero.*/    
    rsa2048[0] &= 0xFF >> 1;
}

void main()
{
    unsigned char rsa2048[256]={0};
    md = EVP_sha256();
    hash_len = EVP_MD_size(md);

    encode(rsa2048);
    //private_key_enc(rsa2048);
    //private_key_edc(rsa2048);
    decode(rsa2048);
}
```