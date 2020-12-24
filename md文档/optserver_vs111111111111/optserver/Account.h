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
	const char *key; //�� encoding �㷨���ܵ� secret key

	const char *pin;// ��������ս�� ��

	const char *udid;//hash ����
} otp_params_t;

class Account
{
public:
	Account();
	~Account();


private:
	string _name;		//�û�
	string _description ;//����

	//OTP(K,C) = Truncate(HMAC-SHA-1(K,C))
	OTPType _type = OTPNONE;//otp����
	OTPAlgorithm _otpAlgorithm = ALGNONE;//otp�㷨
	SKeyAlgorithm _keyAlgorithm = SNONE;

	string _secret;	//K ������  ��_keyAlgorithm����
	uint32_t _digits;	   //Digit λ��	 6 \7\ 8
	uint32_t _otp;	//8-byte counter value	
};


