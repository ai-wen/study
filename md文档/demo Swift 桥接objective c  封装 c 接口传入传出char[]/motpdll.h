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
@stype			【IN】种子码 编码类型 SKeyAlgorithm
@hasAlg			【IN】使用的hmac算法 OTPAlgorithm
@secret			【IN】种子码
@moving_factor	【IN】产生当前动态口令所需的counter C  ,一个 8个 byte的数值
@digits			【IN】即将产生的动态口令的位数
@code			【OUT】动态口令
*/


int otp_compute_code(int stype, int hasAlg, const char *secret,
					 uint64_t moving_factor, int digits, char* code);

/** 
获取UTC标准时间
*/
ULONGLONG GetUtcTime();

#ifdef __cplusplus
}
#endif

#endif

