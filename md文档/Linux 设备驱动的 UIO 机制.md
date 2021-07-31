# 设备驱动
硬件设备可以根据功能分为网络设备，块设备，字符设备，或者根据与CPU相连的方式分为PCI设备，USB设备等。
设备驱动的主要任务有两个：
1. 存取设备的内存
2. 处理设备产生的中断

# Linux 设备驱动的 UIO（Userspace I/O） 机制,运行在用户空间的I/O技术

第一个任务，UIO 核心实现了mmap()可以处理物理内存(physical memory)，逻辑内存(logical memory)，虚拟内存(virtual memory)。UIO驱动的编写是就不需要再考虑这些繁琐的细节。
第二个任务，对于设备中断的应答必须在内核空间进行。所以在内核空间有一小部分代码用来应答中断和禁止中断，但是其余的工作全部留给用户空间处理。

## UIO 其它机制
- 如果用户空间要等待一个设备中断，它只需要简单的阻塞在对 /dev/uioX的read()操作上。当设备产生中断时，read()操作立即返回。
- UIO 也实现了poll()系统调用，你可以使用select()来等待中断的发生。select()有一个超时参数可以用来实现有限时间内等待中断。

- 对设备的控制还可以通过/sys/class/uio下的各个文件的读写来完成。你注册的uio设备将会出现在该目录下。
例如:uio0映射的设备内存文件出现在 /sys/class/uio/uio0/maps/mapX，对该文件的读写就是对设备内存的读写。

- uio可以使用C++，Java ...等高级语言极大的方便了开发。
  

-------------------------------------------------
        read/mmap   用户空间
-------------------------------------------------
sysfs   /dev/uioX   系统接口层   
-------------------------------------------------
UIO框架--驱动        内核层
-------------------------------------------------
sysfs下uio相关的文件结构如下
sys  
├───uio  
    ├───uio0  
    │     ├───maps  
    │          ├───mapX  
    ├───uio1  
            ├───maps                           
            │    ├───mapX        
            ├───portio  
                ├───portX  


## UIO内核空间功能(即驱动的功能)
UIO框架下运行在内核空间的驱动程序所做的工作很简单,常做的只有两个:
（1）分配和记录设备需要的资源和注册uio设备
在设备的probe探测函数中：
- 使能PCI 设备
- 申请资源
- 读取并记录配置信息
- 注册uio设备// uio_register_device()
// uio_8139d_pci_probe & uio_8139d_handler

```cpp
1、 函数： static int __init uio_init(void)
功能：申请字符设备号，设备，并注册到系统中，注册uio_class到系统中
调用模块：init_uio_class()
执行流程：
申请字符设备号，设备，并注册到系统中，注册uio_class到系统中 //init_uio_class
//创建”/sys/class/uio”

2、函数：uio_exit
功能：注销uio_class,注销字符设备编号，删除设备
调用模块：release_uio_class
执行流程：
注销uio_class,注销字符设备编号，删除设备 //release_uio_class

3、函数：static void release_uio_class(void)
功能：注销uio_class,注销字符设备编号，删除设备
执行流程：
注销uio_class//class_unregister
注销字符设备编号，删除设备 //uio_major_cleanup

4、函数：static int init_uio_class(void)
功能：申请字符设备号，设备，并注册到系统中，注册uio_class到系统中
调用模块： uio_major_init()
class_register()
执行流程：
申请字符设备编号，设备，并初始化//uio_major_init
注册class 类型全局变量uio_class到系统//class_register
//ls -l /sys/class 查看

5、函数： static int uio_major_init(void)
功能：申请字符设备编号，设备，并初始化
调用模块：
alloc_chrdev_region()
cdev_alloc()
kobject_set_name()
cdev_add()
执行流程：
申请字符设备编号(多个)//alloc_chrdev_region
//2^UIO_MAX_DEVICES个从设备
//设备的名字为”uio”
分配一个表示字符设备的cdev结构//cdev_alloc
初始化cdev结构的file_operations类型字段//控制cdev设备的各种操作，
// 如 open, close, read, write…
设置cdev结构的kobj字段的name为uio //kobject_set_name
添加字符设备到系统中 //cdev_add，调用成功后，我们的设备就“活了”
// cat /proc/devices ，可以查看到分配到主设备号
保存主设备号到全局变量uio_major
保存设备指针到全局变量uio_cdev
返回

6、函数：static void uio_major_cleanup(void)
功能：注销字符设备编号，删除设备
调用模块：unregister_chrdev_region
执行流程：
注销字符设备编号//unregister_chrdev_region
删除设备uio_cdev //cdev_del
file_operations

7、 函数：static int uio_open(struct inode *inode, struct file *filep)
参数：inode:
filep:
功能：获得和次设备号关联的uio_device指针，创建一个辅助变量listener, 并调用info指向的uio_info结构中的open方法
执行流程：
获得保护uio_idr的锁 //mutex_lock
从inode 结构中获取次编号 //iminor
获得和次编号关联的uio_device指针 //idr_find 在那里进行地设置呢？？？
// 在 uio_get_minor 中分配的次设备编号并设置的关联
放弃锁 //mutex_unlock
增加uio_device类型指针指向的模块的引用计数 //try_module_get
分配一个uio_listener类型的listener //kmalloc
关联listener和 uio_device 指针
获得uio_device 指向设备的事件计数值，并存入listener //atomic_read
把listener指针保存到filep->private_data字段
调用uio_device的info字段指向的uio_info中的open方法//*

8、函数：static int uio_release(struct inode *inode, struct file *filep)
功能：从而调用uio_device的字段info指向的uio_info中的release方法
释放辅助结构体listener
执行流程：
从filep->private_data中获得uio_open中保存的listener指针。
利用listener指针找到指向uio_device类型结构指针
从而调用uio_device的字段info指向的uio_info中的release方法。
减少uio_device类型指针指向的模块的引用计数//module_put
释放listener结构体 //kfree

9、 函数：static int uio_fasync(int fd, struct file *filep, int on)
参数：
on : 0, 删除；非零，添加
功能： 管理uio_device的async_queue
调用模块：fasync_helper()
执行流程：
从filep->private_data中获得uio_open中保存的listener指针。
利用listener指针找到指向uio_device类型结构指针
设置uio_device的async_queue//fasync_helper

10、函数：static unsigned int uio_poll(struct file *filep, poll_table *wait)
功能： 使进程在传递到该系统调用的所有文件描述符对应的等待队列上等待，并返回一个是否可以立即无阻塞执行的位掩码
执行流程：
从filep->private_data中获得uio_open中保存的listener指针。
利用listener指针找到指向uio_device类型结构指针
判断用uio_device类型指针的info字段(uio_info类型)的irq成员不为0，则继续，
否则，返回IO错误
向poll_table类型的wait表中添加uio_device类型指针指向结构的wait等待队列//poll_wait
//!!!! 注意poll_wait并不阻塞
如果listener中的事件计数值event_count和uio_device的
事件计数值count不一致时// uio_interrupt调用了uio_event_notify对
//中断事件计数器增一
返回“通常”的数据可读的位掩码

11、函数：static ssize_t uio_read(struct file *filep, char __user *buf,size_t count, loff_t *ppos)
功能：复制uio设备中断事件计数器的值到用户空间
执行流程：
从filep->private_data中获得uio_open中保存的listener指针
利用listener指针找到指向uio_device类型结构指针
创建一个等待队列的项 //DECLARE_WAITQUEUE
检查确认uio设备的设备info的中断号(0)不为零
添加本进程到uio设备的等待队列wait上 // add_wait_queue
//由uio_interrupt调用uio_event_notify唤醒
REP： 设置当前进程的 “可中断标志”
检查是否有中断事件发生，
如果有(listener中的中断事件计数值event_count)和uio设备中的中断事件
计数器值不一致)，则将设备中断计数器的值复制到用户空间
并将listener中的中断事件计数值更新为设备的中断事件计数值
把当前进程设置为TASK_RUNNING状态，
并将当前进程从uio设备的等待队列wait上删除
如果文件读时设置了O_NONBLOCK标志，
那么，把当前进程设置为TASK_RUNNING状态，
并将当前进程从uio设备的等待队列wait上删除
返回 -EAGAIN
检查当前进程是否有信号处理 //signal_pending
//http://blog.chinaunix.net/space.php?uid=20746501&do=blog&cuid=1820175
如有，把当前进程设置为TASK_RUNNING状态，
并将当前进程从uio设备的等待队列wait上删除
并返回 -ERESTARTSYS
执行调度 //schedule
JMP REP

12、uio_register_device
功能： 调用uio_info中注册的handler中断处理函数，对设备的中断事件计数器增一并通知各读进程，有数据可读
执行流程：
从filep->private_data中获得uio_open中保存的listener指针
调用 uio_device类型指针的info字段(uio_info类型)的handler
如果属于本设备的中断，并且在handler中已经处理过
那么对设备的中断事件计数器增一，
并通知各读进程，有数据可读 //uio_event_notify

13、函数：void uio_event_notify(struct uio_info *info)
功能：“触发“ 一个中断事件，对设备的中断事件计数器增一，并通知各读进程，有数据可读
执行流程：
从filep->private_data中获得uio_open中保存的listener指针
对中断事件计数器增一
唤醒阻塞在设备等待队列wait上的读进程 //wake_up_interruptible
// 该队列上的进程在uio_read中添加
向异步等待队列async_queue发出可读信号 //kill_fasync

14、 函数：static ssize_t uio_write(struct file *filep, const char __user*buf,size_t count, loff_t *ppos)
功能： 读取用户空间的值，并调用uio_device注册的irqcontrol函数
执行流程：
从filep->private_data中获得uio_open中保存的listener指针
调用 uio_device类型指针的info字段(uio_info类型)的handler
检验info字段(uio_info类型)的中断号irq
读取从用户空间传过来的32位的值//copy_from_user
调用info字段(uio_info类型)的irqcontrol函数，将用户空间传递过来的32位值作为参数传入。

15、函数：static int uio_mmap(struct file *filep, struct vm_area_struct* vma)
执行流程：
从filep->private_data中获得uio_open中保存的listener指针调用 uio_device类型指针的info字段(uio_info类型)的handler保存uio_device类型指针到 vma 的vm_private_data返回映射区域的索引(比如 mapX，的X) //uio_find_mem_index
计算实际的页数和请求的页数
如果实际的页数小于请求的页数那么，返回-EINVAL
如果uio设备注册有mmap函数，那么就调用它
当内存区域的类型为UIO_MEM_PHYS时，//uio_mmap_physical
当内存区域的类型为UIO_MEM_LOGICAL、UIO_MEM_VIRTUAL时，
为虚拟内存区域设置操作，和告诉内存不要将该区域交换出去，访问计数器增一//uio_mmap_logical
```

（2）必须*在内核空间实现的小部分中断应答函数，经过实践表明这部分的工作也是可以省略的!
我们认为uio内核空间的程序所做的越少越好,在用户空间能完成的我们就不需要放在内核空间做(比如说响应中断),这样假如内核有变化,uio框架中的驱动维护也是比较简单的!
## UIO用户空间关键操作
```cpp
//响应硬件中断
int32_t irq_count;
int fd = open("/dev/uio0", O_RDWR);
//register映射内存文件 寄存器的读写操作，可用过普通内存读写的方式完成
void * access = mmap(NULL, 4096,PROT_READ | PROT_WRITE,MAP_SHARED, fd, 0);
while (read(fd, &irq_count, 4) == 4) 
{
    printf("Interrupt number %d\n", irq_count);
}
```



# 示例

- 映射了一块1024字节的逻辑内存。不申请中断。
- 加载上我们的驱动后，结果是将会在/sys/class/uio/uio0/maps/map0中看到addr，name, offset, size。他们分别是映射内存的起始地址, 映射内存的名字，起始地址的页内偏移, 映射内存的大小。 在用户空间部分，打开addr, size以便使用映射好的内存。

```cpp simple.c  
/**  
* Compile:    
*   # echo "obj-m := simple.o" > Makefile  
*   # make -Wall -C /lib/modules/`uname -r`/build M=`pwd` modules  
* Load the module:  
*   #modprobe uio  
*   #insmod simple.ko  
*/  

#include <linux/module.h>  
#include <linux/platform_device.h>  
#include <linux/uio_driver.h>  
#include <linux/slab.h> /* kmalloc, kfree */  

//一个uio驱动的注册过程简单点说有两个步骤：
//   1. 初始化设备相关的 uio_info结构。
//   2. 调用uio_register_device 分配并注册一个uio设备。

//uio驱动必须要提供并初始化的结构 uio_info, 它包含了您要注册的uio_device的重要特性。
struct uio_info kpart_info = {  
        .name = "kpart",  
        .version = "0.1",  
        .irq = UIO_IRQ_NONE,  //没有使用中断，所以初始为UIO_IRQ_NONE
};  
/*
当你没有实际的硬件设备，但是，还想产生中断的话，就可以把irq设置为UIO_IRQ_CUSTOM，
并初始化uio_info的handler字段，那么在产生中断时，你注册的中断处理函数将会被调用。
如果有实际的硬件设备，那么irq应该是您的硬件设备实际使用的中断号。
然后，在drv_kpart_probe函数(先不要管为什么在这个函数中进行这些操作)中，完成了
kpart_info.mem[0].addr， kpart_info.mem[0].memtype，kpart_info.mem[0].size字段的初始化。这是内存映射必须要设置的。
其中,  kpart_info.mem[0].memtype可以是 UIO_MEM_PHYS，那么被映射到用户空间的是你设备的物理内存。也可以是UIO_MEM_LOGICAL，那么被映射到用户空间的是逻辑内存(比如使用 kmalloc分配的内存)。还可以是UIO_MEM_VIRTUAL,那么被映射到用户空间的是虚拟内存(比如用vmalloc分配的内存).这样就完成了uio_info结构的初始化。
下一步，还是在drv_kpart_probe中，调用uio_register_device完成了uio设备向uio core的注册。
*/

static int drv_kpart_probe(struct device *dev);  
static int drv_kpart_remove(struct device *dev);  
static struct device_driver uio_dummy_driver = {  
        .name = "kpart",  
        .bus = &platform_bus_type,  
        .probe = drv_kpart_probe,  
        .remove = drv_kpart_remove,  
};  
  
static int drv_kpart_probe(struct device *dev)  
{  
        printk("drv_kpart_probe( %p)\n", dev);  
        kpart_info.mem[0].addr = (unsigned long)kmalloc(1024,GFP_KERNEL);  
  
        if(kpart_info.mem[0].addr == 0)  
                return -ENOMEM;  
        kpart_info.mem[0].memtype = UIO_MEM_LOGICAL;  
        kpart_info.mem[0].size = 1024;  
  
        if( uio_register_device(dev, &kpart_info))  
                return -ENODEV;  
        return 0;  
}  
  
static int drv_kpart_remove(struct device *dev)  
{  
    uio_unregister_device(&kpart_info);  
  
    return 0;  
}  
  
static struct platform_device * uio_dummy_device;  
  
static int __init uio_kpart_init(void)  
{  
/*
1 platform_device_register_simple的调用创建了一个简单的platform设备。
2注册device_driver类型的uio_dummy_driver变量到bus。
这里需要注意一个问题，就是 device_driver结构中的name为“kpart", 我们创建的platform设备名称也是"kpart"。
而且先创建platform设备，后注册驱动。这是因为，创建好设备后，注册驱动时，驱动依靠name来匹配设备。
之后 drv_kpart_probe就完成了uio_info的初始化和uio设备的注册。
*/
        uio_dummy_device = platform_device_register_simple("kpart", -1,  NULL, 0);  
  
        return driver_register(&uio_dummy_driver);  
}  
  
static void __exit uio_kpart_exit(void)  
{  
        platform_device_unregister(uio_dummy_device);  
        driver_unregister(&uio_dummy_driver);  
}  
  
module_init(uio_kpart_init);  
module_exit(uio_kpart_exit);  
  
MODULE_LICENSE("GPL");  
MODULE_AUTHOR("Benedikt Spranger");  
MODULE_DESCRIPTION("UIO dummy driver");  

```

```cpp 上层调用
#include <stdio.h>  
#include <fcntl.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/mman.h>  
#include <errno.h>  
  
#define UIO_DEV "/dev/uio0"     //假设设备为
#define UIO_ADDR "/sys/class/uio/uio0/maps/map0/addr"  
#define UIO_SIZE "/sys/class/uio/uio0/maps/map0/size"  
  
static char uio_addr_buf[16], uio_size_buf[16];  
  
int main(void)  
{  
  int uio_fd, addr_fd, size_fd;  
  int uio_size;  
  void* uio_addr, *access_address;  
   
  uio_fd = open(UIO_DEV, /*O_RDONLY*/O_RDWR);  
  addr_fd = open(UIO_ADDR, O_RDONLY);  
  size_fd = open(UIO_SIZE, O_RDONLY);  
  if( addr_fd < 0 || size_fd < 0 || uio_fd < 0) {  
       fprintf(stderr, "mmap: %s\n", strerror(errno));  
       exit(-1);  
  }  
  read(addr_fd, uio_addr_buf, sizeof(uio_addr_buf));  
  read(size_fd, uio_size_buf, sizeof(uio_size_buf));  
  uio_addr = (void*)strtoul(uio_addr_buf, NULL, 0);  
  uio_size = (int)strtol(uio_size_buf, NULL, 0);  
  
  access_address = mmap(NULL, uio_size, PROT_READ | PROT_WRITE,  
                     MAP_SHARED, uio_fd, 0);  
  if ( access_address == (void*) -1) {  
      fprintf(stderr, "mmap: %s\n", strerror(errno));  
      exit(-1);  
  }  
  printf("The device address %p (lenth %d)\n"  
         "can be accessed over\n"  
         "logical address %p\n", uio_addr, uio_size, access_address);  
 //读写操作  
/*  
 access_address = (void*)mremap(access_address, getpagesize(), uio_size + getpagesize() + 11111, MAP_SHARED);  
  
  if ( access_address == (void*) -1) {  
      fprintf(stderr, "mremmap: %s\n", strerror(errno));  
      exit(-1);  
  }   
  printf(">>>AFTER REMAP:"  
         "logical address %p\n", access_address);  
*/  
  return 0;  
}  


```