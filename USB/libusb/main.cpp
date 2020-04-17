#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>

int main1()
{
	int ret = -1;
	void * hDll = NULL;
	char *path = "/home/casic/projects/gm3000.1.0/bin/ARM64/Debug/libgm3000.1.0.so";
	int flag = RTLD_LAZY;
	hDll = dlopen(path, flag);
	if (hDll)
	{
		sleep(1);
		printf("dlclose begin.\n");
		dlsym(hDll, "SKF_VerifyPIN");
		ret = dlclose(hDll);
		printf("dlclose end, ret = %d.\n", ret);
	}
	else
		printf("dlopen FAILED.\n");


	unsigned char GetDevInfo[7] = { 0x80,0x04,0x00,0x00,   0x00,0x00,0x00 };

}



