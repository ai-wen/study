// DACL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

/*
Windowsϵͳ�п���ͬʱ��½����û��������û��ڲ�ͬ��session�С�
�ڶ��session�����еĳ�����ܻ�ʹ�õ�ͬһ����Դ���ļ���ע���ȣ�����ʱ������Դ�ķ���Ҳ����Ҫͬ�����ơ�
windows�µ�Mutex
1�������ռ�
�����Ҫ�ڶ��session��ʹ��ͬһ�����������壬Ӧʹ�á�Global\��ǰ׺��ʵ������ڲ�ͬ��session��ʹ��CreateMutex����ERROR_ACCESS_DENIED�Ĵ���
2����MSDN�У�����CreateMutex�����ķ���ERROR_ACCESS_DENIED�Ľ����У�����ʹ��OpenMutex����������Mutex����Ĵ򿪲�����������Ȼ���ڡ�
3����MSDN�У�ʹ��ȫ�������ռ���ļ�ӳ�����ʱ����ҪȨ�����������˴�ֻ����ָ�ļ�ӳ����󣬲�Ӧ�ð���Mutex��
Mutex�ɹ���Ա�ʻ���½��session�д���������Ա�ʻ��Ըö����п���Ȩ��֮������һ��session��Ҳʹ�ù���ԱȨ�޴�mutexʱ��
ʹ����ͬһ������Ա�ʻ����൱��ͬһ���ʻ����еĳ���Ҳ����˵����Ŀ���Ȩ�����г���ʱʹ�õ��û��йء�
4������֤�û�������Ա�û�����ȫ���Ƶ�DACLȨ�ޡ�
*/

/*
SDDL �﷨
4�־����﷨���£�
1.O:owner_sid							��������SID�ַ���
2.G:Group_sid							һ��SID�ַ���.��ʶ�������Ҫ��
3.D:(ACE Strings1)(ACE Strings2)(...)	DACL��Ϣ.��ACE���
4.S:(ACE Strings1)(ACE Strings2)(...)	 SACLxin

���ֶ����ͣ�(O:) owner, (G:)primary group, (D:)DACL , (S:)SACL .�����Ҳ���������ᵽ��D�������ļ��ķ��ʿ����б�
ÿ��ACE Strings��7���ֶ�
(ace_type; ace_flags;rights;object_guid;inherit_object_guid;	account_sid; (resource_attribute))
https://docs.microsoft.com/zh-tw/windows/win32/secauthz/ace-strings

����Ȩ�޿����б�Discretionary access control list������һ����д��D����ʾ�����ú��û��б��ACEѡ�������޸ġ�
ϵͳȨ�޿����б�System access control list������һ����д��S����ʾ�� ���ú��û��б��ACE�ɻ�ɫѡ������޸ġ�


DACL��Ϊ�������ʵĿ����б���DACL����������û�������ȫ���ʣ�����һ����򿪷��а�ȫ���Զ��Ǵ���NULL��
����⣬DACL�����û�Ȩ�޼��ϣ�ACEָ�ľ��Ǿ���Ȩ������ѡ�

����һ���Լ����Ƶ��ļ�
1.���ȴ���һ�� ��ȫ���Խṹ��.(���紴���ļ�.�ļ���һ�㴫��NULL.���ڲ���NULL)
2.ʹ��SDDL�ַ���������г�ʼ��.SDDL��һ��NULL-Terminated��β�� ��ȫ�������ַ�����ʽ.(ConvertSecurityDescriptorToStringSecurityDescriptor / ConvertStringSecurityDescriptorToSecurityDescriptor)
3.ʹ�� ʹ�� ConvertStringSecurityDescriptorToSecurityDescriptor ������SDDL�ַ���תΪ��ȫ������.(�����Ǹ��ṹ��)SECURITY_ATTRIBUTES��LpsecurityDscriptor��Ա.
4.ʹ�ù�֮��Ҫʹ�� localFree�����ͷ� lpSecurityDescriptor ������ڴ�.
Ҫʹ�� Conver... ����SDDL�ĺ���.���뽫 _WIN32_WINNT ��������Ϊ0x0500 ���߸���.
*/



#include <windows.h>
#include <AclAPI.h>
#include <Sddl.h>
//#define _WIN32_WINNT 0x0500
#pragma comment(lib,"Advapi32.lib")


HANDLE CreateMyDACLMutex(const char* szMutexName)
{
	HANDLE mutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, szMutexName);
	if (NULL != mutex)
		return mutex;

	SECURITY_ATTRIBUTES  sa;
	memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	/*
	const char * szSD1 = TEXT("S:(ML;;NW;;;LW)");

	const char * szSD = TEXT("D:")	
		//TEXT("(A;OICI;GA;;;AN)") �ܾ�����Ȩ��
		TEXT("(A;OICI;GA;;;AN)")//������¼	ANONYMOUS LOGON
		TEXT("(A;OICI;GA;;;AU)")//��֤��¼�û�  Authenticated users
		
		TEXT("(A;OICI;GA;;;BA)")//���ù���Ա DESKTOP-GLTM519\Administrator ok
		TEXT("(A;OICI;GA;;;BG)")//�����ο� DESKTOP-GLTM519\Guest ok
		TEXT("(A;OICI;GA;;;BU)")//�����û� DESKTOP-GLTM519\Users

		//TEXT("(A;OICI;GA;;;DA)")
		//TEXT("(A;OICI;GA;;;DC)")
		//TEXT("(A;OICI;GA;;;DD)")
		//TEXT("(A;OICI;GA;;;DG)")
		//TEXT("(A;OICI;GA;;;DU)")

		//TEXT("(A;OICI;GA;;;LA)")//���ع���Ա DESKTOP-GLTM519\Administrator  fail
		//TEXT("(A;OICI;GA;;;LG)")//�����ο� DESKTOP-GLTM519\Guest  fail
		TEXT("(A;OICI;GA;;;LS)")//LOCAL SERVICE

		TEXT("(A;OICI;GA;;;SU)") //SERVICE
		TEXT("(A;OICI;GA;;;SY)")//SYSTEM
		TEXT("(A;OICI;GA;;;WD)");//Everyone
		*/
	const char * szSD = "D:(A;OICI;GA;;;AN)(A;OICI;GA;;;AU)(A;OICI;GA;;;BA)(A;OICI;GA;;;BG)(A;OICI;GA;;;BU)(A;OICI;GA;;;LS)(A;OICI;GA;;;SU)(A;OICI;GA;;;SY)(A;OICI;GA;;;WD)";

	if (!ConvertStringSecurityDescriptorToSecurityDescriptor(szSD, SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL))
	{
		return NULL;
	}

	//HANDLE hFile = CreateFile(TEXT("1234.txt"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, &sa, CREATE_NEW, NULL, NULL);

	mutex = CreateMutexA(&sa, FALSE, szMutexName);

	LocalFree(sa.lpSecurityDescriptor);

	return mutex;
}


HANDLE  CreateMySACLMutex(const char* szMutexName)
{
	HANDLE mutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, szMutexName);

	if (NULL != mutex)	
		return mutex;
	
	SECURITY_ATTRIBUTES  sa;
	memset(&sa, 0, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;

	ULONG  SecurityDescriptorSize = 0;

	bool bIsVistaOrLater = true;

	SECURITY_DESCRIPTOR secDesc;
	/*
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	bIsVistaOrLater = (osvi.dwMajorVersion > 5);
	*/

	if (bIsVistaOrLater)
	{
		if (FALSE == ConvertStringSecurityDescriptorToSecurityDescriptorA("S:(ML;;NW;;;LW)", // this means "low integrity"
			SDDL_REVISION_1, &(sa.lpSecurityDescriptor), &SecurityDescriptorSize))
			return NULL;

		mutex = CreateMutexA(&sa, FALSE, szMutexName);

		LocalFree(sa.lpSecurityDescriptor);
	}
	else
	{
		if (InitializeSecurityDescriptor(&secDesc, SECURITY_DESCRIPTOR_REVISION)) //Revision level
		{
			if (FALSE == SetSecurityDescriptorSacl(&secDesc, TRUE, NULL, FALSE))
				return NULL;

			sa.lpSecurityDescriptor = &secDesc;

			mutex = CreateMutexA(&sa, FALSE, szMutexName);
		}
	}
	
	return mutex;
}



HANDLE  Create_LowSD_Mutex(const char* szMutexName)
{
	HANDLE mutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, szMutexName);

	if (NULL != mutex)
		return mutex;

	SECURITY_ATTRIBUTES secAttr;
	SECURITY_DESCRIPTOR secDesc;
	PACL pSacl = NULL;                  // not allocated
	PSECURITY_DESCRIPTOR pSD = NULL;
	BOOL fSaclPresent = FALSE;
	BOOL fSaclDefaulted = FALSE;
	ULONG  SecurityDescriptorSize = 0;

	bool bIsVistaOrLater = false;

	/*
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	bIsVistaOrLater = (osvi.dwMajorVersion > 5);
	*/
	if (bIsVistaOrLater)
	{
		if (FALSE == ConvertStringSecurityDescriptorToSecurityDescriptorA("S:(ML;;NW;;;LW)", // this means "low integrity"
			SDDL_REVISION_1, &pSD, &SecurityDescriptorSize))
			return NULL;

		if (FALSE == GetSecurityDescriptorSacl(pSD, &fSaclPresent, &pSacl, &fSaclDefaulted))
			return NULL;
	}

	if (InitializeSecurityDescriptor(&secDesc, SECURITY_DESCRIPTOR_REVISION)) //Revision level
	{
		//	SetSecurityDescriptorDacl
		//	��dacl�Ͳ����ڵ�dacl֮����һ����Ҫ������
		//	1 .��daclΪ��ʱ�������������ʿ�����Ŀaces��ˣ�û����ȷ�������Ȩ�ޣ���ˣ��Զ���ķ��ʱ���ʽ�ܾ���
		//	2 .��һ������û��daclʱ(��pdacl����Ϊ��ʱ)��û�б�����������ö��󣬲������з������󶼱����衣
		//	Ϊ���ڶ���̼�ʵ�����Ȩ�޷��ʰ�ȫ����ֻ��Ѳ���������ΪNULL����ʵ�൱�ڽ�object�İ�ȫ���𽵵�����ͣ����еķ������󶼽��ɹ���
		//	(Ҫ��ʹ�õ�������������������ô��������Ҫ����ΪTRUE���������������ΪFALSE����ô���������ı�����)

		if (bIsVistaOrLater)
		{
			if (FALSE == SetSecurityDescriptorSacl(&secDesc, TRUE, pSacl, FALSE))
				return NULL;
		}
		else
		{
			if (FALSE == SetSecurityDescriptorSacl(&secDesc, TRUE, NULL, FALSE))
				return NULL;
		}

		memset(&secAttr, 0, sizeof(SECURITY_ATTRIBUTES));
		secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		secAttr.bInheritHandle = FALSE;
		secAttr.lpSecurityDescriptor = &secDesc;

		return CreateMutexA(&secAttr, FALSE, szMutexName);
	}

	return NULL;
}

int main()
{

	//CreateMutexA(NULL, FALSE, "Global\\TestGlobalMutex");

	DWORD dwE = 0;
	HANDLE hmutex;

	hmutex = CreateMyDACLMutex("Global\\TestGlobalMutex");
	printf("%d error = %d\n", hmutex, GetLastError());

	hmutex = CreateMyDACLMutex("Global\\TestGlobalMutex");
	printf("%d  error = %d\n", hmutex, GetLastError());

	//hmutex = CreateMySACLMutex("Global\\TestGlobalMutex");
	//printf("%d error = %d\n", hmutex, GetLastError());

	//hmutex = CreateMySACLMutex("Global\\TestGlobalMutex");
	//printf("%d error = %d\n", hmutex, GetLastError());
	
/*
	hmutex = Create_LowSD_Mutex("Global\\TestGlobalMutex");
	printf("%d error = %d\n", hmutex, GetLastError());

	hmutex = Create_LowSD_Mutex("Global\\TestGlobalMutex");
	printf("%d error = %d\n", hmutex, GetLastError());
*/	
	getchar();

	return 0;
}
