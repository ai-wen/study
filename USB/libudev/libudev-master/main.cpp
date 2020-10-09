#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <sys/select.h>
#include <libudev.h>
    
struct udev_device 	*dev  = NULL;
struct udev_monitor *mon  = NULL;
struct udev 		*udev = NULL;

fd_set readfds;
char* device;
const char *action = NULL;

dev_t devnum = 0;
unsigned int major = 0, minor = 0;
const char *value = NULL;
int main ()
{
	udev = udev_new();
	if(!udev) {
		fprintf(stderr, "Can't create udev.\n");
		exit(EXIT_FAILURE);
	}

	mon = udev_monitor_new_from_netlink(udev, "udev");
	udev_monitor_filter_add_match_subsystem_devtype(mon, "block", NULL);
	udev_monitor_enable_receiving(mon);

	while(1)
	{
		if (mon != NULL)
			FD_SET(udev_monitor_get_fd(mon), &readfds);
		select(udev_monitor_get_fd(mon) + 1, &readfds, NULL, NULL, NULL);

		if ((mon != NULL) && FD_ISSET(udev_monitor_get_fd(mon), &readfds)) 
		{
			dev = udev_monitor_receive_device(mon);
			if(dev) {
				device = (char *) udev_device_get_sysname(dev);
			}
		}

		devnum = udev_device_get_devnum(dev);
		major = major(devnum);
		minor = minor(devnum);
		action = udev_device_get_action(dev);
		printf("Processing device %s %d:%d\n", action, major, minor);

		struct udev_list_entry * props = udev_device_get_properties_list_entry(dev);
		while(props != NULL)
		{
			printf("%s = %s\n", udev_list_entry_get_name(props), udev_list_entry_get_value(props));
			props = udev_list_entry_get_next (props);
		}

		props = udev_device_get_sysattr_list_entry(dev);
		while(props != NULL)
		{
			printf("%s = %s\n", udev_list_entry_get_name(props), udev_device_get_sysattr_value(dev, udev_list_entry_get_name(props)));
			props = udev_list_entry_get_next (props);
		}
	}

  	return 0;
}