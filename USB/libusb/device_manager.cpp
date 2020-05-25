#include "device_manager.h"


device_discover::device_discover()
{
}

device_discover::~device_discover()
{
}

device_manager::device_manager()
{
	_dev_number = 0;

	memset(_discover_parameer, 0, sizeof(_discover_parameer));
}


device_manager::~device_manager()
{
	//clear_devices();
}

//"gm3000,vid_055c&pid_db08,vid_055c&pid_e618|vid_055c&pid_f603|vid_055c&pid_2205";
int device_manager::split_enum_strings(char * input, char scsi_enum_id[256], char scsi_waitfor_id[256], char hid_enum[256])
{
	char * delim = ",";
	char temp[1024] = { 0 };
	char * p = NULL;
	strncpy(temp, input, 1024);

	p = strtok(temp, delim);
	if (!p) return 1;

	strncpy(scsi_enum_id, p, 256);

	p = strtok(NULL, delim);
	if (!p) return 2;

	strncpy(scsi_waitfor_id, p, 256);

	p = strtok(NULL, delim);
	if (!p) return 0;

	strncpy(hid_enum, p, 1024);

	ltrim(scsi_enum_id);
	ltrim(scsi_waitfor_id);
	ltrim(hid_enum);

	return 0;
}

int device_manager::split_pid_vid_strings(char * input, char hidEnum[16][256])
{
	char * delim = "|";
	char temp[1024] = { 0 };
	char * p = NULL;
	int  num = 0;
	strncpy(temp, input, 1024);

	p = strtok(temp, delim);
	if (!p) return 1;

	strncpy(hidEnum[num++], p, 256);

	while (p = strtok(NULL, delim))
	{
		strncpy(hidEnum[num++], p, 256);
	}

	return num;
}


void device_manager::ltrim(char *s)
{
	char *p;
	p = s;
	while (*p == ' ' || *p == '\t') { *p++; }
	strcpy(s, p);
}


int device_manager::discover_devices(char * parameter, char device_pathes[32][256])
{
	long			number = 0;
	long			hid_num = 0;
	long			scsi_num = 0;
	char			scsi_enum_id[1024] = { 0 };
	char			scsi_waitfor_id[1024] = { 0 };
	char			hid_enum[1024] = { 0 };
	char			discover_enum[1024] = { 0 };

	strncpy(discover_enum, parameter, 1024);
	if (0 != split_enum_strings(discover_enum, scsi_enum_id, scsi_waitfor_id, hid_enum))
	{
		return 0;
	}

	//DBG_LOGMSG("discover_devices %s", parameter);

	if (strlen(hid_enum) > 0)
	{
		//hid_num = g_hid_discover.discover_devices(hid_enum, device_pathes);
	}

	if (strlen(scsi_enum_id) > 0)
	{
		//scsi_num = g_scsi_discover.discover_devices(scsi_enum_id, &device_pathes[hid_num]);
	}

	//DBG_LOGMSG("libusb hidnumber = %d, scsinumber=%d", hid_num, scsi_num);
	number = hid_num + scsi_num;
	return number;
}