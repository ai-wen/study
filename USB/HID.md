# Windows   

## 主机端设备枚举程序流程

![主机端设备枚举程序流程](https://github.com/ai-wen/study/blob/master/USB/hid.jpg)  

## API系列1 中断传输
> * CreateFile 
> * CloseHandle 
> * 1.设备到主机方向的数据通信 
>    + HidD_GetFeature
>    + HidD_GetInputReport
>    + ReadFile   **通过中断IN 进行数据传输**
> * 2.主机到设备方向的数据通信。**（需要注意report ID的不同）**
>    + HidD_SetFeature
>    + WriteFile  **设备接收到输出报告请求，如果设备使用中断OUT进行数据传输则会通过中断 OUT 管道来进行传输；否则会使用 SetReport 请求通过控制管道来传输。**
>    + HidD_SetOutputReport
> * GetLastError() 会得到以下常见错误:
>     - 1 :           Error Code 1 (incorrect function)
>     不支持此函数，很可能是设备的报告描述符中未定义这样的报告类型（输入、输出、特征）
>     - 6 ：          句柄无效  
>     - 23 ：        数据错误（循环冗余码检查）  
>     - 87 ：        参数错误  
>      很可能是 createfile 时声明了异步方式，但是读取时按同步读取。
>     - 1784 ：     用户提供的 buffer 无效,传参时传入的“读取 buffer 长度”与实际的报告长度不符。  

## API系列2 控制传输
> * CreateFile 
> * CloseHandle 
> * DeviceIoControl
>    + IOCTL_STORAGE_MCN_CONTROL
>   + 1.HID
>     + IOCTL_HID_SET_OUTPUT_REPORT
>     + IOCTL_HID_GET_INPUT_REPORT
>   + 2.SCSI
>     + IOCTL_SCSI_PASS_THROUGH_DIRECT
> * CancelIo
```cpp
HID:
DeviceIoControl(hHid,
				IOCTL_HID_SET_OUTPUT_REPORT, //IOCTL_HID_GET_INPUT_REPORT
				SendBuff,
				(DataLength+1),
				NULL,
				0x0,
				&numBytesReturned,
				FALSE);	
SCSI:
	SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER sptdwb = {0};
	sptdwb.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	sptdwb.sptd.PathId = 0;
	sptdwb.sptd.TargetId = 1;
	sptdwb.sptd.Lun = 0;
	sptdwb.sptd.CdbLength = GM_CDB_GENERIC_LENGTH;
	sptdwb.sptd.SenseInfoLength = SPT_SENSE_LENGTH;
	sptdwb.sptd.DataIn = SCSI_IOCTL_DATA_OUT;//SCSI_IOCTL_DATA_IN
	sptdwb.sptd.DataTransferLength = buffSize;
	sptdwb.sptd.TimeOutValue = USB_SCSI_TIMEOUT;
	sptdwb.sptd.DataBuffer = pDataWriteBuff;
	sptdwb.sptd.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER,ucSenseBuf);
	sptdwb.sptd.Cdb[0] = cmd;
DeviceIoControl(_win32Handle,
		IOCTL_SCSI_PASS_THROUGH_DIRECT,
		&sptdwb,
		sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER),
		&sptdwb,
		sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER),
		&bytesReturn,
		&_ovSCSIOverlapped);

```


## API 详解
``` cpp
设备路径通过函数SetupDiGetInterfaceDeviceDetail 取得

CreateFile( devDetail->DevicePath,         // 设备路径
            GENERIC_READ | GENERIC_WRITE, // 访问方式
            FILE_SHARE_READ | FILE_SHARE_WRITE,// 共享模式
            NULL,
            OPEN_EXISTING,// 文件不存在时，返回失败
            FILE_FLAG_OVERLAPPED,// 以重叠（异步）模式打开
            NULL);
-访问方式： 
    系统独占设备，例如鼠标、键盘等等，应将此参数设置为 0 ，否则后续函数操作将失败（譬如 HidD_GetAttributes ）；也就是说，不能对独占设备进行除了查询以外的任何操作，所以能够使用的函数也是很有限的，下文的一些函数并不一定适合这些设备。在此顺便列出 MSDN 上关于此参数的说明：
    If this parameter is zero, the application can query file and device attributes without accessing the device. This is useful if an application wants to determine the size of a floppy disk drive and the formats it supports without requiring a floppy in the drive. It can also be used to test for the file's or directory's existence without opening it for read or write access 。
    重叠（异步）模式：此参数并不会在此处表现出明显的意义，它主要是对后续的 WriteFile ，ReadFile 有影响。如果这里设置为重叠（异步）模式，那么在使用 WriteFile ， ReadFile 时也应该使用重叠（异步）模式，反之亦然。这首先要求 WriteFile ， ReadFile 的最后一个参数不能为空（NULL ）。否则，便会返回 87 （参数错误）错误号。当然， 87 号错误并不代表就是此参数不正确，更多的信息将在具体讲述这两个函数时指出。此参数为 0 时，代表同步模式，即 WriteFile ， ReadFile操作会在数据处理完成之后才返回，否则阻塞在函数内部。            
```
``` cpp
ReadFile 用于读取 HID 设备通过中断 IN 传输发来的输入报告

ReadFile( hDev,			// 设备句柄，即 CreateFile 的返回值
          recvBuffer,	// 用于接收数据的 buffer
          IN_REPORT_LEN,// 要读取数据的长度
          &recvBytes,	// 实际收到的数据的字节数
          &ol);			// 异步模式
1、ReadFile的调用不会引起设备的任何反应，即HID设备与主机之间的中断IN传输不与ReadFile打交道。实际上主机会在最大间隔时间（由设备的端点描述符来指定）内轮询设备，发出中断IN传输的请求。“读取”即意味着从某个buffer里面取回数据，实际上这个buffer就是HID设备驱动中的buffer。这个buffer的大小可以通过HidD_SetNumInputBuffers来改变。在 XP 上缺省值是 32（个报告）。
2、读取的数据对象是输入报告，也即通过中断输入管道传入的数据。所以，如果设备不支持中断IN传输，那么是无法使用此函数来得到预期结果的。实际上这种情况不可能在HID中出现，因为协议指明了至少要有一个中断 IN 端点。
3、IN_REPORT_LEN 代表要读取的数据的长度（不过也可以通过另外的函数（ HidD_GetPreparsedData ）来事先取得报告的长度）
4、关于异步模式。前面已经提过，此参数的设置必须与CreateFile时的设置相对应，否则会返回87号错误（参数错误）。如果不需要异步模式，此参数需置为NULL。在这种情况下， ReadFile 会一直等待直到数据读取成功，所以会阻塞住程序的当前过程。
```
``` cpp
 WriteFile 用于传输一个输出报告给 HID 设备

 WriteFile(	hDev,		// 设备句柄，即 CreateFile 的返回值
			reportBuf,	// 存有待发送数据的 buffer
			OUT_REPORT_LEN,// 待发送数据的长度
			&sendBytes,	// 实际收到的数据的字节数
			&ol);		// 异步模式
1、与 ReadFile 不同， WriteFile 函数被调用后，虽然也是经过驱动程序，但是最终会反映到设备中。也就是说，调用 WriteFile 后，设备会接收到输出报告的请求。如果设备使用了中断 OUT 传输，则WriteFile 会通过中断 OUT 管道来进行传输；否则会使用 SetReport 请求通过控制管道来传输。
2、OUT_REPORT_LEN 代表要写入的数据长度（实际的数据正文 + 一个 byte 的报告 ID ）。如果大于实际报告的长度，则使用实际报告长度；如果小于实际报告长度，会返回 1784 号错误（用户提供的 buffer 无效）。
3、reportBuf [0] 必须存有待发送报告的 ID ，并且此报告 ID 指示的必须是输出报告，否则会返回 87号错误（参数错误）。这种情况可能容易被程序员忽略，结果不知错误号所反映的是什么，网上也经常有类似疑问的帖子。顺便指出，输入报告、输入报告、特征报告这些报告类型，是反映在 HID 设备的报告描述符中。后文将做举例讨论。
4、关于异步模式。前面已经提过，此参数的设置必须与 CreateFile 时的设置相对应，否则会返回 87 号错误（参数错误）。如果不需要异步模式，此参数需置为 NULL 。在这种情况下， WriteFile 会一直等待直到数据读取成功，所以会阻塞住程序的当前过程。  

```
``` cpp
HidD_SetFeature 发送一个特征报告 给设备， HidD_ SetOutputReport 发送一个输出报告 给设备。
HidD_SetFeature(hDev,		// 设备句柄，即CreateFile 的返回值
                reportBuf,	// 存有待发送数据的 buffer
                FEATURE_REPORT_LEN);//buffer 的长度
HidD_SetOutputReport(hDev,	// 设备句柄，即 CreateFile的返回值
                     reportBuf,// 存有待发送数据的 buffer
                     OUT_REPORT_LEN);//buffer 的长度
1、跟 WriteFile 类似，必须在 reportBuf [0] 中指明要发送的报告的 ID ，并且和各自适合的类型相对应。也就是说， HidD_SetFeature 只能发送特征报告，因此报告 ID 必须是特征报告的 ID ；HidD_SetOutputReport 只能发送输出报告，因此报告 ID 只能是输出报告的 ID 。
2、这两个函数最常返回的错误代码是 23 （数据错误）。包括但不仅限于以下情况：
- 报告 ID 与固件描述的不符。
- 传入的 buffer 长度少于固件描述的报告的长度。
据有关资料反映（非官方文档），只要是驱动程序对请求无反应，都会产生此错误。
```                     

### 报告描述符及数据通信
```cpp

const UINT8 HID_DataReportDesc[] = 
{
	0x06,0xA0,0xFF,        /*Usage Page (vendor defined FFA0)*/
	0x09,0x01,       /*Usage (vendor defined)*/
	0xA1,0x01,        /*Collection (Application)*/
	0x06,0xA1,0xFF,     /*Usage Page (vendor defined FFA1)*/

	/*The input report*/
	0x09,0x03,     /*usage - vendor defined*/
	0x15,0x00,     /*logical minimum (0)*/
	0x25,0xff,     /*logical maximum*/
	0x75,0x08,     /* Report Size (8)  (bits)*/
	#if USB_HID_REPORT_LEN <= 255
	0x95,     /* Report Count (1B)  (fields)*/
	USB_HID_REPORT_LEN,
	#else
	0x96,
	USB_HID_REPORT_LEN & 0xff,
	(USB_HID_REPORT_LEN >> 8) & 0xff,
	#endif
	0x81,0x02,     /*Input (Data, Variable, Absolute)*/
	
	/*The output report*/
	0x09,0x04,     /*usage - vendor defined*/
	0x15,0x00,     /*logical minimum (0)*/
	0x25,0xff,     /*logical maximum*/
	0x75,0x08,     /*Report Size (8)  (bits)*/
	#if USB_HID_REPORT_LEN <= 255
	0x95,     /* Report Count (1B)  (fields)*/
	USB_HID_REPORT_LEN,
	#else
	0x96,
	USB_HID_REPORT_LEN & 0xff,
	(USB_HID_REPORT_LEN >> 8) & 0xff,
	#endif
	0x91,0x02,     /*Output (Data, Variable, Absolute)*/
	
	0xC0        /*End Collection*/
};

const UINT8 hid_msg_descr[42] = 
{
	0x06, 0xa0, 0xff,           //usage page (Vendor defined) 
	0x09, 0xa5,                 //usage (Vendor defined)
	0xa1, 0x01,                 //collection (application)
	0x09, 0xa6,                 //usage (Vendor defined)
	//the input report
	0x09, 0xa7,                 //usage (Vendor defined)
	0x15, 0x80,                 //logical minimum (-127)
	0x25, 0x7f,                 //logical maximum (128)
	0x35, 0x00,                 //physical minimum(0) 
	0x45, 0xff,                 //physical maximum(255) 
	0x75, 0x08,                 //report size 8 (bits) 
	0x95, HID_REPORT_SIZE,            //report count 32 (fields) 
	0x81, 0x9a,                 //input (Data,Var,rel)
	//the output report
	0x09, 0xa9,                 //usage (Vendor defined)
	0x15, 0x80,                 //logical minimum (-127) 
	0x25, 0x7f,                 //logical maximum (128)
	0x35, 0x00,                 //physical minimum(0) 
	0x45, 0xff,                 //physical maximum(255) 
	0x75, 0x08,                 //report size 8 (bits) 
	0x95, HID_REPORT_SIZE,      //report count 32 (fields) 
	0x91, 0x9a,                 //onput (Data,Var,rel)
	0xc0                        //END_COLLECTION	 
};


上面两个报告描述符没有定义报告 ID，默认传0，下面这个报告描述符，定义了 4 个不同的报告：输入报告 1 ，特征报告 2 ，特征报告 3 ，输出报告 4 （数字代表其报告 ID ）。为了简化，每个报告都是 7 个字节（加上报告 ID 就是 8 个字节）。
_ReportDescriptor{   // 报告描述符

0x06, 0x00, 0xff // 用法页
0x09, 0x01 // 用法 ( 供应商用法 1)
0xa1, 0x01 // 集合开始

0x85, 0x01 // 报告 ID(1)
0x09, 0x01 // 用法 ( 供应商用法 1)
0x15, 0x00 // 逻辑最小值 (0)
0x26, 0xff, 0x0 // 逻辑最大值 (255)
0x75, 0x08 // 报告大小 (8)
0x95, 0x07 // 报告计数 (7)
0x81, 0x06 // 输入 （数据，变量，相对值）

0x09, 0x01 // 用法 ( 供应商用法 1)
0x85, 0x03 // 报告 ID （ 3 ）
0xb1, 0x06 // 特征 （数据，变量，相对值）

0x09, 0x01 // 用法 ( 供应商用法 1)
0x85, 0x02 // 报告 ID （ 2 ）
0xb1, 0x06 // 特征 （数据，变量，相对值）

0x09, 0x01 // 用法 ( 供应商用法 1)
0x85, 0x04 // 报告 ID （ 4 ）
0x91, 0x06 // 输出 （数据，变量，相对值）

0xc0 // 结合结束
};


#define     USB_VID       0xFC0   
#define     USB_PID       0x420   
HANDLE OpenMyHIDDevice(int overlapped);   
void HIDSampleFunc()   
{   
    HANDLE       hDev;   
    BYTE         recvDataBuf[8];   
    BYTE         reportBuf[8];   
    DWORD        bytes;   
    hDev = OpenMyHIDDevice(0);                                // 打开设备，不使用重叠（异步）方式 ;   
    if (hDev == INVALID_HANDLE_VALUE)   
        return;   
    
    reportBuf[0] = 4;                                         // 输出报告的报告 ID 是 4   
    memset(reportBuf, 0, 8);   
    reportBuf[1] = 1;   
    if (!WriteFile(hDev, reportBuf, 8, &bytes, NULL))         // 写入数据到设备   
         return;   
    ReadFile(hDev, recvDatatBuf, 8, &bytes, NULL);            // 读取设备发给主机的数据   
}   
    
HANDLE OpenMyHIDDevice(int overlapped)   
{   
    HANDLE hidHandle;   
    GUID hidGuid;   
    HidD_GetHidGuid(&hidGuid);   
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&hidGuid,  NULL,   
    NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));   
    if (hDevInfo == INVALID_HANDLE_VALUE)   
    {   
        return INVALID_HANDLE_VALUE;   
    }   
    SP_DEVICE_INTERFACE_DATA devInfoData;   
    devInfoData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);   
    int deviceNo = 0;   
    SetLastError(NO_ERROR);   
    while (GetLastError() != ERROR_NO_MORE_ITEMS)   
    {   
        if (SetupDiEnumInterfaceDevice (hDevInfo,   
           0,   &hidGuid,   deviceNo, &devInfoData))   
        {   
            ULONG  requiredLength = 0;   
            SetupDiGetInterfaceDeviceDetail(hDevInfo,   
             &devInfoData,NULL,   0,  &requiredLength,  NULL);  
            PSP_INTERFACE_DEVICE_DETAIL_DATA devDetail = (SP_INTERFACE_DEVICE_DETAIL_DATA*) malloc (requiredLength);   
            devDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);   
            if(!SetupDiGetInterfaceDeviceDetail(hDevInfo,   
                &devInfoData,   devDetail,    requiredLength,   
                  NULL,   NULL))   
            {   
                free(devDetail);   
                SetupDiDestroyDeviceInfoList(hDevInfo);   
                return INVALID_HANDLE_VALUE;   
            }   
            if (overlapped)   
            {   
                hidHandle = CreateFile(devDetail->DevicePath,   
                 GENERIC_READ | GENERIC_WRITE,   
                FILE_SHARE_READ | FILE_SHARE_WRITE,   
                 NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED,   
                 NULL);   
            }   
            else   
            {   
                hidHandle = CreateFile(devDetail->DevicePath,   
                GENERIC_READ | GENERIC_WRITE,   
                FILE_SHARE_READ | FILE_SHARE_WRITE,   
                NULL, OPEN_EXISTING, 0,NULL);   
            }   
            free(devDetail);   
            if (hidHandle==INVALID_HANDLE_VALUE)   
            {   
                SetupDiDestroyDeviceInfoList(hDevInfo);   
                free(devDetail);   
                return INVALID_HANDLE_VALUE;   
            }   
            _HIDD_ATTRIBUTES hidAttributes;   
            if(!HidD_GetAttributes(hidHandle, &hidAttributes))   
            {   
                CloseHandle(hidHandle);   
                SetupDiDestroyDeviceInfoList(hDevInfo);   
                return INVALID_HANDLE_VALUE;  
            }
        }
    }
}                
```
