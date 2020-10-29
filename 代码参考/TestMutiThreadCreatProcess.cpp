// TestMutiThread.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <stdio.h>

#include <windows.h>
#include <SetupAPI.h>
#pragma comment(lib,"Setupapi.lib")



#define DEVAPI  __stdcall
#pragma comment(lib,"mtoken_gm3000_ex.lib")
typedef void *				HANDLE;
typedef HANDLE				DEVHANDLE;

#ifdef __cplusplus
extern "C" {
#endif

	ULONG DEVAPI MKF_SetEnumDeviceString(LPSTR szInq, LPSTR szOEMID);
	ULONG DEVAPI MKF_GetDeviceCount(DEVHANDLE hDev[32]);
	ULONG DEVAPI MKF_ConnectDev(DEVHANDLE hDev);
	ULONG DEVAPI MKF_DevAuth(DEVHANDLE hDev, BYTE *pbAuthData, ULONG ulLen);
	ULONG DEVAPI MKF_SetSerialNumber(DEVHANDLE hDev, char * serialNumber, ULONG length);

	ULONG DEVAPI MKF_3DES_SelfTest(DEVHANDLE hDev);
	ULONG DEVAPI MKF_SM3_SelfTest(DEVHANDLE hDev);
	ULONG DEVAPI MKF_SM2_SelfTest(DEVHANDLE hDev);
	ULONG DEVAPI MKF_SM4_SelfTest(DEVHANDLE hDev);
	ULONG DEVAPI MKF_SM1_SelfTest(DEVHANDLE hDev);
	ULONG DEVAPI MKF_RSA_SelfTest(DEVHANDLE hDev, ULONG bitLen);
	ULONG DEVAPI MKF_AES_SelfTest(DEVHANDLE hDev);

	ULONG DEVAPI SKF_EnumDev(BOOL bPresent, LPSTR szNameList, ULONG *pulSize);
	ULONG DEVAPI SKF_ConnectDev(LPSTR szName, DEVHANDLE *phDev);
	ULONG DEVAPI SKF_DisConnectDev(DEVHANDLE hDev);

#ifdef __cplusplus
}
#endif

bool EnumDevice();

#if 1

int main(int argc, char* argv[])
{
	EnumDevice();

	ULONG ulRslt = 0;
	HANDLE hDev = 0;

	BYTE bAuthKey[17] = "1234567812345678";

	char szDevName[1024] = { 0 };
	ULONG ulNamelen = 1024;
	int num = 0;

	MKF_SetEnumDeviceString((LPSTR)"mtoken,vid_055c&pid_0223,gmpcsc", (LPSTR)"*");
	
#if 1
		
		SKF_EnumDev(1, szDevName, &ulNamelen);
		ulRslt = SKF_ConnectDev(szDevName,&hDev);
		if(0 != ulRslt)
		{
			printf("SKF_ConnectDev %s error\n",szDevName);
			return 0;
		}

		ulRslt = MKF_DevAuth(hDev, bAuthKey, 16);
		if(0 != ulRslt)
		{
			printf("MKF_DevAuth %s error\n",argv[1]);
			return 0;
		}

		ulRslt = MKF_SetSerialNumber(hDev,(char*)"20201029",strlen("20201027"));
		if(0 != ulRslt)
			return 0;

		
#endif

	if (2 != argc)
	{
		printf("arg error\n");
		return 1;
	}

	do
	{
		HANDLE devHandle[32] = { 0 };

		ULONG ulCount = MKF_GetDeviceCount(devHandle);


		hDev = devHandle[atoi(argv[1])];

		ulRslt = MKF_ConnectDev(hDev);
		if (0 != ulRslt)
		{
			printf("SKF_ConnectDev %s error\n", argv[1]);
			break;
		}
	
		ulRslt = MKF_3DES_SelfTest(hDev);
		if (0 != ulRslt)
		{
			printf("MKF_3DES_SelfTest %s error\n", argv[1]);
			break;
		}

		ulRslt = MKF_SM3_SelfTest(hDev);
		if (0 != ulRslt)
		{
			printf("MKF_SM3_SelfTest %s error\n", argv[1]);
			break;
		}

		ulRslt = MKF_SM2_SelfTest(hDev);
		if (0 != ulRslt)
		{
			printf("MKF_SM2_SelfTest %s error\n", argv[1]);
			break;
		}

		ulRslt = MKF_SM4_SelfTest(hDev);
		if (0 != ulRslt)
		{
			printf("MKF_SM4_SelfTest %s error\n", argv[1]);
			break;
		}

		ulRslt = MKF_SM1_SelfTest(hDev);
		if (0 != ulRslt)
		{
			printf("MKF_SM1_SelfTest %s error\n", argv[1]);
			break;
		}

		ulRslt = MKF_RSA_SelfTest(hDev, 1024);
		if (0 != ulRslt)
		{
			printf("MKF_RSA_SelfTest %s error\n", argv[1]);
			break;
		}

		ulRslt = MKF_AES_SelfTest(hDev);
		if (0 != ulRslt)
		{
			printf("MKF_AES_SelfTest %s error\n", argv[1]);
			break;
		}

		ulRslt = SKF_DisConnectDev(hDev);
		if (0 != ulRslt)
		{
			printf("SKF_DisConnectDev %s error\n", argv[1]);
			break;

		}

		//printf(" %s ok \n",argv[1]);

	} while (0);

	return ulRslt;
}


#else

void setsn(int index);
void setsnE(const char* keyPath);


#define KEYNUM 3
#define RUNEXE "eccS.exe"  //"init_mscp.exe"

int main()
{
	std::thread t[KEYNUM];

	do
	{

#if 0
		//MKF_SetEnumDeviceString((LPSTR)"mtoken,vid_055c&pid_0223,gmpcsc", (LPSTR)"*");

		char device_pathes[32][256] = { 0 };

		int num = discover_devices((char*)"cryptoid", device_pathes);

		if (KEYNUM != num)
			continue;
		
		for (int i = 0; i < num; i++) {
			t[i] = std::thread(setsn, device_pathes[i]);
		}

#else
		char device_pathes[KEYNUM][100] = { 0 };
		char szDevName[1024] = { 0 };
		ULONG ulNamelen = 1024;
		int num = 0;
		HANDLE devHandle[32] = { 0 };	

		num = MKF_GetDeviceCount(devHandle);

		if (KEYNUM != num)
		{
			continue;
		}

		Sleep(2000);
		for (int i = 0; i < num; i++) {
			t[i] = std::thread(setsn, i);
		}
#endif


		for (int i = 0; i < num; i++) {
			t[i].join();
		}

		Sleep(3000);

		printf("\n\n----------------------\n\n");
	} while (1);

	return 0;
}

void setsn(int index)
{
	char szCmd[2048] = { 0 };
	sprintf(szCmd, "%s %d ", RUNEXE,index);

	STARTUPINFO StartInfo;
	PROCESS_INFORMATION procStruct;
	memset(&StartInfo, 0, sizeof(STARTUPINFO));
	StartInfo.cb = sizeof(STARTUPINFO);
	BOOL working = ::CreateProcess(NULL, szCmd, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &StartInfo, &procStruct);
	if (working == 0)
	{
		printf("error CreateProcess --- %s\n", szCmd);

		DWORD error = GetLastError();
		return;
	}
	WaitForSingleObject(procStruct.hProcess, INFINITE);
	unsigned long Result;
	GetExitCodeProcess(procStruct.hProcess, &Result);
	printf("error --- %0x \n", Result);
}



int discover_devices(char * parameter, char device_pathes[32][256]);



void setsnE(const char* keyPath)
{
	/*
	exe = 'init_mscp.exe '
	//devPath = "\\?\usbstor#cdrom&ven_longmai&prod_mtoken_cryptoide&rev_3.00#8&36e61f9b&0#{53f56308-b6bf-11d0-94f2-00a0c91efb8b}"
	mod = '2'
	amdpin = 'admin123'
	usrpin = '12345678'
	cd = 'disableCD'
	sn = '20201030'
	*/
	char szCmd[2048] = { 0 };
	sprintf(szCmd, "%s %s 2 admin123 12345678 disableCD 20201030", RUNEXE, keyPath);
	//ShellExecute(NULL, "open", "init_mscp.exe", szCmd, NULL, 0);

	STARTUPINFO StartInfo;
	PROCESS_INFORMATION procStruct;
	memset(&StartInfo, 0, sizeof(STARTUPINFO));
	StartInfo.cb = sizeof(STARTUPINFO);
	BOOL working = ::CreateProcess(NULL, szCmd, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &StartInfo, &procStruct);
	if (working == 0)
	{
		printf("error CreateProcess --- %s\n", szCmd);

		DWORD error = GetLastError();
		return;
	}
	WaitForSingleObject(procStruct.hProcess, INFINITE);
	unsigned long Result;
	GetExitCodeProcess(procStruct.hProcess, &Result);
	printf("error --- %0x \n", Result);
}

#endif












bool EnumDevice()
{
	DWORD dwFlag = (DIGCF_ALLCLASSES | DIGCF_PRESENT);
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, dwFlag);
	if (INVALID_HANDLE_VALUE == hDevInfo)
		return false;

	SP_DEVINFO_DATA sDevInfoData;
	sDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	TCHAR szDIS[MAX_PATH] = { 0 };
	int index = 0;
	DWORD nSize = 0;
	for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &sDevInfoData); i++)
	{
		nSize = 0;
		memset(szDIS, 0, MAX_PATH);

		if (!SetupDiGetDeviceInstanceId(hDevInfo, &sDevInfoData, szDIS, sizeof(szDIS), &nSize))
			break;

		if (strstr(_strlwr(szDIS), "usb\\"))
			printf("%s\n", szDIS);
		/*
		if (strstr(_strlwr(szDIS), "usb\\vid_1234&pid_abcd")) //usb\vid_1234&pid_abcd
		{
			printf("[%d] Î´ÏÂÔØCOS\t%s\n", ++index, _strlwr(szDIS));
		}

		if (strstr(_strlwr(szDIS), "usb\\vid_101d&pid_c002"))
		{
			printf("[%d] Î´Éú²ú\t%s\n", ++index, _strlwr(szDIS));
		}
		*/
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return true;
	
}





long get_device_path(GUID *ClassGuid, HDEVINFO IntDevInfo, DWORD Index, char devPath[256])
{
	unsigned char						interfaceDetail[1024] = { 0 };

	SP_DEVICE_INTERFACE_DATA            interfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA    interfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)interfaceDetail;
	BOOL                                status;

	DWORD								interfaceDetailDataSize, reqSize, errorCode;

	interfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);
	status = SetupDiEnumDeviceInterfaces(
		IntDevInfo,              // Interface Device Info handle
		0,                       // Device Info data
		ClassGuid,
		Index,                   // Member
		&interfaceData           // Device Interface Data
	);

	if (status == FALSE)
	{
		errorCode = GetLastError();
		return errorCode;
	}

	interfaceDetailDataSize = 1024;
	//interfaceDetailData->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);

	//Here is a windows bug, must be 5
	if (sizeof(void*) == 8) // X64
	{
		interfaceDetailData->cbSize = 8;
	}
	else
	{
		interfaceDetailData->cbSize = 5;  //sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
	}

	status = SetupDiGetDeviceInterfaceDetail(
		IntDevInfo,               // Interface Device info handle
		&interfaceData,           // Interface data for the event class
		interfaceDetailData,      // Interface detail data
		interfaceDetailDataSize,  // Interface detail data size
		&reqSize,                 // Buffer size required to get the detail data
		NULL);                    // Interface device info

	if (status == FALSE)
	{
		return GetLastError();
	}

	//
	// Now we have the device path. Open the device interface
	// to send Pass Through command
	strcpy(devPath, interfaceDetailData->DevicePath);
	return 0;
}

int discover_devices(GUID *ClassGuid, char * parameter, char device_pathes[32][256])
{
	HDEVINFO         hIntDevInfo;
	DWORD           index;
	long            status;
	char			szDevId[1024] = { 0 };
	char			szDevPath[1024] = { 0 };
	long			number = 0;

	strcpy(szDevId, parameter);

	hIntDevInfo = SetupDiGetClassDevs(
		ClassGuid,
		NULL,                                   // Enumerator
		NULL,                                   // Parent Window
		(DIGCF_PRESENT | DIGCF_INTERFACEDEVICE  // Only Devices present & Interface class
			));

	if (hIntDevInfo == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	//
	//  Enumerate all the CD devices
	//
	index = 0;
	while (TRUE)
	{
		status = get_device_path(ClassGuid, hIntDevInfo, index, szDevPath);
		if (status == 0)
		{
			_strlwr(szDevPath);

			if (NULL != strstr(szDevPath, szDevId)) //Found
			{
				strncpy(device_pathes[number], szDevPath, 256);
				number++;
			}
		}
		else if (status == ERROR_NO_MORE_ITEMS)
		{
			break;
		}

		index++;
	}

	SetupDiDestroyDeviceInfoList(hIntDevInfo);

	return number;
}

int discover_devices(char * parameter, char device_pathes[32][256])
{
	long			number = 0;

	number = discover_devices((LPGUID)&CdRomClassGuid, parameter, device_pathes);

	return number;
}