#include <cstdio>
#include <unistd.h>
#include <stdio.h>
#include "libusb/libusb.h"
//#include "libusb.h"
#include <string.h>
int main()
{
	FILE* fp = fopen("usb.log", "w");

	//daemon(0, 0);

	libusb_context *g_ctx = NULL;
	libusb_device **devs = NULL, *dev = NULL;
	libusb_device_handle *handle = NULL;
	int i = 0, rv = 0,j=0;
	

	char bufmsg[100] = { 0 };
	//fwrite("11111111", 1, strlen("11111111"), fp);
	while (1)
	{
		i = 0;
		if(g_ctx)
			libusb_exit(g_ctx);

		libusb_init(&g_ctx);
		libusb_set_debug(g_ctx,3);
		rv = libusb_get_device_list(g_ctx, &devs);
		printf("%d\n", rv);
		/*
		while ((dev = devs[i++]) != NULL)
		{
			
			struct libusb_device_descriptor desc = {0};
			int r = libusb_get_device_descriptor(dev, &desc);
			
			if (r < 0)
			{
				continue;
			}
		   
			if (desc.idVendor == 0x55c)
			{
				
		
			rv = libusb_open(dev, &handle);
			if (rv < 0)
			{
				sprintf(bufmsg, "%d  [%d]libusb_open rv=%d\n", j++, i, rv);
				fwrite(bufmsg, 1, strlen(bufmsg), fp);
				//rv = libusb_open(dev, &handle);
			}

			if (i == 6)
			{
				sprintf(bufmsg, "%d  [%d]libusb_open rv=%d\n", j++, i, rv);
				fwrite(bufmsg, 1, strlen(bufmsg), fp);
			}
			

			if(rv == 0)
				libusb_close(handle);
				//sleep(1);
			//}
			 
		}
		*/
	}
	

	//fclose(fp);

	if (devs)
		libusb_free_device_list(devs, 1);
		
	while (1)
	{
		
	}
	//
    return 0;
}


