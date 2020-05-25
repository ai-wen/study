#pragma once

//#include "../device_base.h"
#include "device_usb.h"

class device_discover
{
public:
	device_discover();
	~device_discover();

private:

};



class device_manager :
	public device_discover
{
public:
	device_manager();
	~device_manager();
public:
	int discover_devices(char * parameter, char device_pathes[32][256]);

	/*
	int enum_and_create_devices(char * parameter);

	int update_devices(int * removed_num, int * new_num, char * parameter);

	device_base * get_device(int index);

	int get_device_number();

	device_base * get_first_device(MK_POSITION & pos);

	device_base * get_next_device(MK_POSITION & pos);

	int destroy_removed_deviced();
	*/
private:
	/*
	void clear_devices();

	void release_mk_node(mk_node * node);

	device_base * get_device_from_node(mk_node * node);

	device_base * create_device(char * dev_path);

	device_base * get_device_by_path(char * dev_path);

	device_base * get_device_by_position(MK_POSITION pos);

	int contains_string(char * strElement, char strColleciton[][256], int collection_size);

	int detect_new_devices(int devNumber, char devPathes[][256]);

	int detect_removed_devices(char devPathes[][256]);
	*/
	
	static int split_enum_strings(char * input, char scsi_enum_id[256], char scsi_waitfor_id[256], char hid_enum[256]);
	static int split_pid_vid_strings(char * input, char Enum[16][256]);
	static void ltrim(char *s);

private:
	//mk_node_list	_device_list;
	int				_dev_number;
	char			_discover_parameer[256];

	//hid_device_discover		g_hid_discover;
	//scsi_device_discover	g_scsi_discover;
};

