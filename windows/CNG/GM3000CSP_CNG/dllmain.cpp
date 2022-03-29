// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>

//userenv.lib
//crypt32.lib
//ncrypt_provider.lib
//cng_provider.lib

#pragma comment(lib,"bcrypt_provider.lib")
/*
#pragma comment(lib, "apdu.core.lib")
#pragma comment(lib, "apdu.lib.lib")
#pragma comment(lib, "devmgr.windows.lib")
#pragma comment(lib, "gm_api.lib.lib")
#pragma comment(lib, "gm_data_mgr.lib")
#pragma comment(lib, "devmgr.windows.rpc.lib")
#pragma comment(lib, "mklib.lib")
#pragma comment(lib, "polarssl.lib")
#pragma comment(lib, "os.windows.lib")
#pragma comment(lib, "devmon.usb.lib")

#pragma comment(lib, "SetupApi.lib")
#pragma comment(lib, "hid.lib")
#pragma comment(lib, "Crypt32.lib")
*/
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

