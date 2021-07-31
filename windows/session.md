
#include <windows.h>

DWORD dwLoginSessionID = WTSGetActiveConsoleSessionId();//当前登录桌面用户的session

DWORD processId = GetCurrentProcessId();//当前进程id
DWORD dwProcessSessionId = -1;
if(ProcessIdToSessionId(processId,&dwProcessSessionId)) //获取当前进程session
{

    DBG_LOGMSG_EX("LoginSessionID=%d  %s  PID=%d  ProcessSessionId=%d",dwLoginSessionID,exeFileName,processId,dwProcessSessionId);
    if((0 != dwProcessSessionId)  &&                //lass.exe进程是0 session创建的
        (dwProcessSessionId != dwLoginSessionID))   //当前进程session与当前登录桌面用户的session不同，表示此进程在其它桌面运行
        return FALSE;
}
else
{
    DBG_LOGMSG_EX("ProcessIdToSessionId error=%d",GetLastError());
}