RSA_PKCS1_PADDING （参看openssl： ./crypto/rsa/rsa_eay.c ）：  
私钥加密时，使用的是： RSA_padding_add_PKCS1_type_1(…)   pad的是定值 0xff  
公钥加密时，使用的是： RSA_padding_add_PKCS1_type_2(…)   pad的是随机的非零值   
（公钥加密每次结果不同，而私钥加密每次结果相同；对于私钥加密，多半用于签名，相同的数据每次签名结果都相同是很好的）  