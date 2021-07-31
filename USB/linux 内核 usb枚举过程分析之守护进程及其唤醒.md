usb热插拔，即usb设备可以实现即插即用，像U盘一样，插到电脑里就可以用，不用时可以直接拔除，这个动作不会影响USB设备使用性能。

在linx 系统中，usb热插拔由两部分方面共同实现，即内核空间和用户空间，内核由一个守护进程实现，用户空间由udev 程序实现。在内核空间里，有一个专门用于监控usb hub的状态的守护进程，守护进程通过等待队列实现，等待队列平时处理休眠状态，当usb hub上状态变化时(即有usb设备从usb hub上插入或拔出)时，便会去唤醒等待队列，然后去实现usb设备枚举，枚举成功后，向linux系统注册usb设备，并通过kobject_event通知用户空间，有设备插入或拔出，用户空间里有一个专门用于实现动态加载设备节点的程序udev,当它收到内核通知后，能够动态创建usb设备节点，这样便实现了usb的热插拔。

本文主要从usb设备枚举最基本的几个方面进行讲解，即usb守护进程、守护进程如何唤醒、被谁唤醒。

# 一.守护进程
在linux系统中，usb是一个相对比较复杂的子系统，所以usb子系统的初始化过程 也相对复杂，涉及了多个方面：usb总线初始化、usb文件系统初始化、usb hub初始化、usb设备驱动注册等。其中，在usb hub初始化usb_hub_init过程中，它除了向系统注册usb hub驱动处，还创建了一个用于监控usb root hub状态的守护进程hub_thread.
hub_thread由kthread_run创建并唤醒：

```cpp
khubd_task = kthread_run(hub_thread, NULL, "khubd");  
    if (!IS_ERR(khubd_task))  
        return 0;  


static int hub_thread(void *__unused)  
{  
    set_freezable();  
  
    do {  
        hub_events();  
        wait_event_freezable(khubd_wait,  
                !list_empty(&hub_event_list) ||  
                kthread_should_stop());  
    } while (!kthread_should_stop() || !list_empty(&hub_event_list));  
  
    pr_debug("%s: khubd exiting\n", usbcore_name);  
    return 0;  
}  
```
第3行的set_freezable作用是清除当前线程标志flags中的PF_NOFREEZE位，表示当前线程能进入挂起或休眠状态。
接下来就是一个do...while的死循环，循环结束的条件是当前线程收到stop请求并且hub_event_list列表里为空，守护进程由kthread_run创建并唤醒，当调用kthread_stop时，就会停止该线程；只有收到kthread_stop且hub_event_list列表为空时才会跳出do...while循环。
在do...while循环中只有两行代码，hub_events为usb系统中最核心部分，这里只要hub_event_list里非空就会运行USB的枚举过程，直到hub_event_list为空，则跳出hub_events,通过wait_event_freezable进入休眠，其中khubd_wait为一个等待队列头，它通过静态方式 定义：

static DECLARE_WAIT_QUEUE_HEAD(khubd_wait);  
wait_event_freezable中第二个参数为等待队列进入休眠条件condition，只有第二个参数为假时才能进入休眠，即hub_event_list列表为空且没有收到stop请求，这里hub_event_list肯定为空，所以只要没有收到kthread_stop请求，就能进入休眠，休眠被唤醒后检测condition时否已经满足条件，即condition为真，hub_event_list不为空或收到stop请求，如果是收到stop请求，则直接退出do...while循环，如果是因为hub_event_list非空，则运行hub_events，实现usb枚举。

# 二 .守护进程唤醒
当运行完hub_events后，usb线程就会通过wait_event_freezable进入休眠状态，直到被信号中断或条件满足被唤醒，usb的守护进程由kick_khubd函数进行唤醒，该函数在多个地方被调用：
```cpp
static void kick_khubd(struct usb_hub *hub)  
{  
    unsigned long   flags;  
  
    spin_lock_irqsave(&hub_event_lock, flags);  
    if (!hub->disconnected && list_empty(&hub->event_list)) {  
        list_add_tail(&hub->event_list, &hub_event_list);  
  
        /* Suppress autosuspend until khubd runs */  
        usb_autopm_get_interface_no_resume(  
                to_usb_interface(hub->intfdev));  
        wake_up(&khubd_wait);  
    }  
    spin_unlock_irqrestore(&hub_event_lock, flags);  
}  
```
第6行，只有在hub连接并且hub中的event_list为初始状态；
第7行，将hub的event_list添加到hub_event_list列表，用于满足守护进程被唤醒时的条件。
第10行，自动挂载计数加1，停止hub在进行枚举的时候进入suspend态。
第12行，通过wake_up唤醒已经休眠的usb守护进程。
有多个地方可以调用kick_khubd来唤醒usb守护进程： 

hub有两个端口，即control endpoint和interrupt endpoint，其中interrupt endpoint主要用于查询hub 上的port状态变化。在添加控制器驱动时会创建一个root hub，而当root hub的port上检测到有hub插入时，也会创建一个usb hub，在为root hub或普通hub进行配置时，会为它申请的interrupt endpoint申请urb相关资源：
```cpp
hub->urb = usb_alloc_urb(0, GFP_KERNEL);  
if (!hub->urb) {  
    ret = -ENOMEM;  
    goto fail;  
}  
usb_fill_int_urb(hub->urb, hdev, pipe, *hub->buffer, maxp, hub_irq, hub, endpoint->bInterval);  
urb的回调函数为hub_irq:

static void hub_irq(struct urb *urb)  
{  
    struct usb_hub *hub = urb->context;  
    int status = urb->status;  
    unsigned i;  
    unsigned long bits;  
  
    switch (status) {  
    case -ENOENT:       /* synchronous unlink */  
    case -ECONNRESET:   /* async unlink */  
    case -ESHUTDOWN:    /* hardware going away */  
        return;  
  
    default:        /* presumably an error */  
        /* Cause a hub reset after 10 consecutive errors */  
        dev_dbg (hub->intfdev, "transfer --> %d\n", status);  
        if ((++hub->nerrors < 10) || hub->error)  
            goto resubmit;  
        hub->error = status;  
        /* FALL THROUGH */  
  
    /* let khubd handle things */  
    case 0:         /* we got data:  port status changed */  
        bits = 0;  
        for (i = 0; i < urb->actual_length; ++i)  
            bits |= ((unsigned long) ((*hub->buffer)[i]))  
                    << (i*8);  
        hub->event_bits[0] = bits;  
        break;  
    }  
  
    hub->nerrors = 0;  
  
    /* Something happened, let khubd figure it out */  
    kick_khubd(hub);  
  
resubmit:  
    if (hub->quiescing)  
        return;  
  
    if ((status = usb_submit_urb (hub->urb, GFP_ATOMIC)) != 0  
            && status != -ENODEV && status != -EPERM)  
        dev_err (hub->intfdev, "resubmit --> %d\n", status);  
}  
```
在hub_irq中可以看到，如果urb被成功提交给控制器，并成功获取port状态，就会调用kick_khubd唤醒守护进程；
对于ohci类型的root hub，它除了和其它hub一样，可以通过usb_submit_urb提交查询hub状态的请求，它还可以通过中断方式去查询，在用usb_add_hcd添加控制器驱动时，会为控制器申请中断：

retval = usb_hcd_request_irqs(hcd, irqnum, irqflags);  
if (retval)  
    goto err_request_irq;  
对于ohci类型的控制器，在中断处理程序ohci_irq中，其中有一项就是用来监控root hub上的port状态的：

if (ints & OHCI_INTR_RHSC) {  
    ohci_vdbg(ohci, "rhsc\n");  
    ohci->next_statechange = jiffies + STATECHANGE_DELAY;  
    ohci_writel(ohci, OHCI_INTR_RD | OHCI_INTR_RHSC, ®s->intrstatus);  
    ohci_writel(ohci, OHCI_INTR_RHSC, ®s->intrdisable);  
    usb_hcd_poll_rh_status(hcd);  
}  
当port上状态发生变化时，就会调用usb_hcd_poll_rh_status去查询usb root hub port状态，并调用hub中interrupt urb的回调函数hub_irq，最终去唤醒usb守护进程。
对于root hub，当它使用usb_submit_urb去提交查询hub状态请求时，它是通过启动一个定时器rh_timer去定时查询root hub状态的，如果成功获取hub状态，并通过hub_irq返回，则在hub_irq最后会再次自动调用usb_submit_urb去获取hub状态。如果不能获取或不能成功通过hub_irq返回，则不会再去调用usb_submit_urb；
对于root hub可以通过主动去调用usb_submit_urb来查询port状态或通过控制器的中断来处理，除此之外，在向系统添加控制器驱动时，会自动创建并注册一个root hub，驱动将会对这个root hub匹配hub驱动并对它进行配置，配置完后它会主动的调用kick_khubd去唤醒守护进程(hub_activate)。
如果在枚举的时候发现插入的是一个usb hub，则也会为这个hub匹配其驱动，如果成功匹配驱动，便会对hub进行配置，配置成功后也会去调用kick_khubd去唤醒守护进程。