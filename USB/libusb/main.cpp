#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "libusb.h"

static void hid_test();

int main2()
{
	hid_test();

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





/*
// HID Class-Specific Requests values. See section 7.2 of the HID specifications
#define HID_GET_REPORT                0x01
#define HID_GET_IDLE                  0x02
#define HID_GET_PROTOCOL              0x03
#define HID_SET_REPORT                0x09
#define HID_SET_IDLE                  0x0A
#define HID_SET_PROTOCOL              0x0B
#define HID_REPORT_TYPE_INPUT         0x01
#define HID_REPORT_TYPE_OUTPUT        0x02
#define HID_REPORT_TYPE_FEATURE       0x03
void hid_control_cmd()
{

int API_EXPORTED libusb_control_transfer(libusb_device_handle *dev_handle,
	uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
	unsigned char *data, uint16_t wLength, unsigned int timeout);

uint8_t hid_report_descriptor[256];
int  r, size;
uint8_t *report_buffer;
//1、GET_DESCRIPTOR
libusb_control_transfer(libusb_device_handle, 
	LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_STANDARD | LIBUSB_RECIPIENT_INTERFACE,LIBUSB_REQUEST_GET_DESCRIPTOR, LIBUSB_DT_REPORT << 8, 0, 
	hid_report_descriptor, sizeof(hid_report_descriptor), 1000);

//2、GET_REPORT_FEATURE
//从hid_report_descriptor中获取REPORT_FEATURE的size  report_buffer=new char[size];
r = libusb_control_transfer(libusb_device_handle,
	LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, HID_GET_REPORT, (HID_REPORT_TYPE_FEATURE << 8) | 0x00, 0,
	report_buffer, (uint16_t)size, 5000);
if(LIBUSB_ERROR_NOT_FOUND)		{printf("   No Feature Report available for this device\n");}
else if(LIBUSB_ERROR_PIPE == r)	{ printf("   Detected stall - resetting pipe...\n"); libusb_clear_halt(libusb_device_handle, 0); }
else							{ printf("   Error: %s\n", libusb_strerror((enum libusb_error)r)); }

//3、GET_REPORT_INPUT
//从hid_report_descriptor中获取REPORT_INPUT的size  report_buffer=new char[size];
r = libusb_control_transfer(libusb_device_handle, 
	LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, HID_GET_REPORT, (HID_REPORT_TYPE_INPUT << 8) | 0x00, 0, 
	report_buffer, (uint16_t)size, 5000);
if (LIBUSB_ERROR_TIMEOUT)			{ printf("   Timeout! Please make sure you act on the device within the 5 seconds allocated...\n"); }
else if (LIBUSB_ERROR_PIPE == r)	{ printf("   Detected stall - resetting pipe...\n"); libusb_clear_halt(libusb_device_handle, 0); }
else								{ printf("   Error: %s\n", libusb_strerror((enum libusb_error)r)); }

//4、SET_REPORT_OUTPUT
r = libusb_control_transfer(libusb_device_handle, 
	LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, HID_SET_REPORT,( HID_REPORT_TYPE_OUTPUT << 8) | 0x00 , 0,
	report_buffer, (uint16_t)size, 5000);

}

void hid_interrupt_cmd()
{
	int LIBUSB_CALL libusb_interrupt_transfer(libusb_device_handle *dev_handle,
		unsigned char endpoint_in/out, unsigned char *data, int length,
		int *actual_length, unsigned int timeout);
}
*/


//libusb_endpoint_descriptor
//->bEndpointAddress 
//Bits 0:3 arethe endpoint number.		0x0000 0001
//Bits 4:6 are reserved.				0x0 000 0001 = 0x1
//Bit 7 indicates direction				0x1 000 0001 = 0x81
//->bmAttributes
//Bits 0:1 determine the transfer type and correspond to ref libusb_transfer_type.			0x0000 0011 = 0x3
//Bits 2:3 are only used for isochronous endpoints and correspond to ref libusb_iso_sync_type.
//Bits 4:5 are also only used for isochronous endpoints and correspond to ref libusb_iso_usage_type.
//Bits 6:7 are reserved.
//libusb_transfer_type有以下类型
//LIBUSB_TRANSFER_TYPE_CONTROL = 0,// Control endpoint
//LIBUSB_TRANSFER_TYPE_ISOCHRONOUS = 1,// Isochronous endpoint 
//LIBUSB_TRANSFER_TYPE_BULK = 2,// Bulk endpoint 
//LIBUSB_TRANSFER_TYPE_INTERRUPT = 3,//Interrupt endpoint 
//LIBUSB_TRANSFER_TYPE_BULK_STREAM = 4,//Stream endpoint


//libusb_set_auto_detach_kernel_driver(libusb_device_handle, 1);
typedef struct
{
	uint8_t interfaceClass;	 //LIBUSB_CLASS_SMART_CARD  LIBUSB_CLASS_HID  LIBUSB_CLASS_MASS_STORAGE
	uint8_t	interfaceIndex;  //libusb_claim_interface(libusb_device_handle, interfaceIndex);
							 //libusb_release_interface(libusb_device_handle, interfaceIndex);
	uint8_t	endpoint_in_type[2];	//主机收包端点[0]端点号[1]通信方式INTERRUPT/BULK
	uint8_t endpoint_out_type[2];	//主机发包端点[0]端点号[1]通信方式INTERRUPT/BULK
}lm_usbdev;


/*
typedef struct
{
	uint8_t interfaceClass;
	uint8_t	interfaceIndex; 
	uint8_t	endpoint_in_type[2];	//主机收包端点[0]端点号[1]通信方式INTERRUPT/BULK
	uint8_t endpoint_out_type[2];	//主机发包端点[0]端点号[1]通信方式INTERRUPT/BULK
	uint8_t	transfer_type[2]
}lm_usbdev ;
*/

#include <vector>
#include <map>

typedef struct
{
	libusb_device *pdev;
	//std::map <uint8_t, > maphidkey;
}xxxx;




std::vector<lm_usbdev> g_vcUsbDevs;

static void hid_test()
{
	int r,devNum;
	libusb_context *g_ctx = NULL;
	libusb_device **g_devs = NULL;
	libusb_device_handle *usb_handle = NULL;

	r = libusb_init(&g_ctx);

	g_vcUsbDevs.clear();
	
	/*
	//单个设备
	usb_handle = libusb_open_device_with_vid_pid(g_ctx, VID, PID);//CCIDHIDPID
	if (0 == usb_handle)
		return;

	g_ctx[0] = libusb_get_device(usb_handle);
	*/



	devNum = libusb_get_device_list(g_ctx, &g_devs);
	for (int ii = 0; ii < devNum; ii++)
	{
		libusb_device *pdev = g_devs[ii];

		struct libusb_device_descriptor dev_desc = { 0 };
		struct libusb_config_descriptor *conf_desc = NULL;


		r = libusb_get_device_descriptor(pdev, &dev_desc);
		if (r < 0)continue;

		char szDeviceDesc[32] = { 0 };
		sprintf(szDeviceDesc, "vid_%04x&pid_%04x", dev_desc.idVendor, dev_desc.idProduct);
		printf("%s\n", szDeviceDesc);

		if (0x55c != dev_desc.idVendor)
			continue;

		//if (0 != strncmp(szProdectId, szDeviceDesc, strlen(szProdectId)))		
		//	continue;
				
		for (int jj = 0; jj < dev_desc.bNumConfigurations; jj++)
		{
			r = libusb_get_config_descriptor(pdev, jj, &conf_desc);
			if (r < 0)continue;

			for (int i = 0; i < conf_desc->bNumInterfaces; i++)
			{
				lm_usbdev usbdev = { 0 };
				usbdev.interfaceIndex = i;//记录接口描述符index

				const struct libusb_endpoint_descriptor *endpoint = NULL;

				for (int j = 0; j < conf_desc->interface[i].num_altsetting; j++)
				{
					usbdev.interfaceClass = conf_desc->interface[i].altsetting[j].bInterfaceClass;//LIBUSB_CLASS_SMART_CARD  LIBUSB_CLASS_HID LIBUSB_CLASS_MASS_STORAGE

					for (int k = 0; k < conf_desc->interface[i].altsetting[j].bNumEndpoints; k++)
					{
						endpoint = &conf_desc->interface[i].altsetting[j].endpoint[k];
						
						const char* transfer_type = NULL;
						const char* direction = NULL;

						//printf("endpoint number = %d",endpoint->bEndpointAddress & 0xF); 
						if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN)
						{
							//endpoint_in = 1000 0001 = 0x81
							usbdev.endpoint_in_type[0] = endpoint->bEndpointAddress;
							usbdev.endpoint_in_type[1] = endpoint->bmAttributes & 0x3;
							direction = "IN";
						}
						else
						{
							//endpoint_out = 0000 0001 = 0x1
							usbdev.endpoint_out_type[0] = endpoint->bEndpointAddress;
							usbdev.endpoint_out_type[1] = endpoint->bmAttributes & 0x3;
							direction = "OUT";
						}

						g_vcUsbDevs.push_back(usbdev);

						if ((endpoint->bmAttributes & 0x3) == LIBUSB_TRANSFER_TYPE_BULK)
						{
							transfer_type = "BULK";
						}

						if ((endpoint->bmAttributes & 0x3) == LIBUSB_TRANSFER_TYPE_INTERRUPT)
						{
							transfer_type = "INT";
						}

						printf("Endpoint %d %s: Type=%s Class=%02x SubClass=%02x Protocol=%02x MaxPacket=%02x\n",
							endpoint->bEndpointAddress & 0xF, direction, transfer_type,
							conf_desc->interface[i].altsetting[j].bInterfaceClass, conf_desc->interface[i].altsetting[j].bInterfaceSubClass, conf_desc->interface[i].altsetting[j].bInterfaceProtocol,
							endpoint->wMaxPacketSize
						);


					}
				}
			}
			if(conf_desc)			
				libusb_free_config_descriptor(conf_desc);			
		}

		

		//r = libusb_open(g_devs[i], &usb_handle);
		//if (r < 0)continue;


		//libusb_get_device_descriptor(g_devs[i], &dev_desc);

	}
	
	libusb_set_auto_detach_kernel_driver(usb_handle, 1);


	libusb_free_device_list(g_devs, 1);
}