#include "device_usb.h"

device_base::device_base()
{
}

device_base::~device_base()
{
}



void printfusb(lm_key* key)
{
	if (key)
	{
		printf("pid_%04x&vid_%04x  interface=%d\n", key->pid_vid.product_id, key->pid_vid.vendor_id, key->interfaceIndex);

		const char* transfer_type = NULL;
		if (key->endpoint_out_type == LIBUSB_TRANSFER_TYPE_BULK)
			transfer_type = "BULK";
		if (key->endpoint_out_type == LIBUSB_TRANSFER_TYPE_INTERRUPT)
			transfer_type = "INT";
		printf("Endpoint %d %s: Type=%s Class=%02x SubClass=%02x Protocol=%02x MaxPacket=%02x\n",
			key->endpoint_out & 0xF, "OUT", transfer_type, key->interfaceClass, key->interfaceSubClass, key->protocol, key->maxPacket
		);

		if (key->endpoint_in_type == LIBUSB_TRANSFER_TYPE_BULK)
			transfer_type = "BULK";
		if (key->endpoint_in_type == LIBUSB_TRANSFER_TYPE_INTERRUPT)
			transfer_type = "INT";

		printf("Endpoint %d %s: Type=%s Class=%02x SubClass=%02x Protocol=%02x MaxPacket=%02x\n",
			key->endpoint_in & 0xF, "IN", transfer_type, key->interfaceClass, key->interfaceSubClass, key->protocol, key->maxPacket
		);
	}
}

int main()
{

	vector<lm_pid_vid> pivs;

	vector<libusb_device *> devs;

	lm_pid_vid pidcd = { 0x55c, 0xdb08 };
	lm_pid_vid pidcdhid = { 0x55c, 0xf603 };
	pivs.push_back(pidcd);
	pivs.push_back(pidcdhid);

	int num = UsbDevMgr::Instance().find_device_with_vid_pid(pivs, devs);
	for (int i = 0; i < num; i++)
	{
		lm_key hidkey = { 0 };
		lm_key cdkey = { 0 };
		lm_key ccidkey = { 0 };

		int r = UsbDevMgr::Instance().get_device_info(devs[i], &hidkey, &cdkey, &ccidkey);
		if (r < 0)
			continue;

		if (hidkey.interfaceClass == LIBUSB_CLASS_HID)
			printfusb(&hidkey);

		if (cdkey.interfaceClass == LIBUSB_CLASS_MASS_STORAGE)
			printfusb(&cdkey);

		if (ccidkey.interfaceClass == LIBUSB_CLASS_SMART_CARD)
			printfusb(&ccidkey);

	}


	return 0;
}


UsbDevMgr& UsbDevMgr::Instance()
{
	static UsbDevMgr usbmgr;
	return usbmgr;
}

UsbDevMgr::UsbDevMgr() :_ctx(NULL), _devs(NULL)
{
	libusb_init(&_ctx);
}

UsbDevMgr::~UsbDevMgr()
{
	release_devices();
	libusb_exit(_ctx);
}


void UsbDevMgr::release_devices()
{
	if (_devs)
	{
		//libusb_free_device_list(_devs, 0);//删除libusb_device **，不调用libusb_unref_device 不减少设备的引用计数，内存泄漏
		libusb_free_device_list(_devs, 1);//删除libusb_device **,并且libusb_unref_device 减少设备的引用计数
		//调用libusb_open后也会增加引用计数,所有调用open之后
	}

}

int UsbDevMgr::find_device_with_vid_pid(vector<string> pivs, vector<libusb_device *> &devs)
{
	int devNum = 0, r = LIBUSB_SUCCESS;

	libusb_device **tmpdevs = NULL;

	char szDeviceDesc[32] = { 0 };
	std::size_t in_size = pivs.size();	

	devNum = libusb_get_device_list(_ctx, &tmpdevs);//枚举的时候已经调用libusb_ref_device对每一个设备进行了引用计数
	if (devNum > 0)
	{
		release_devices();

		devs.clear();

		_devs = tmpdevs;

		for (int i = 0; i < devNum; i++)
		{
			struct libusb_device_descriptor dev_desc = { 0 };

			r = libusb_get_device_descriptor(_devs[i], &dev_desc);
			if (r < 0)continue;
			
			sprintf(szDeviceDesc, "vid_%04x&pid_%04x", dev_desc.idVendor, dev_desc.idProduct);

			for (size_t j = 0; j < in_size; j++)
			{
				if (strcmp(pivs[i].c_str(), szDeviceDesc) == 0)
				{
					devs.push_back(_devs[i]);
					break;
				}
			}
		}
	}
	else
	{
		if (tmpdevs)
			libusb_free_device_list(tmpdevs, 1);
	}

	return devs.size();
}

int UsbDevMgr::find_device_with_vid_pid(vector<lm_pid_vid> pivs, vector<libusb_device *> &devs)
{
	int devNum = 0, r = LIBUSB_SUCCESS;

	libusb_device **tmpdevs = NULL;

	devNum = libusb_get_device_list(_ctx, &tmpdevs);//枚举的时候已经调用libusb_ref_device对每一个设备进行了引用计数
	if (devNum > 0)
	{
		release_devices();

		devs.clear();

		_devs = tmpdevs;

		for (int i = 0; i < devNum; i++)
		{
			struct libusb_device_descriptor dev_desc = { 0 };

			r = libusb_get_device_descriptor(_devs[i], &dev_desc);
			if (r < 0)continue;

			std::size_t in_size = pivs.size();
			for (size_t j = 0; j < in_size; j++)
			{
				if (dev_desc.idVendor == pivs[j].vendor_id  && dev_desc.idProduct == pivs[j].product_id)
				{
					devs.push_back(_devs[i]);
					break;
				}
			}
		}
	}
	else
	{
		if (tmpdevs)
			libusb_free_device_list(tmpdevs, 1);
	}

	return devs.size();
}

int UsbDevMgr::get_device_info(libusb_device* devs, lm_key* hidkey, lm_key* cdkey, lm_key* ccidkey)
{
	if (NULL == devs || NULL == hidkey || NULL == cdkey || NULL == ccidkey)
		return LIBUSB_ERROR_INVALID_PARAM;

	struct libusb_device_descriptor dev_desc = { 0 };
	struct libusb_config_descriptor *conf_desc = NULL;

	int	r = LIBUSB_SUCCESS;
	//int faceClass = 0;
	do
	{
		//1、取设备描述符
		r = libusb_get_device_descriptor(devs, &dev_desc);
		if (r < 0)break;

		lm_key tmpkey = { 0 };
		tmpkey.pid_vid.vendor_id = dev_desc.idVendor;
		tmpkey.pid_vid.product_id = dev_desc.idProduct;
		tmpkey.pid_vid.dev_addr = libusb_get_device_address(devs);

		for (int h = 0; h < dev_desc.bNumConfigurations; h++)
		{
			//2、取配置描述符
			r = libusb_get_config_descriptor(devs, h, &conf_desc);
			if (r < 0)
				continue;

			for (int i = 0; i < conf_desc->bNumInterfaces; i++)
			{
				tmpkey.interfaceIndex = i;//记录接口描述符index

				for (int j = 0; j < conf_desc->interface[i].num_altsetting; j++)
				{
					tmpkey.interfaceClass = conf_desc->interface[i].altsetting[j].bInterfaceClass;//LIBUSB_CLASS_SMART_CARD  LIBUSB_CLASS_HID LIBUSB_CLASS_MASS_STORAGE
					tmpkey.interfaceSubClass = conf_desc->interface[i].altsetting[j].bInterfaceSubClass;
					tmpkey.protocol = conf_desc->interface[i].altsetting[j].bInterfaceProtocol;

					//3、取端点描述符
					for (int k = 0; k < conf_desc->interface[i].altsetting[j].bNumEndpoints; k++)
					{
						const struct libusb_endpoint_descriptor *endpoint = &conf_desc->interface[i].altsetting[j].endpoint[k];
						tmpkey.maxPacket = endpoint->wMaxPacketSize;

						if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN)
						{
							//endpoint_in = 1000 0001 = 0x81
							tmpkey.endpoint_in = endpoint->bEndpointAddress;
							tmpkey.endpoint_in_type = endpoint->bmAttributes & 0x3;
						}
						else
						{
							//endpoint_out = 0000 0001 = 0x1
							tmpkey.endpoint_out = endpoint->bEndpointAddress;
							tmpkey.endpoint_out_type = endpoint->bmAttributes & 0x3; //LIBUSB_TRANSFER_TYPE_BULK    LIBUSB_TRANSFER_TYPE_INTERRUPT
						}
					}
				}

				if (LIBUSB_CLASS_HID == tmpkey.interfaceClass)
				{
					//faceClass |= LIBUSB_CLASS_HID; //3		   0x11
					memcpy(hidkey, &tmpkey, sizeof(lm_key));
				}
				else if (LIBUSB_CLASS_MASS_STORAGE == tmpkey.interfaceClass)
				{
					//faceClass |= LIBUSB_CLASS_MASS_STORAGE;//8  0x1000
					memcpy(cdkey, &tmpkey, sizeof(lm_key));
				}
				else if (LIBUSB_CLASS_SMART_CARD == tmpkey.interfaceClass)
				{
					//faceClass |= LIBUSB_CLASS_SMART_CARD;//0x0b  0x1011
					memcpy(ccidkey, &tmpkey, sizeof(lm_key));
				}
				else
				{

				}

			}
			if (conf_desc)
				libusb_free_config_descriptor(conf_desc), conf_desc = NULL;
		}

	} while (0);

	return  r;
}


device_usb::device_usb()
{
}


device_usb::~device_usb()
{
}




int device_usb::open()
{
	int r = LIBUSB_ERROR_INVALID_PARAM;
	if (_dev)
	{
		int r = libusb_open(_dev, &_usb_handle);
		if (LIBUSB_SUCCESS == r)
			libusb_set_auto_detach_kernel_driver(_usb_handle, 1);
	}
	return r;
}

int device_usb::close()
{
	if (_usb_handle)
		libusb_close(_usb_handle), _usb_handle = NULL;
}
