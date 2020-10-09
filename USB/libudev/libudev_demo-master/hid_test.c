/*  LIBUDEV HID SAMPLE
 *  
 *  Author: Vincent Yuan
 *  Date  : 2017-07-26
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <libudev.h>


int main(int argc, char* argv[]){
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry, *udev_sys_attr, *sys_attr_entry;
    udev = udev_new();

    if(!udev){
        printf("Create udev fail\r\n");
    }
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate,"hidraw");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    printf("=-=-=-=-=-=-=-=-=-=-=-=-=\r\nResults: \r\n");
//Loop through all the devices found 
    udev_list_entry_foreach(dev_list_entry,devices){
            const char *sysfs_path, *dev_path; 
            struct udev_device *hid_dev, *dev;
            sysfs_path = udev_list_entry_get_name(dev_list_entry);
            printf("%s",sysfs_path);
            hid_dev = udev_device_new_from_syspath(udev,sysfs_path);
            dev_path = udev_device_get_devnode(hid_dev);
            dev_path = "/dev/hidraw2";
            //Getting the parent device is important to differenciate from virtual ports with not info
        while(udev_device_get_parent(hid_dev)){
            hid_dev = udev_device_get_parent(hid_dev);
            udev_sys_attr = udev_device_get_sysattr_list_entry(hid_dev);
    //Get sys atttributes
            printf("\r\n=============\r\nsys attr: \r\n");
            udev_list_entry_foreach(sys_attr_entry,udev_sys_attr){
                printf("||  %s \r\n",udev_list_entry_get_value(sys_attr_entry));
            }

    //Get sys Properties 

            udev_sys_attr = udev_device_get_properties_list_entry(hid_dev);
            printf("\r\n=============\r\nsys properties: \r\n");
            //Generally second item is device path 
            udev_list_entry_foreach(sys_attr_entry,udev_sys_attr){
                printf("||  %s \r\n",udev_list_entry_get_value(sys_attr_entry));
            }

    //Get sys tags

            udev_sys_attr = udev_device_get_tags_list_entry(hid_dev);
            printf("\r\n=============\r\nsys tags: \r\n");
            udev_list_entry_foreach(sys_attr_entry,udev_sys_attr){
                printf("|| %s \r\n",udev_list_entry_get_value(sys_attr_entry));
            }

    //Get sys devlinks

            udev_sys_attr = udev_device_get_devlinks_list_entry(hid_dev);
            printf("\r\n=============\r\nsys devlinks: \r\n");
            udev_list_entry_foreach(sys_attr_entry,udev_sys_attr){
                printf("|| %s \r\n",udev_list_entry_get_value(sys_attr_entry));
            }
    //
            printf("\r\n=========================================\r\n");
            printf("Udev: %s \r\n",udev_device_get_sysattr_value(hid_dev,"uevent"));
            printf("idVendor: %s \r\n",udev_device_get_sysattr_value(hid_dev,"idVendor"));
        }
    }
}









