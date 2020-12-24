#pragma once

#include <stdint.h>
#include <string>
using namespace std;

enum OTPType {
	TOTP,
	HOTP,
	OTPNONE
};

enum OTPAlgorithm {
	SHA1,
	SHA256,
	SHA512,
	MD5,
	SM3,
	ALGNONE
};

enum SKeyAlgorithm {
	SBASE32,
	SBASE64,
	SHEX,
	SNONE
};



typedef struct otp_params {
	const char *method;//totp  totp-60-6  hotp
	const char *hash; //EVP_get_digestbyname

	const char *encoding; // base32 hex text
	const char *key; //被 encoding 算法加密的 secret key

	const char *pin;// 算出的最终结果 ？

	const char *udid;//hash 混淆
} otp_params_t;

class Account
{
public:
	Account();
	~Account();


private:
	string _name;		//用户
	string _description ;//描述

	//OTP(K,C) = Truncate(HMAC-SHA-1(K,C))
	OTPType _type = OTPNONE;//otp类型
	OTPAlgorithm _otpAlgorithm = ALGNONE;//otp算法
	SKeyAlgorithm _keyAlgorithm = SNONE;

	string _secret;	//K 种子码  被_keyAlgorithm加密
	uint32_t _digits;	   //Digit 位数	 6 \7\ 8
	uint32_t _otp;	//8-byte counter value	
};


