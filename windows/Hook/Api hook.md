# API hook的方法有很多种，比较常见的有：API inline hook、IAT hook等。
##### API inline hook
API inline hook的方法是修改API函数的前几个字节，让这几个字节执行无条件跳转指令，跳转到我们自己的函数里，这时候我们就可以根据参数进行一些判断，如果我们愿意放行，还可以恢复函数的前几字节并重新调用原函数，不愿意放行，就直接返回错误，Ring3级的进程保护就是这样实现的。
API inline hook在16位的系统上确实可以非常正常的工作，因为16位系统是单任务的！而如果是在32位的多任务抢占式操作系统上，系统可能会随时切换线程上下文，如果我们刚把函数前几个字节恢复了，系统突然把线程切换到了另一个线程，而那个线程又无巧不成书的调用了我们hook的这个函数，那么后果就是，出现了漏网之鱼！
##### IAThook 
而IAThook可以克服这个问题，但是IAThook的缺陷更是灾难性的！因为IAThook是通过修改导入地址表实现的，如果动态调用API，IAThook根本无法拦截。


使用API inline hook需要注意的地方：
1.hook的函数和你的函数必须用一样的形参表
2.hook的函数和你的函数必须用一样的调用约定（我上回就是没有想到这个，导致hook的函数一调用就崩溃）
原因很简单，如果不着么做，后果就是函数调用前后堆栈不平衡，引发程序异常。
3.API inline hook需要在目标进程的地址空间内进行，需要DLL注入，关于注入请看：
```cpp
DLL注入是把指定的DLL加载到另一个进程的内存空间中去的技术。
DLL注入技术有很多种，我知道的有：
1。通过远程线程注入（CreateRemoteThread）
2。通过全局消息钩子注入（SetWindowsHookEx）
3。通过注册表注入
4。通过输入法注入
5。驱动注入
6。在启动进程时挂起直接挂起主线程，写入指令后再恢复线程（难度较大）
7。DLL劫持（严格意义上这个不是注入方法）
8。修改PE文件导入表
还有百度到的：
9。使用NtMapViewOfSection注入
10。使用SetThreadContext注入

这里主要说一下第一种方法(注意最好加上Debug权限)
1。用OpenProcess打开要注入进程的句柄。
2。用VirtualAllocEx在远程进程中申请一段内存，长度为DLL路径长度+1（多出来的一字节用于存放\0）。
3。用WriteProcessMemory将Dll的路径远程写入申请的内存中。
4。用CreateRemoteThread将LoadLibraryA作为线程启动函数，参数为DLL的路径，远程创建线程。
5。用CloseHandle关闭线程句柄。
6。会调用DllMain函数，在这个函数里完成你要做的事，比如偷窥密码edit的内容，或者Hook API了等等
BOOL InjectDll(DWORD dwProcessID, char* dllPath)//参数：目标进程ID、DLL路径
{
	FARPROC FuncAddr = NULL;
	HMODULE hdll = LoadLibrary(TEXT("Kernel32.dll"));//加载DLL
	if (hdll != NULL){
		FuncAddr = GetProcAddress(hdll, "LoadLibraryA");//获取LoadLibraryA函数地址
		if (FuncAddr == NULL)return FALSE;
	}
 
	HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION, FALSE, dwProcessID);//获取进程句柄
	if (hProcess == NULL)return FALSE;
	DWORD dwSize = strlen(dllPath) + 1;
	LPVOID RemoteBuf = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);//远程申请内存
	DWORD dwRealSize;
	if (WriteProcessMemory(hProcess, RemoteBuf, dllPath, dwSize, &dwRealSize))//远程写内存
	{
		DWORD dwThreadId;
		HANDLE hRemoteThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)FuncAddr, RemoteBuf, 0, &dwThreadId);//创建远程线程
		if (hRemoteThread == NULL)
		{
			VirtualFreeEx(hProcess, RemoteBuf, dwSize, MEM_COMMIT);
			CloseHandle(hProcess);
			return FALSE;
		}
		//释放资源
		WaitForSingleObject(hRemoteThread, INFINITE);
		CloseHandle(hRemoteThread);
		VirtualFreeEx(hProcess, RemoteBuf, dwSize, MEM_COMMIT);
		CloseHandle(hProcess);
		return TRUE;
	}
	else
	{
		VirtualFreeEx(hProcess, RemoteBuf, dwSize, MEM_COMMIT);
		CloseHandle(hProcess);
		return FALSE;
	}
}

管理员权限完全可以关闭系统中运行的任何用户模式下的进程，但是为何无法关闭呢，是因为Windows有一项机制叫“强制完整性控制”，IE保护模式处于低完整性，标准用户进程和管理员以标准用户权限运行时处于中完整性，管理员权限进程有高完整性，SYSTEM权限进程是系统完整性，强制完整性控制要优先于访问控制和安全描述符，所以管理员权限的程序依然不能直接访问服务程序的进程，只有获取了Debug权限，才可以以高完整性访问系统完整性的进程。

可能有人没有听说过强制完整性控制，但想必知道UIPI（用户界面特权隔离）吧，其实UIPI不是根据用户权限来隔离Windows消息的！而是通过强制完整性控制！哈哈，其实以前我也不知道，而且有些资料对于这个UIPI也是知其然但不知其所以然，说的很含糊，误导了很多人。
如何获取Debug特权呢？
首先用OpenProcessToken获取自身的权限令牌，之后用LookupPrivilegeValue查看系统权限的特权值，再用AdjustTokenPrivileges启用debug权限。

BOOL EnableDebugPrivilege()
{
	HANDLE hToken;
	LUID Luid;
	TOKEN_PRIVILEGES tp;
 
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))return FALSE;
 
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Luid))
	{
		CloseHandle(hToken);
		return FALSE;
	}
 
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = Luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
 
	if (!AdjustTokenPrivileges(hToken, false, &tp, sizeof(tp), NULL, NULL))
	{
		CloseHandle(hToken);
		return FALSE;
	}
	CloseHandle(hToken);
	return TRUE;
}

```


```cpp inline hook
#include <stdio.h>
#include <windows.h>
 
unsigned char code[5];
unsigned char oldcode[5];
FARPROC addr;
DWORD pid;
 
int getpid()
{
	char buffer[255];
	DWORD get = 255;
	//判断环境是否为WOW64
	BOOL isWOW64;
	REGSAM p = KEY_READ;
	IsWow64Process(GetCurrentProcess(), &isWOW64);
	if (isWOW64)p |= KEY_WOW64_64KEY;
 
	HKEY hKey;
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\测试"), 0, NULL, 0, p, NULL, &hKey, NULL) != ERROR_SUCCESS){
		return 0;
	}
	if (RegQueryValueExA(hKey, "Main_PID", 0, NULL, (BYTE*)buffer, &get) != ERROR_SUCCESS){
		return 0;
	}
	return atoi(buffer);
}


HANDLE WINAPI MyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId){
	HANDLE handle;
	if (getpid() == dwProcessId){
		SetLastError(5);
		return NULL;
	}
 
#ifdef _WIN64 
    DWORD old;
	if (VirtualProtectEx(GetCurrentProcess(), addr, 12, PAGE_EXECUTE_READWRITE, &old)){
		WriteProcessMemory(GetCurrentProcess(), addr, oldcode, 12, NULL);
		VirtualProtectEx(GetCurrentProcess(), addr, 12, old, &old);
	}
	handle = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	if (VirtualProtectEx(GetCurrentProcess(), addr, 12, PAGE_EXECUTE_READWRITE, &old)){
		WriteProcessMemory(GetCurrentProcess(), addr, code, 12, NULL);
		VirtualProtectEx(GetCurrentProcess(), addr, 12, old, &old);
	}

#else
	DWORD old;
	if (VirtualProtectEx(GetCurrentProcess(), addr, 5, PAGE_EXECUTE_READWRITE, &old)){
		WriteProcessMemory(GetCurrentProcess(), addr, oldcode, 5, NULL);
		VirtualProtectEx(GetCurrentProcess(), addr, 5, old, &old);
	}
	handle = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	if (VirtualProtectEx(GetCurrentProcess(), addr, 5, PAGE_EXECUTE_READWRITE, &old)){
		WriteProcessMemory(GetCurrentProcess(), addr, code, 5, NULL);
		VirtualProtectEx(GetCurrentProcess(), addr, 5, old, &old);
	}
 
#endif    
	return handle;
}
 
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		addr = 0;
		HMODULE hdll; hdll = LoadLibrary(TEXT("Kernel32.dll"));
		addr = GetProcAddress(hdll, "OpenProcess");
/*
DLL_PROCESS_ATTACH里，dll加载进目标程序，用GetProcAddress函数得到OpenProcess函数在内存中的位置，code[0]里存储一个jmp指令，jmp指令是根据位移无条件跳转指令，具体可查询win32汇编的资料。
随后计算我们的函数和OpenProcess之间的位移，并存储在code[1-4]里
VirtualProtectEx修改虚拟内存页面保护，如果不修改会引发虚拟内存访问违规。
之后，RtlMoveMemory(oldcode, addr, 5);将原函数的前五个字节保存在oldcode里面，WriteProcessMemory修改前五个字节
*/
        if (addr)
        {
            //此代码只对只对32位有效
			code[0] = 0xe9;
			DWORD a = (DWORD)MyOpenProcess - (DWORD)addr - 5;
			RtlMoveMemory(code + 1, &a, 4);
 
			DWORD old;
			if (VirtualProtectEx(GetCurrentProcess(), addr, 5, PAGE_EXECUTE_READWRITE, &old))
            {
				RtlMoveMemory(oldcode, addr, 5);
				WriteProcessMemory(GetCurrentProcess(), addr, code, 5, NULL);
				VirtualProtectEx(GetCurrentProcess(), addr, 5, old, &old);
			}
		}
/*
64位和32位原理一样，就是换了个指令，jmp只能跳转32位大小，如果超过了就没法跳转了，所以更换指令
mov rax,地址
push rax
ret        
利用了一个ret，因为已经把返回地址压入栈了，因此直接返回到了我们自己的函数里了，因为用的ret，不是ret(at)num，因此入栈的实参并没有清除，而原来的返回地址已经在call的时候就入栈了，因此也会保留，完美地实现了跳转。
*/
        if (addr)
        {
			code[0] = 0x48;
			code[1] = 0xB8;
			code[10] = 0x50;
			code[11] = 0xC3;
			long long a = (long long)MyOpenProcess;
			RtlMoveMemory(code + 2, &a, 8);
 
			DWORD old;
			if (VirtualProtectEx(GetCurrentProcess(), addr, 12, PAGE_EXECUTE_READWRITE, &old))
            {
				RtlMoveMemory(oldcode, addr, 12);
				WriteProcessMemory(GetCurrentProcess(), addr, code, 12, NULL);
				VirtualProtectEx(GetCurrentProcess(), addr, 12, old, &old);
			}
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
```


``` code
unsigned char code[12];
unsigned char oldcode[12];
inline hook 核心
 //进行 Inline Hook
        HMODULE hdll = LoadLibrary(TEXT("kernel32.dll"));
        addr = GetProcAddress(hdll, "OpenProcess");
        if (addr){
#ifdef _M_IX86
            code[0] = 0xe9;
            aint a = (aint)MyOpenProcess - (aint)addr - 5;
            RtlMoveMemory(code + 1, &a, 4);

            DWORD old;
            if (VirtualProtectEx(GetCurrentProcess(), addr, 5, PAGE_EXECUTE_READWRITE, &old)){
                RtlMoveMemory(oldcode, addr, 5);
                WriteProcessMemory(GetCurrentProcess(), addr, code, 5, NULL);
                VirtualProtectEx(GetCurrentProcess(), addr, 5, old, &old);
            }
#elif _M_X64
            code[0] = 0x48;
            code[1] = 0xB8;
            code[10] = 0x50;
            code[11] = 0xC3;
            aint a = (aint)MyOpenProcess;
            RtlMoveMemory(code + 2, &a, 8);

            DWORD old;
            if (VirtualProtectEx(GetCurrentProcess(), addr, 12, PAGE_EXECUTE_READWRITE, &old)){
                RtlMoveMemory(oldcode, addr, 12);
                WriteProcessMemory(GetCurrentProcess(), addr, code, 12, NULL);
                VirtualProtectEx(GetCurrentProcess(), addr, 12, old, &old);
            }
#endif
        }

```



# ![Ring3 下 API Inline Hook 优化方案探索与实现](https://blog.csdn.net/zuishikonghuan/article/details/51302024)
以前写过两篇“[Win32] API Hook（1）在32/64位系统上的实现”博客，介绍并给出了 API inline hook 代码，如下： 
blog.csdn.net/zuishikonghuan/article/details/47976067 
blog.csdn.net/zuishikonghuan/article/details/47979603 
在这两篇博文中，我简要说了API inline hook的缺陷：

API inline hook的方法是修改API函数的前几个字节，让这几个字节执行无条件跳转指令，跳转到我们自己的函数里，这时候我们就可以根据参数进行一些判断，如果我们愿意放行，还可以恢复函数的前几字节并重新调用原函数，不愿意放行，就直接返回错误，Ring3级的进程保护就是这样实现的。 
API inline hook在16位的系统上确实可以非常正常的工作，因为16位系统是单任务的！而如果是在32位的多任务抢占式操作系统上，系统可能会随时切换线程上下文，如果我们刚把函数前几个字节恢复了，系统突然把线程切换到了另一个线程，而那个线程又无巧不成书的调用了我们hook的这个函数，那么后果就是，出现了漏网之鱼！ 
而IAThook可以克服这个问题，但是IAThook的缺陷更是灾难性的！因为IAThook是通过修改导入地址表实现的，如果动态调用API，IAThook根本无法拦截。

当时我说这是一个小瑕疵，是的，在一些老系统（例如XP）上的确可以算是比较好的工作，但是现在看来，这个问题已经非常严重了，在Win8、8.1、10系统上，如果在explorer里Hook，会造成explorer特别容易崩溃！！（特别是 Hook NT Native API）

我希望找到新的解决方案，IAT HOOK 首先被否决了，因为 IAT HOOK 拦截不住 API 动态调用，在 Ring 0 中 Hook 更是行不通，因为在 amd64 平台上是有驱动程序强制签名的，买签名是很贵的，而且还不能干坏事。

那么我们就要认命吗？
于是我试图从 Inline Hook 的实现方法上找原因，这是我写的 Inline Hook 代码（x86 和 amd 64）

没错，原理就是修改要 Hook 函数的前几个字节，问题关键就在于，如何能避免还原时恢复函数的这几个字节呢？

        //进行 Inline Hook
        HMODULE hdll = LoadLibrary(TEXT("kernel32.dll"));
        addr = GetProcAddress(hdll, "OpenProcess");
        if (addr){
#ifdef _M_IX86
            code[0] = 0xe9;
            aint a = (aint)MyOpenProcess - (aint)addr - 5;
            RtlMoveMemory(code + 1, &a, 4);

            DWORD old;
            if (VirtualProtectEx(GetCurrentProcess(), addr, 5, PAGE_EXECUTE_READWRITE, &old)){
                RtlMoveMemory(oldcode, addr, 5);
                WriteProcessMemory(GetCurrentProcess(), addr, code, 5, NULL);
                VirtualProtectEx(GetCurrentProcess(), addr, 5, old, &old);
            }
#elif _M_X64
            code[0] = 0x48;
            code[1] = 0xB8;
            code[10] = 0x50;
            code[11] = 0xC3;
            aint a = (aint)MyOpenProcess;
            RtlMoveMemory(code + 2, &a, 8);

            DWORD old;
            if (VirtualProtectEx(GetCurrentProcess(), addr, 12, PAGE_EXECUTE_READWRITE, &old)){
                RtlMoveMemory(oldcode, addr, 12);
                WriteProcessMemory(GetCurrentProcess(), addr, code, 12, NULL);
                VirtualProtectEx(GetCurrentProcess(), addr, 12, old, &old);
            }
#endif
        }

Google、Bing 后无果，于是只好自己想办法，我想，我们的目的是要对目标 API 进行过滤，对于同意放行的，我们可以再调用原函数，而程序载入内存后就是数据，这些 API 自然也不例外，而且这些 API 都是在 DLL 中，这些 DLL 需要映射（载入）到进程的线性地址空间（虚拟内存）中，那么，我们只需要将整个模块复制一遍，然后把不需要过滤的调用统统跳转到复制的区域不就好了。

第一次探索：将函数所在模块复制一份
#ifdef _M_IX86
#define aint DWORD
#elif _M_X64
#define aint unsigned long long
#endif
        //模块基地址
        HMODULE BaseAddr = GetModuleHandle(TEXT("kernel32.dll"));

        //计算模块在内存中的大小
        MEMORY_BASIC_INFORMATION MemoryBasicInfomation;
        Size = 0;
        VirtualQueryEx(GetCurrentProcess(), BaseAddr, &MemoryBasicInfomation, sizeof(MEMORY_BASIC_INFORMATION));
        PVOID BaseAddress = MemoryBasicInfomation.AllocationBase;

        while (MemoryBasicInfomation.AllocationBase == BaseAddress)
        {
            Size += MemoryBasicInfomation.RegionSize;
            VirtualQueryEx(GetCurrentProcess(), BaseAddr + Size, &MemoryBasicInfomation, sizeof(MEMORY_BASIC_INFORMATION));
        }

        //分配内存并复制
        Buf = VirtualAlloc(NULL, Size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        RtlMoveMemory(Buf, BaseAddr, Size);

        //进行 Inline Hook
        HMODULE hdll = LoadLibrary(TEXT("kernel32.dll"));
        addr = GetProcAddress(hdll, "OpenProcess");
        if (addr){
#ifdef _M_IX86
            code[0] = 0xe9;
            aint a = (aint)MyOpenProcess - (aint)addr - 5;
            RtlMoveMemory(code + 1, &a, 4);

            DWORD old;
            if (VirtualProtectEx(GetCurrentProcess(), addr, 5, PAGE_EXECUTE_READWRITE, &old)){
                RtlMoveMemory(oldcode, addr, 5);
                WriteProcessMemory(GetCurrentProcess(), addr, code, 5, NULL);
                VirtualProtectEx(GetCurrentProcess(), addr, 5, old, &old);
            }
#elif _M_X64
            code[0] = 0x48;
            code[1] = 0xB8;
            code[10] = 0x50;
            code[11] = 0xC3;
            aint a = (aint)MyOpenProcess;
            RtlMoveMemory(code + 2, &a, 8);

            DWORD old;
            if (VirtualProtectEx(GetCurrentProcess(), addr, 12, PAGE_EXECUTE_READWRITE, &old)){
                RtlMoveMemory(oldcode, addr, 12);
                WriteProcessMemory(GetCurrentProcess(), addr, code, 12, NULL);
                VirtualProtectEx(GetCurrentProcess(), addr, 12, old, &old);
            }
#endif
        }

        //记录偏移量
        offset = (aint)addr - (aint)BaseAddr;
        DWORD old2;
        VirtualProtectEx(GetCurrentProcess(), Buf, Size, PAGE_EXECUTE_READWRITE, &old2);

那么，在我们的函数中：

#ifdef _M_IX86
#define aint DWORD
#elif _M_X64
#define aint unsigned long long
#endif
HANDLE WINAPI MyOpenProcess(
  _In_ DWORD dwDesiredAccess,
  _In_ BOOL  bInheritHandle,
  _In_ DWORD dwProcessId
){

    HANDLE handle = 0;
    //保护我们的进程，发现被 Hook 的进程试图访问我们的进程时返回“拒绝访问”
    if (getpid() == dwProcessId){
        SetLastError(5);
        return NULL;
    }

    //如果不是访问我们的进程则去 Call 复制的 kernel32
    typedef HANDLE(WINAPI * OpenProcess_t)(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
    OpenProcess_t RealOpenProcess = (OpenProcess_t)((aint)Buf + offset);
    handle = RealOpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
    return handle;
}

而之前呢，手动恢复原函数，再去 Call 原函数，在多任务的环境下的确很不好。

DWORD old;
    if (VirtualProtectEx(GetCurrentProcess(), addr, 5, PAGE_EXECUTE_READWRITE, &old)){
        WriteProcessMemory(GetCurrentProcess(), addr, oldcode, 5, NULL);
        VirtualProtectEx(GetCurrentProcess(), addr, 5, old, &old);
    }
    handle = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
    if (VirtualProtectEx(GetCurrentProcess(), addr, 5, PAGE_EXECUTE_READWRITE, &old)){
        WriteProcessMemory(GetCurrentProcess(), addr, code, 5, NULL);
        VirtualProtectEx(GetCurrentProcess(), addr, 5, old, &old);
    }

这一步，我们需要注意的是： 
1.要使我们复制的 kernel32 （或其他模块）具有可执行权限，比如PAGE_EXECUTE_READWRITE

2.我们应该从内存中复制，而不是从磁盘上把 DLL 读进内存！同时，我们必须计算模块在内存中的大小，而不是去计算在磁盘中 DLL 文件的大小！（原因是：PE文件在磁盘上是连续存储的，但载入内存后每个节是按页面对齐的）（计算大小可以用 VirtualQueryEx 、ToolHelp API 或 读取PE头部（在PE文件头的0x50偏移处存放），这里我使用是 VirtualQueryEx ，如果用第三种那种方法可移植性就不好了，万一PE结构变了呢，而且32位和64位的PE结构应该不一样呢）

现在，我们已经实现了我们的目的！事实上他已经非常健壮且稳定的工作了！经过测试，注入 explorer 长时间后已经不会造成崩溃，也不会造成性能明显下降，用户已经感觉不出什么不同了。

但是，还能更好一点吗？

其实，这一套机制有一个非常不友好的行为，那就是 Buf = VirtualAlloc(NULL, Size, MEM_COMMIT, PAGE_EXECUTE_READWRITE); 他会向操作系统申请内存，用于存放复制的 kernel32.dll ，而在 Ring 3 下 Hook ，需要注入到每一个进程，那么我们在每一个进程中都执行一遍复制，那么会浪费太多的内存，而且这种浪费是完全没有必要的。

第二次探索：使用共享内存
其实，如果只是为了 Hook 一个进程，上面的那一套已经非常成熟了。换句话说，上面的一套机制保证了稳定性，但是如果要 Hook 多个进程或全局 Hook ，则会造成一定的资源浪费，这是不能容忍的。

于是，我首先想到了共享内存，既然每个进程都复制一份太浪费了，那么我就创建一个 Daemon 进程，专门负责这件事，即创建映射、复制模块，然后其他进程直接把这块数据映射到自己的线性地址空间（虚拟内存）中就好了，就不会出现重复复制浪费内存的问题了。但是经过不断测试研究才发现，这样根本行不通。

原因有两个： 
1. 你怎么保证每个进程中的 kernel32 （或其他模块）中的这个函数都一样？ 
2. 你怎么保证每个进程都能将 Daemon 创建的共享内存映射到自己的线性地址空间？

这两个问题，咋一听有点无稽之谈，难道还会不一样？其实，如果此时有其他程序（比如杀软等软件） Hook 了这个函数，那么他也需要把自己 DLL 注入到每个进程中，问题来了，他的 DLL 能载入到他预期的位置吗？答案是不确定的，那么重定位是很有可能发生的，那么也就意味着他修改后的代码是不一样的！那么如果我们再次 Hook 后，创建的共享内存里肯定是 Daemon 的代码吧，里面的指令是跳转到上一个 Hook 函数里，这时，如果其他进程将这块共享内存映射到自己的线性地址空间中，这个跳转的地址还会是上一个 Hook 的地址吗？此时我们的 Hook 跳转到原函数时，就会跳转到一个非法的地址，结果是什么很难预料，不过崩溃的可能性是很大的，不是吗？

你可以说“那我们可以再把每个进程中的这几个字节复制进去吗，几个字节而已了”但是 Inline Hook 的方法千奇百怪，不一定都是头部的字节，字节大小也不一定一样，而且还有在函数内部 Inline Hook 的，所有这样弄成本太高。 
第二个问题就是，其他程序不一定能映射我们创建的共享内存。第一个是权限的问题，第二个是完整性的问题，权限还好解决，我们的 Daemon 可以以低权限运行，并且还可以创建共享内存时创建一个允许任何人访问的“ACL”（访问控制列表）（虽然很麻烦），但是还有完整性呢，比如 IE 就是第完整性的进程，进程还可以降低完整性创建进程，低完整性的程序不能访问更高完整性的进程，额。。

话说回来，这两点有点吹毛求疵了，不过，我们要把用户体验做到极致，不是吗？

第三次探索：那么自己再次载入一份要 Hook 的函数所在的模块呢
没错，我们自己模拟系统再次载入一个 kernel32 （或其他模块），不就完事大吉了吗。CreateFileMapping 一定要是SEC_IMAGE | PAGE_READONLY，即载入的是一个可执行文件映像，不然操作系统不会帮你把每个节是按页面对齐的。

原因是操作系统映射 DLL 到进程内存时，其实是共享的，他只是载入一份，然后每个进程要求载入时直接映射到目标进程的线性地址空间（虚拟内存）中，如果程序修改了这部分虚拟内存，会触发“写时复制”机制，使修改局限于一个进程，因此我们也这样做就OK了。

这是最终的代码，既解决了 Inline Hook 的稳定性问题、可能出现漏网之鱼的问题，也解决了上文中第一个解决方案的不必要资源浪费的问题。对了，这个代码兼容 x86 和 amd64 哦！

#ifdef _M_IX86
#define aint DWORD
#elif _M_X64
#define aint unsigned long long
#endif
    case DLL_PROCESS_ATTACH:{

        TCHAR WinDir[MAX_PATH];
        GetWindowsDirectory(WinDir, MAX_PATH);
        std::wstring dir = WinDir;
        if ((wchar_t*)dir.at(dir.size() - 1) != L"\\") dir += L"\\";

        std::wstring dll = dir + L"system32\\kernel32.dll";
        hFile = CreateFile(dll.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE){
            //如果进程调用了 Wow64DisableWow64FsRedirection 那么必定无法加载 system32\\kernel32.dll
            dll = dir + L"SysWOW64\\kernel32.dll";
            HANDLE hFile = CreateFile(dll.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE){
                goto cleanup;
            }
        }

        dir.clear();
        dll.clear();

        hMapFile = 0;
        Buf = 0;
        if ((hMapFile = CreateFileMapping(hFile, NULL, SEC_IMAGE | PAGE_READONLY, 0, 0, NULL)) != NULL){
            if ((Buf = MapViewOfFile(hMapFile, FILE_MAP_COPY, 0, 0, 0)) == 0)
                goto cleanup;
        }
        else goto cleanup;

        //进行 Inline Hook
        addr = 0;
        HMODULE hdll = LoadLibrary(TEXT("kernel32.dll"));
        addr = GetProcAddress(hdll, "OpenProcess");
        if (addr){
#ifdef _M_IX86
            code[0] = 0xe9;
            aint a = (aint)MyOpenProcess - (aint)addr - 5;
            RtlMoveMemory(code + 1, &a, 4);

            DWORD old;
            if (VirtualProtectEx(GetCurrentProcess(), addr, 5, PAGE_EXECUTE_READWRITE, &old)){
                RtlMoveMemory(oldcode, addr, 5);
                WriteProcessMemory(GetCurrentProcess(), addr, code, 5, NULL);
                VirtualProtectEx(GetCurrentProcess(), addr, 5, old, &old);
            }
            else goto cleanup;
#elif _M_X64
            code[0] = 0x48;
            code[1] = 0xB8;
            code[10] = 0x50;
            code[11] = 0xC3;
            aint a = (aint)MyOpenProcess;
            RtlMoveMemory(code + 2, &a, 8);

            DWORD old;
            if (VirtualProtectEx(GetCurrentProcess(), addr, 12, PAGE_EXECUTE_READWRITE, &old)){
                RtlMoveMemory(oldcode, addr, 12);
                WriteProcessMemory(GetCurrentProcess(), addr, code, 12, NULL);
                VirtualProtectEx(GetCurrentProcess(), addr, 12, old, &old);
            }
            else goto cleanup;
#endif
        }

        HMODULE BaseAddr = GetModuleHandle(TEXT("kernel32.dll"));
        //记录偏移量
        offset = (aint)addr - (aint)BaseAddr;
        DWORD old2;
        VirtualProtectEx(GetCurrentProcess(), Buf, Size, PAGE_EXECUTE_READWRITE, &old2);
        return TRUE;

    cleanup:
        if (Buf)
            UnmapViewOfFile(Buf);
        if (hMapFile)
            CloseHandle(hMapFile);
        if (hFile)
            CloseHandle(hFile);
        return FALSE;
        break;
    }

会不会带来什么影响呢？
从 Process Explorer 中会看到被 Hook 进程加载了两个 Kernel32.dll （或其他模块）如图：



其实请大家放心，我们只是将 kernel32 （或其他模块）以可执行映像的形式载入到进程的线性地址空间（虚拟内存）中，实际上并没有完成 DLL 的载入过程，也就是说，不会影响 GetModuleHandle 和 LoadLibrary 的执行结果，更不会去调用 DLL 的 DllEntry（废话我们只是映射进来又没有调用），并且这一点我已经经过了测试。

继续拓宽思维
嘿嘿，自己改进了 Ring3 下的 Inline Hook ，克服了持续多年的 Inline Hook 的问题，心情简直 exciting 。

然后继续想了想，发现这里面还有文章可做呢！

用这种思路，可以 Anti Ring3 Hook !我们可以自己载入一份模块，然后找到 API 的位置，直接调用，即可绕过 Ring 3 下的钩子，其实这种方法并不新鲜了，但网上的说法大多是复制一份 DLL 再 Load，其实没有必要，按照上文的方法重新映射一遍就好了。

但是，我想说的是，Hook 者要如何防范这个问题呢？最好不要动 CreateFileMapping 、ZwCreateSection，因为这可能造成程序工作不正常，或者造成你 Hook 了让另一个软件的这种 Hook 无法工作，因此，我们可以 Hook MapViewOfFile 或更低的接口，先自己映射一遍，然后对新映射的这一部分代码再进行 Hook （即修改新映射的内存中 API 的前几个字节），然后再把地址告诉应用程序，嘿嘿，进程根本不知道得到的新映射的模块映像已经是动过手脚的了。

这篇博文到这里就结束了，记录了我的这次Ring3 下 Inline Hook 优化方案探索过程，结果也很完美
