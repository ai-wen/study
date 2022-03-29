// testcng.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "..\GM3000CSP_CNG\Gm3000Ksp.h"
#pragma comment(lib,"../GM3000CSP_CNG/Debug/GM3000CSP_CNG.lib")

int main()
{
   
	DWORD dwRet = GM3000RegKsp(GM3000_KSP_PROVIDER_NAME);
	//dwRet = GM3000UnRegKsp(GM3000_KSP_PROVIDER_NAME);


	return 0;

}

