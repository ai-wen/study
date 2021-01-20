#ifndef _MOTPDLL_H_
#define _MOTPDLL_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <Windows.h>
	typedef unsigned __int64 ULONGLONG;
#else
	typedef unsigned long ULONGLONG;
#include <string.h>
#endif

#include <stdio.h>
#include <stdint.h>

enum SKeyAlgorithm {
	SBASE32, 
	SHEX
};

enum OTPAlgorithm {
	HSHA1,
	HSHA224,
	HSHA256,
	HSHA384,
	HSHA512,
	HMD5,
	HSM3
};



/** 
@stype			��IN�������� �������� SKeyAlgorithm
@hasAlg			��IN��ʹ�õ�hmac�㷨 OTPAlgorithm
@secret			��IN��������
@moving_factor	��IN��������ǰ��̬���������counter C  ,һ�� 8�� byte����ֵ
@digits			��IN�����������Ķ�̬�����λ��
@code			��OUT����̬����
*/


int otp_compute_code(int stype, int hasAlg, const char *secret,
					 uint64_t moving_factor, int digits, char* code);

/** 
��ȡUTC��׼ʱ��
*/
ULONGLONG GetUtcTime();

#ifdef __cplusplus
}
#endif

#endif

