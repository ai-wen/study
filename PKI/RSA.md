# ![rsa签名验签 原理 数据填充](OpenSSL基于RSA算法的签名和验证(原理+代码).png)
# [libtomcrypt](https://github.com/libtom/libtomcrypt)
libtomcrypt/src/pk/pkcs1/  路径下包含pkcs1 填充算法  
pkcs_1_i2osp.c 
pkcs_1_mgf1.c  
pkcs_1_oaep_decode.c  
pkcs_1_oaep_encode.c  
pkcs_1_os2ip.c  
pkcs_1_pss_decode.c  
pkcs_1_pss_encode.c  
pkcs_1_v1_5_decode.c  
pkcs_1_v1_5_encode.c  




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


### RSAES-PKCS1-v1_5 加解密时对原文的填充格式
|EM = |1字节|1字节|(模长-原文长-3)个字节| 1字节|原文长度(不大于模长-11)个字节| 
| - | - | - | - | - |- |
|EM=|0x00|0x02|PS(随机数)|0x00|M(原文)|

 * RSAES-PKCS1-v1_5 加密流程  
1：待加密数据为M，规范要求M必须不大于 模长-11。   
2：满足1，然后生成每字节为非0的随机值PS，PS的长度是 k - 原文长 - 3   
EM= 0x00 || 0x02 || PS || 0x00 || M    
C = EM ^e mod n
* RSAES-PKCS1-v1_5 解密流程  
1：校验C的长度，C必须是模长。   
2：C ^d mod n得到EM  
EM理论上是0x00 || 0x02 || PS || 0x00 || M这种格式的，所以校验的方法也相对比较简单。   
先判断开头2字节是否是0x00 0x02，然后找到第一个0x00，这个0x00后面的值就是解密后的明文 。  

### RSASSA-PKCS1-V1_5-SIGN 签名验签时对原文哈希值的填充格式
封装格式名 EMSA-PKCS1-v1_5
|EM = |1字节|1字节|(模长-T的长度-3)个字节| 1字节|T的长度个字节| 
| - | - | - | - | - |- |
|EM=|0x00|0x01|PS(随机数)|0x00|T|

* RSASSA-PKCS1-V1_5-SIGN 签名流程  
1：计算M的哈希值，H = hash(M)，哈希算法可以是MD5、SHA1、SHA2等算法。   
2：对H进行ASN1(DigestInfo)编码得到T 。如果哈希算法确定，即oid确定，那么哈希数据H之前的ASN1数据都是确定的值。 
EM = 0x00 || 0x01 || PS || 0x00 || T   
C = EM^d mod n      
``` cpp
    DigestInfo ::= SEQUENCE {  
         digestAlgorithm AlgorithmIdentifier,  
         digest OCTET STRING  
     }  

    p = T
    *p++ = ASN1_SEQUENCE | ASN1_CONSTRUCTED;
    *p++ = (unsigned char) ( 0x08 + oid_size + hashlen );
    *p++ = ASN1_SEQUENCE | ASN1_CONSTRUCTED;
    *p++ = (unsigned char) ( 0x04 + oid_size );
    *p++ = ASN1_OID;
    *p++ = oid_size & 0xFF;
    memcpy( p, oid, oid_size );
    *p += oid_size;
    *p++ = ASN1_NULL;
    *p++ = 0x00;
    *p++ = ASN1_OCTET_STRING;
    *p++ = hashlen;
    memcpy( p, H, hashlen );//哈希值在这里

    MD2:     (0x)30 20 30 0c 06 08 2a 86 48 86 f7 0d 02 02 05 00 04 10 || H.
    MD5:     (0x)30 20 30 0c 06 08 2a 86 48 86 f7 0d 02 05 05 00 04 10 || H.
    SHA-1:   (0x)30 21 30 09 06 05 2b 0e 03 02 1a 05 00 04 14 || H.
    SHA-224:  (0x)30 2d 30 0d 06 09 60 86 48 01 65 03 04 02 04 05 00 04 1c || H.
    SHA-256: (0x)30 31 30 0d 06 09 60 86 48 01 65 03 04 02 01 05 00 04 20 || H.
    SHA-384: (0x)30 41 30 0d 06 09 60 86 48 01 65 03 04 02 02 05 00 04 30 || H.
    SHA-512: (0x)30 51 30 0d 06 09 60 86 48 01 65 03 04 02 03 05 00 04 40 || H.
    SHA-512/224:  (0x)30 2d 30 0d 06 09 60 86 48 01 65 03 04 02 05 05 00 04 1c || H.
    SHA-512/256:  (0x)30 31 30 0d 06 09 60 86 48 01 65 03 04 02 06 05 00 04 20 || H.
```
* RSASSA-PKCS1-V1_5-SIGN 验签流程  
    与签名流程相逆



### RSASSA-PSS 签名验签时原文填充的格式
封装格式名 EMSA-PSS
|DB = |（模长-2*哈希个字节-2）字节|1字节|hash个字节| 
| - | - | - | - |
|DB=|PS(全0填充)|0x01|salt|

|EM = |maskDB(MGF运算后的DB)|hash个字节|1个字节| 
| - | - | - | - |
|EM = |maskDB|M'|0XBC|

EM = PS || 0x01 || salt || M' || 0XBC  

* 签名流程
1：计算 mHash  = Hash(M)    
2：生成上个步骤中哈希长度的随机值salt  
3：计算 M' = Hash(00 00 00 00 00 00 00 00(8字节长) || mHash  || salt)  
4：计算 DB = PS（全0字节,长度为模长-2*哈希个字节-2） || 0x01 || salt   
5：进行MGF运算得到 maskedDB     
6：签名运算  
 EM = maskDB || M' || 0xBC   
 C = EM^d mod n  

                                 +-----------+  
                                 |     M     |
                                 +-----------+
                                       |
                                       V
                                     Hash
                                       |
                                       V
                         +--------+----------+----------+
                    M' = |Padding1|  mHash   |   salt   |
                         +--------+----------+----------+
                                        |
              +--------+----------+     V
        DB =  |Padding2|   salt   |   Hash
              +--------+----------+     |
                        |               |
                        V               |
                       xor <--- MGF <---|
                        |               |
                        |               |
                        V               V
              +-------------------+----------+--+
        EM =  |    maskedDB       |     H    |bc|
              +-------------------+----------+--+
 
*  验签流程 
1：解密C，得到的结果是 EM。   
2：解封装EM。EM解密出的结果是EM = maskDB || M' || 0xBC  
3：恢复salt  
    salt就是M’前的hashlen字节的值，只是被mask亦或了。我们只需要再被mask亦或一次，就能恢复salt。  
4：执行MGF,

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