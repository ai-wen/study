1. 初始化 
    首先调用udev_new，创建一个udev library context。udev library context采用引用记数机制，创建的context默认引用记数为1，使用udev_ref和udev_unref增加或减少引用记数，如果引用记数为0，则释放内部资源。 
2. 枚举设备 
    使用udev_enumrate_new创建一个枚举器，用于扫描系统已接设备。使用udev_enumrate_ref和udev_enumrate_unref增加或减少引用记数。 
    使用udev_enumrate_add_match/nomatch_xxx系列函数增加枚举的过滤器，过滤关键字以字符表示，如"block"设备。 
    使用udev_enumrate_scan_xxx系列函数扫描/sys目录下，所有与过滤器匹配的设备。扫描完成后的数据结构是一个链表，使用udev_enumerate_get_list_entry获取链表的首个结点，使用udev_list_entry_foreach遍历整个链表。 
3. 监控设备插拔 udev的设备插拔基于netlink实现。 
    使用udev_monitor_new_from_netlink创建一个新的monitor，函数的第二个参数是事件源的名称，可选"kernel"或"udev"。基于"kernel"的事件通知要早于"udev"，但相关的设备结点未必创建完成，所以一般应用的设计要基于"udev"进行监控。 
    使用udev_monitor_filter_add_match_subsystem_devtype增加一个基于设备类型的udev事件过滤器，例如: "block"设备。 
    使用udev_monitor_enable_receiving启动监控过程。监控可以使用udev_monitor_get_fd获取一个文件描述符，基于返回的fd可以执行poll操作，简化程序设计。 
    插拔事件到达后，可以使用udev_monitor_receive_device获取产生事件的设备映射。调用udev_device_get_action可以获得一个字符串："add"或者"remove"，以及"change", "online", "offline"等，但后三个未知什么情况下会产生。
4. 获取设备信息
    使用udev_list_entry_get_name可以得到一个设备结点的sys路径，基于这个路径使用udev_device_new_from_syspath可以创建一个udev设备的映射，用于获取设备属性。获取设备属性使用udev_device_get_properties_list_entry，返回一个存储了设备所有属性信息的链表，使用udev_list_entry_foreach遍历链表，使用udev_list_entry_get_name和udev_list_entry_get_value获取属性的名称和值。
    

[libudev and Sysfs Tutorial](https://blog.csdn.net/fjb2080/article/details/7528894)
[Libudev和sysfs指南](https://blog.csdn.net/zhaoxiaoqiang10_/article/details/54924770)
1.       初始化库，获取一个struct udev句柄
2.       枚举设备
3.       对找到的匹配设备输出它的节点名称，找到实际USB设备的起始节点，打印出USB设备的IDs和序列号等，最后解引用设备对象
4.       解引用枚举对象
5.       解引用udev对象    
gcc -Wall -g -o udev_example udev_example.c -ludev
所有从sysfs返回的字符串都是UTF-8格式的，若当做ASCII码会出错
```cpp
#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

int main (void)
{
 struct udev *udev;
 struct udev_enumerate *enumerate;
 struct udev_list_entry *devices, *dev_list_entry;
 struct udev_device *dev;

 /* Create the udev object */
 udev = udev_new();
 if (!udev) {
  printf("Can't create udev\n");
  exit(1);

 }

 /* Create a list of the devices in the 'hidraw' subsystem. */
 enumerate = udev_enumerate_new(udev);
 udev_enumerate_add_match_subsystem(enumerate, "hidraw");
 udev_enumerate_scan_devices(enumerate);
 devices = udev_enumerate_get_list_entry(enumerate);

 /* For each item enumerated, print out its information.
    udev_list_entry_foreach is a macro which expands to
    a loop. The loop will be executed for each member in
    devices, setting dev_list_entry to a list entry
    which contains the device's path in /sys. */

 udev_list_entry_foreach(dev_list_entry, devices){
  const char *path;

  /* Get the filename of the /sys entry for the device and create a udev_device object (dev) representing it */
  path = udev_list_entry_get_name(dev_list_entry);
  dev = udev_device_new_from_syspath(udev, path);

  /* usb_device_get_devnode() returns the path to the device node itself in /dev. */
  printf("Device Node Path: %s\n", udev_device_get_devnode(dev));


  /* The device pointed to by dev contains information about
     the hidraw device. In order to get information about the
     USB device, get the parent device with the
     subsystem/devtype pair of "usb"/"usb_device". This will
     be several levels up the tree, but the function will find
     it.*/

  dev = udev_device_get_parent_with_subsystem_devtype(
         dev,
         "usb",
         "usb_device");

  if (!dev) {
   printf("Unable to find parent usb device.");
   exit(1);
  }

  /* From here, we can call get_sysattr_value() for each file
     in the device's /sys entry. The strings passed into these
     functions (idProduct, idVendor, serial, etc.) correspond
     directly to the files in the directory which represents
     the USB device. Note that USB strings are Unicode, UCS2
     encoded, but the strings returned from
     udev_device_get_sysattr_value() are UTF-8 encoded. */

  printf(" VID/PID: %s %s\n",udev_device_get_sysattr_value(dev,"idVendor"),udev_device_get_sysattr_value(dev, "idProduct"));
  printf(" %s\n %s\n",udev_device_get_sysattr_value(dev,"manufacturer"),udev_device_get_sysattr_value(dev,"product"));
  printf(" serial: %s\n",udev_device_get_sysattr_value(dev, "serial"));

  udev_device_unref(dev);
 }

 /* Free the enumerator object */
 udev_enumerate_unref(enumerate);
 udev_unref(udev);
 return 0; 
}
```


Libudev-监视接口(Monitoring Interface)

Libudev同时提供了监视接口，当设备的状态改变时，监视接口可以向应用程序报告发生的事件，当设备加入系统或从系统移除时可以接到通知，这非常有用。就像上面例子中的枚举接口一样，监视接口也有过滤匹配机制，应用程序只需要接受它关心的事件。如当将“hidraw”加入过滤器中，只有关于hidraw的事件才会传递给应用程序。当一个设备改变状态时，函数udev_monitor_receive_device()函数返回一个关于udev_device的句柄，表示发生的事件。通过函数udev_device_get_action()可以查询发生的具体动作，函数的返回字符串如下：

add:设备连接到系统
remove:设备从系统断开连接
change:设备的状态改变
move：设备节点被移动、复制或在重新指定了父节点
函数udev_monitor_receive_device()是阻塞函数，当程序执行到这个函数时将阻塞直到有事件发生才返回。在某些情况下这是不方便的，所以udev_monitor对象提供了一个文件描述符给系统调用select()，select系统调用提供了udev_monitor_receive_device()非阻塞的用法。
下面的例子演示了udev的监视接口用法，程序执行一个循环查询select()函数等待事件的发生。若有事件发生将调用udev_monitor_receive_device()函数接受事件并打印出来。在循环中每次调用sleep()睡眠250毫秒。

/* Set up a monitor to monitor hidraw devices */
 mon = udev_monitor_new_from_netlink(udev, "udev");
 udev_monitor_filter_add_match_subsystem_devtype(mon, "hidraw", NULL);
 udev_monitor_enable_receiving(mon);
 /* Get the file descriptor (fd) for the monitor.
    This fd will get passed to select() */
 fd = udev_monitor_get_fd(mon);
 
 /* This section will run continuously, calling usleep() at
    the end of each pass. This is to demonstrate how to use
    a udev_monitor in a non-blocking way. */
 while (1) {
  /* Set up the call to select(). In this case, select() will
     only operate on a single file descriptor, the one
     associated with our udev_monitor. Note that the timeval
     object is set to 0, which will cause select() to not
     block. */
  fd_set fds;
  struct timeval tv;
  int ret;
  
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  
  ret = select(fd+1, &fds, NULL, NULL, &tv);
  
  /* Check if our file descriptor has received data. */
  if (ret > 0 && FD_ISSET(fd, &fds)) {
   printf("\nselect() says there should be data\n");
   
   /* Make the call to receive the device.
      select() ensured that this will not block. */
   dev = udev_monitor_receive_device(mon);
   if (dev) {
    printf("Got Device\n");
    printf(" Node: %s\n", udev_device_get_devnode(dev));
    printf(" Subsystem: %s\n", udev_device_get_subsystem(dev));
    printf(" Devtype: %s\n", udev_device_get_devtype(dev));

    printf(" Action: %s\n",udev_device_get_action(dev));
    udev_device_unref(dev);
   }
   else {
    printf("No Device from receive_device(). An error occured.\n");
   } 
  }
  usleep(250*1000);
  printf(".");
  fflush(stdout);
 }