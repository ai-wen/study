
#include <stdio.h>
#include <string.h>
//#include <iostream>
#include "EnkyCTApp.h"
//using namespace std;

int main()
{
   //cout << "Hello world!" << endl;

	long nRet = 0;

	int j,k;
	char *appID = "MyApplication";
	long keyHandles[8] = {0};
	long keyNumber= 0;
	long Max_Count = 0;
	char Tmp[8] = {0};

    if ( 0 != EnkyCTFind(appID,keyHandles,&keyNumber))
	{
		printf("Failed to EnkyCTFind ！error code = %d 。\n",EnkyCTGetLastError());
		return 1;
	}
	else
	{
		printf("EnkyCTFind successfull \n");
	}


	if ( 0 != EnkyCTFind(appID,keyHandles,&keyNumber))
	{
		printf("Failed to EnkyCTFind ！error code = %d 。\n",EnkyCTGetLastError());
		return 1;
	}
	else
	{
		printf("EnkyCTFind successfull \n");
	}


	if ( 0 != EnkyCTCheckExist(keyHandles[0]))
	{
		printf("Failed to EnkyCTCheckExist, error code = %d 。\n",EnkyCTGetLastError());
		return 2;
	}
	else
		printf("EnkyCTCheckExist successfully \n");


	char UID[33] = {0};
	if ( 0 != EnkyCTGetUid(keyHandles[0],UID))
	{
		printf("Failed to EnkyCTGetUid, error code = %d 。\n",EnkyCTGetLastError());
		return 3;
	}
	else
		printf("EnkyCTGetUid successfully \n");

	long password1 = 0x8E26E0A7,password2 = 0xE7482AFF, password3 = 0xA9BEBE91, password4 = 0x890126B;

	if ( 0 != EnkyCTOpen(keyHandles[0],password1,password2, password3, password4))
	{
		printf("Failed to EnkyCTOpen, error code = %d 。\n",EnkyCTGetLastError());
		return 4;
	}
	else
		printf("EnkyCTOpen successfully \n");

	unsigned char pBuffer_WritePageFile[4096] = {0};
	memcpy(pBuffer_WritePageFile,"ABCDefgh12345678",16);

	for (k = 0; k < 4;k++)
	{
		if ( 0 != EnkyCTWritePageFile(keyHandles[0],k,0,4096,pBuffer_WritePageFile))
		{
			printf("Failed to EnkyCTWritePageFile %d, error code = %d 。\n",k,EnkyCTGetLastError());
			return 5;
		}
		else
			printf("EnkyCTWritePageFile %d successfully \n",k);
	}


	unsigned char pBuffer[4096] = {0};
	for (j = 0;j< 4;j++)
	{
		if ( 0 != EnkyCTReadPageFile(keyHandles[0],j,0,4096,pBuffer))
		{
			printf("Failed to EnkyCTReadPageFile %d,  error code = %d 。\n",j,EnkyCTGetLastError());
			return 6;
		}
		else
			printf("EnkyCTReadPageFile %d successfully \n",j);
	}


	unsigned char pDataBuffer[4096] = {0};
	memcpy(pDataBuffer,"12345678ABCDefgh",16);
	if ( 0 != EnkyCTTriDesEncrypt(keyHandles[0],pDataBuffer,16))
	{
		printf("Failed to EnkyCTTriDesEncrypt, error code = %d 。\n",EnkyCTGetLastError());
		return 7;
	}
	else
		printf("EnkyCTTriDesEncrypt successfully \n");


	if ( 0 != EnkyCTTriDesDecrypt(keyHandles[0],pDataBuffer,16))
	{
		printf("Failed to EnkyCTTriDesDecrypt, error code = %d 。\n",EnkyCTGetLastError());
		return 8;
	}
	else
		printf("EnkyCTTriDesDecrypt successfully \n");

	
	unsigned char pBuffer_ReadMemory[128] = {0};
	if ( 0 != EnkyCTReadMemory(keyHandles[0],0,128,pBuffer_ReadMemory))
	{
		printf("Failed to EnkyCTReadMemory, error code = %d 。\n",EnkyCTGetLastError());
		return 9;
	}
	else
		printf("EnkyCTReadMemory successfully \n");

	
	unsigned char pBuffer_WriteMemory[128] = {0};
	if ( 0 != EnkyCTWriteMemory(keyHandles[0],0,128,pBuffer_WriteMemory))
	{
		printf("Failed to EnkyCTWriteMemory, error code = %d 。\n",EnkyCTGetLastError());
		return 10;
	}
	else
		printf("EnkyCTWriteMemory successfully \n");


	long endYear = 0,endMonth = 0,endDay = 0,endHour = 0,endMin  = 0,endSec = 0;
	if ( 0 != EnkyCTGetExpiryDateTime(keyHandles[0],&endYear,&endMonth,&endDay, &endHour,&endMin ,&endSec))
	{
		printf("Failed to EnkyCTGetExpiryDateTime, error code = %d 。\n",EnkyCTGetLastError());
		return 11;
	}
	else
		printf("EnkyCTGetExpiryDateTime successfully \n");

	long currYear = 0,currMonth = 0,currDay = 0,currHour = 0,currMin  = 0,currSec = 0;
	if ( 0 != EnkyCTGetCurrentDateTime(keyHandles[0],&currYear,&currMonth,&currDay, &currHour,&currMin ,&currSec))
	{
		printf("Failed to EnkyCTGetCurrentDateTime, error code = %d 。\n",EnkyCTGetLastError());
		return 12;
	}
	else
		printf("EnkyCTGetCurrentDateTime successfully \n");

	long Count = 0;
	if ( 0 != EnkyCTGetCount(keyHandles[0],&Count))
	{
		printf("Failed to EnkyCTGetCount, error code = %d 。\n",EnkyCTGetLastError());
		return 13;
	}
	else
		printf("EnkyCTGetCount successfully \n");

	long SoftVersion = 0;
	if ( 0 != EnkyCTGetSoftVersion(keyHandles[0],&SoftVersion))
	{
		printf("Failed to EnkyCTGetSoftVersion ,error code = %d 。\n",EnkyCTGetLastError());
		return 14;
	}
	else
		printf("EnkyCTGetSoftVersion successfully \n");

	char Request[32] = {0};
	if ( 0 != EnkyCTRemoteRequest(keyHandles[0], Request))
	{
		printf("Failed to EnkyCTRemoteRequest, error code = %d 。\n",EnkyCTGetLastError());
		return 15;
	}
	else
		printf("EnkyCTRemoteRequest successfully \n");


// 		char Register[32] = {'A','A','A','A','B','B','B','B','A','A','A','A','B','B','B','B'};
// 		if ( 0 != EnkyCTRemoteRegister(keyHandles[0],Register))
// 		{
// 			if ( 0 != ErrorLog("EnkyCTRemoteRegister",EnkyCTGetLastError(),i))
// 			{
// 				return 17;
// 			}
// 		}
// 		else
// 			printf("EnkyCTRemoteRegister 调用成功！\n");

	if ( 0 != EnkyCTClose(keyHandles[0]))
	{
		printf("Failed to EnkyCTClose, error code = ：%d 。\n",EnkyCTGetLastError());
		return 16;
	}
	else
		printf("EnkyCTClose successfully \n");


	printf("Completed \n");

	return 0;
}
