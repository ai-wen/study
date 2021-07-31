// DACL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

/*
Windows系统中可以同时登陆多个用户，各个用户在不同的session中。
在多个session上运行的程序可能会使用到同一份资源（文件，注册表等），此时对于资源的访问也会需要同步控制。
windows下的Mutex
1、命名空间
如果想要在多个session中使用同一个命名互斥体，应使用“Global\”前缀，实际情况在不同的session下使用CreateMutex返回ERROR_ACCESS_DENIED的错误。
2、在MSDN中，关于CreateMutex函数的返回ERROR_ACCESS_DENIED的解释中，建议使用OpenMutex函数来进行Mutex对象的打开操作，问题任然存在。
3、在MSDN中，使用全局命名空间的文件映射对象时，需要权限提升。但此处只是特指文件映射对象，不应该包含Mutex。
Mutex由管理员帐户登陆的session中创建，管理员帐户对该对象有控制权，之后在另一个session中也使用管理员权限打开mutex时，
使用了同一个管理员帐户，相当于同一个帐户运行的程序。也就是说对象的控制权与运行程序时使用的用户有关。
4、给认证用户，管理员用户组完全控制的DACL权限。
*/

/*
SDDL 语法
4种具体语法如下：
1.O:owner_sid							代表对象的SID字符串
2.G:Group_sid							一个SID字符串.标识对象的主要组
3.D:(ACE Strings1)(ACE Strings2)(...)	DACL信息.由ACE组成
4.S:(ACE Strings1)(ACE Strings2)(...)	 SACLxin

首字段类型：(O:) owner, (G:)primary group, (D:)DACL , (S:)SACL .最常见的也就是上面提到的D，设置文件的访问控制列表。
每条ACE Strings有7个字段
(ace_type; ace_flags;rights;object_guid;inherit_object_guid;	account_sid; (resource_attribute))
https://docs.microsoft.com/zh-tw/windows/win32/secauthz/ace-strings

任意权限控制列表（Discretionary access control list），用一个大写的D来表示。设置后用户列表的ACE选项允许修改。
系统权限控制列表（System access control list），用一个大写的S来表示。 设置后用户列表的ACE成灰色选项不允许修改。


DACL称为自主访问的控制列表。空DACL允许对所有用户进行完全访问，所以一般程序开发中安全属性都是传入NULL。
简单理解，DACL就是用户权限集合，ACE指的就是具体权限属性选项。

创建一个自己控制的文件
1.首先创建一个 安全属性结构体.(比如创建文件.文件中一般传入NULL.现在不传NULL)
2.使用SDDL字符串对其进行初始化.SDDL是一个NULL-Terminated结尾的 安全描述符字符串格式.(ConvertSecurityDescriptorToStringSecurityDescriptor / ConvertStringSecurityDescriptorToSecurityDescriptor)
3.使用 使用 ConvertStringSecurityDescriptorToSecurityDescriptor 函数将SDDL字符串转为安全描述符.(就是那个结构体)SECURITY_ATTRIBUTES的LpsecurityDscriptor成员.
4.使用过之后要使用 localFree函数释放 lpSecurityDescriptor 申请的内存.
要使用 Conver... 这种SDDL的函数.必须将 _WIN32_WINNT 常量定义为0x0500 或者更高.
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
		//TEXT("(A;OICI;GA;;;AN)") 拒绝所有权限
		TEXT("(A;OICI;GA;;;AN)")//匿名登录	ANONYMOUS LOGON
		TEXT("(A;OICI;GA;;;AU)")//认证登录用户  Authenticated users
		
		TEXT("(A;OICI;GA;;;BA)")//内置管理员 DESKTOP-GLTM519\Administrator ok
		TEXT("(A;OICI;GA;;;BG)")//内置游客 DESKTOP-GLTM519\Guest ok
		TEXT("(A;OICI;GA;;;BU)")//内置用户 DESKTOP-GLTM519\Users

		//TEXT("(A;OICI;GA;;;DA)")
		//TEXT("(A;OICI;GA;;;DC)")
		//TEXT("(A;OICI;GA;;;DD)")
		//TEXT("(A;OICI;GA;;;DG)")
		//TEXT("(A;OICI;GA;;;DU)")

		//TEXT("(A;OICI;GA;;;LA)")//本地管理员 DESKTOP-GLTM519\Administrator  fail
		//TEXT("(A;OICI;GA;;;LG)")//本地游客 DESKTOP-GLTM519\Guest  fail
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
		//	空dacl和不存在的dacl之间有一个重要的区别。
		//	1 .当dacl为空时，它不包含访问控制条目aces因此，没有明确授予访问权限，因此，对对象的访问被隐式拒绝。
		//	2 .当一个对象没有dacl时(当pdacl参数为空时)，没有保护被分配给该对象，并且所有访问请求都被授予。
		//	为了在多进程间实现最低权限访问安全对象，只需把参数三设置为NULL，其实相当于将object的安全级别降到了最低，所有的访问请求都将成功。
		//	(要想使用第三、四两个参数，那么参数二需要设置为TRUE；如果参数二设置为FALSE，那么参数三、四被忽略)

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
