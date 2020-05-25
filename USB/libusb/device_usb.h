#pragma once

class device_base
{
public:
	device_base();
	~device_base();

private:

};

#include <string.h>
#include <string>
#include <vector>
#include <map>
using namespace std;

#include "libusb.h"

typedef struct pid_vid
{
	uint16_t vendor_id;
	uint16_t product_id;
	uint8_t  dev_addr;
}lm_pid_vid;

typedef struct
{
	lm_pid_vid pid_vid;

	uint8_t interfaceClass;	 //LIBUSB_CLASS_SMART_CARD  LIBUSB_CLASS_HID  LIBUSB_CLASS_MASS_STORAGE
	uint8_t interfaceSubClass;
	uint8_t protocol;
	uint8_t maxPacket;

	uint8_t	interfaceIndex;  //libusb_claim_interface(libusb_device_handle, interfaceIndex);
							 //libusb_release_interface(libusb_device_handle, interfaceIndex);
	uint8_t	endpoint_in;	//主机收包端点
	uint8_t endpoint_out;	//主机发包端点
	uint8_t	endpoint_in_type;	//主机收包端点通信方式INTERRUPT/BULK
	uint8_t endpoint_out_type;	//主机发包端点通信方式INTERRUPT/BULK
}lm_key;

class UsbDevMgr
{
private:
	UsbDevMgr();
public:
	~UsbDevMgr();

	static UsbDevMgr& Instance();
public:

	int find_device_with_vid_pid(vector<string> pivs, vector<libusb_device *> &devs);

	int find_device_with_vid_pid(vector<lm_pid_vid> pivs, vector<libusb_device *> &devs);

	int get_device_info(libusb_device* devs, lm_key* hidkey, lm_key* cdkey, lm_key* ccidkey);

private:
	libusb_context *_ctx;
	libusb_device **_devs;
	void release_devices();
};





class device_usb :
	public device_base
{
public:
	device_usb();
	~device_usb();


	typedef enum
	{
		devStateNormal = 0,
		devStateNew = 1,
		devStateRemoved = 2,
		devStateAny = 3

	}enmDevState;

	virtual int open();

	virtual int close();

	/*
	virtual char * get_dev_path();

	virtual char * get_dev_name();

	virtual	void set_dev_name(char * devName);

	virtual int set_dev_state(enmDevState state);

	virtual enmDevState get_dev_state();

	virtual int get_device_type();


	virtual void set_comm_timeout(int time_out = USB_TIME_OUT);
	virtual void reset_comm_timeout();

	virtual int send_data(unsigned char * request, int request_len);

	virtual int receive_data(unsigned char * response, int *response_len);

	virtual int send_raw_data(unsigned char * cdb, int cbw_len, unsigned char * request, long request_len);

	virtual int receive_raw_data(unsigned char * cdb, int cbw_len, unsigned char * response, long *response_len);

	virtual int device_io(unsigned char * request, int request_len, unsigned char * response, int *response_len);
	*/
private:
	lm_key _keyinfo;
	libusb_device *_dev;
	libusb_device_handle *_usb_handle;

	char			_dev_path[256]; //自定义设备唯一串 									
									//"hid_vid_%04x&pid_%04x&addr_%04x"     hid
									//"scard_vid_%04x&pid_%04x&addr_%04x"   scsi
									//"hidctr_vid_%04x&pid_%04x&addr_%04x"  hid_cd
	char			_dev_sn[256];
	enmDevState		_device_state;
	int				_timeout;
};

