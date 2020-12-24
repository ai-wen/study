// optserver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>


int  strtohex(const char *string, unsigned char *cbuf, int len)
{
	unsigned char high, low;
	int idx, ii = 0;
	for (idx = 0; idx < len; idx += 2)
	{
		high = string[idx];
		low = string[idx + 1];

		if (high >= '0' && high <= '9')
			high = high - '0';
		else if (high >= 'A' && high <= 'F')
			high = high - 'A' + 10;
		else if (high >= 'a' && high <= 'f')
			high = high - 'a' + 10;
		else
			return -1;

		if (low >= '0' && low <= '9')
			low = low - '0';
		else if (low >= 'A' && low <= 'F')
			low = low - 'A' + 10;
		else if (low >= 'a' && low <= 'f')
			low = low - 'a' + 10;
		else
			return -1;

		cbuf[ii++] = high << 4 | low;
	}
	return 0;
}


// GenSecretKey Computes the 80-bit base32 encoded `secret key

#include "base32.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    
	while (1)
	{
		char cbuf[41] = { 0 };
		printf("\n请输入种子码:\t");

		scanf("%s", cbuf);

		unsigned char ckey[21] = { 0 };
		strtohex((const char *)cbuf, ckey, 40);

		unsigned char cout[100] = { 0 };

		base32_encode(ckey, 20, cout);

		printf("app密钥:\t%s\n", cout);
	}
	//10字节
	// GenSecretKey Computes the 80-bit base32 encoded `secret key

	std::string skey1 = "B13AFA0E279DE8218437EFED98D81FE6EAF8CF59"; //WE5PUDRHTXUCDBBX57WZRWA743VPRT2Z
	std::string skey2 = "A73E30D304F150DF322D29948EE27C33F738FC00"; //U47DBUYE6FIN6MRNFGKI5YT4GP3TR7AA
	
	unsigned char cbuf[21] = {0};
	strtohex(skey2.c_str(), cbuf, skey2.length());

	unsigned char cout[200] = { 0 };

	base32_encode(cbuf, 20, cout);

/*
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	HMAC_CTX* hmac = HMAC_CTX_new();
	HMAC_CTX_reset(hmac);
	HMAC_Init_ex(hmac, otp_key, key_len, otp_digest, NULL);
	HMAC_Update(hmac, (uint8_t *)&Tn, sizeof(Tn));
	HMAC_Final(hmac, mac, &maclen);
	HMAC_CTX_free(hmac);
#else
	HMAC_CTX hmac;
	HMAC_CTX_init(&hmac);
	HMAC_Init(&hmac, otp_key, key_len, otp_digest);
	HMAC_Update(&hmac, (uint8_t *)&Tn, sizeof(Tn));
	HMAC_Final(&hmac, mac, &maclen);
#endif
*/


	return 0;
}
