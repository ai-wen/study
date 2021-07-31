// DevControl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <SetupAPI.h>
#pragma comment(lib,"Setupapi.lib")


bool EnumAllDevice();
void EnumAndControlDevice(int control_code);


int _tmain(int argc, _TCHAR* argv[])
{

	//EnumAllDevice();
	//EnumAndControlDevice(DICS_ENABLE);
	EnumAndControlDevice(DICS_DISABLE);

	getchar();

	return 0;
}



/*
#define DICS_ENABLE      0x00000001
#define DICS_DISABLE     0x00000002
#define DICS_PROPCHANGE  0x00000003
#define DICS_START       0x00000004
#define DICS_STOP        0x00000005
*/
void EnumAndControlDevice(int controlCode)
{
/*
SetupDiGetClassDevs                   // ��ȡ�豸��Ϣ��
SetupDiEnumDeviceInfo                 // ���豸��Ϣ����ö��ÿ���豸�ľ�����Ϣ
SetupDiGetDeviceRegistryProperty      // ��ע����ж�ȡPnP�豸������
SetupDiSetClassInstallParams          // ���ã�����ȡ�����豸��İ�װ����
SetupDiCallClassInstaller             // ��װָ���豸
*/

	BOOL rlt = FALSE;

	//Microsoft Usbccid Smartcard Reader (WUDF)
	GUID    Guid = { 0x50dd5230, 0xba8a, 0x11d1, { 0xbf, 0x5d, 0x00, 0x00, 0xf8, 0x05, 0xf5, 0x30 } };

	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	hDevInfo = SetupDiGetClassDevs(&Guid, 0, 0, DIGCF_PRESENT);

	if (INVALID_HANDLE_VALUE == hDevInfo)
	{
		printf("SetupDiGetClassDevs error\n");
		return;
	}
	
	SP_DEVINFO_DATA DeviceData;
	RtlZeroMemory(&DeviceData, sizeof(SP_DEVINFO_DATA));
	DeviceData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceData); i++)
	{

		TCHAR deviceId[1024] = { 0 };
		DWORD requiredSize = 0;

		if (SetupDiGetDeviceInstanceId(hDevInfo, &DeviceData, deviceId, sizeof(deviceId), &requiredSize))
		{
			printf("%s\n", deviceId);
		}

		DWORD DataT;
		LPTSTR buffer = NULL;
		DWORD buffersize = 0;
		while (!SetupDiGetDeviceRegistryProperty(hDevInfo,&DeviceData,SPDRP_DEVICEDESC,
			&DataT,(PBYTE)buffer,buffersize,&buffersize))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				// Change the buffer size.
				if (buffer)
				{
					LocalFree(buffer);
				}
				buffer = (char*)LocalAlloc(LPTR, buffersize);
			}
			else
			{
				break;
			}
		}		

		if (buffer)
		{
			printf("%s\n", buffer); //Microsoft Usbccid Smartcard Reader (WUDF)
			LocalFree(buffer);
		}		

		SP_PROPCHANGE_PARAMS propChange = { sizeof(SP_CLASSINSTALL_HEADER) };
		propChange.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
		propChange.Scope = DICS_FLAG_GLOBAL;
		propChange.StateChange = controlCode; // ���ã�����ͣ�ã���ʹ��DICS_DISABLE

		if (DeviceData.DevInst != NULL)
		{
			rlt = SetupDiSetClassInstallParams(hDevInfo, &DeviceData, (SP_CLASSINSTALL_HEADER *)&propChange, sizeof(propChange));
		}

		if (rlt)
		{
			rlt = SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &DeviceData);
		}

		if (5 == GetLastError())
		{
			printf("Ȩ�޲���\n");
		}

		if (0xE0000235 == GetLastError())
		{
			printf("����λ����ϵͳ��ƥ��\n");
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
	//printf("%0x\n", GetLastError()); //5 Ȩ�޲���  E0000235 ����λ����ϵͳ��ƥ��
	
}


bool EnumAllDevice()
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

		printf("%s\n", szDIS);
/*
		if (strstr(_strlwr(szDIS), "usb\\vid_055c&pid_0223"))
			printf("%s\n", szDIS);

		if (strstr(_strlwr(szDIS), "usb\\vid_1234&pid_abcd")) //usb\vid_1234&pid_abcd
		{
			printf("[%d] δ����COS\t%s\n", ++index, _strlwr(szDIS));
		}

		if (strstr(_strlwr(szDIS), "usb\\vid_101d&pid_c002"))
		{
			printf("[%d] δ����\t%s\n", ++index, _strlwr(szDIS));
		}
		*/
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return true;
	
}
