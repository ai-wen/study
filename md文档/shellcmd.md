# 无桌面程序通过XWinodw 关闭显示器电源管理功能
export DISPLAY=:0
xset s 0 #关闭屏幕保护
xset -dpms #关闭电源管理


# 查看进程树，并显示PID
pstree -p

# 通过PID查看对应进程的cpu内存使用情况
top -p100

# 通过端口或进程名查看进程网络运行状态
netstat -anop |grep "Xorg"
netstat -anop |grep "80"


# ssh连接
apt-get install openssh-server
openssh-sftp-server

openssh 依赖sftp-server

ssh lm@192.168.4.79 -p 22
ssh lm@192.168.4.66 -p 22
ssh lm@192.168.4.58 -p 22
ssh lm@192.168.4.61 -p 22

ssh传输文件
scp lm@192.168.4.66:/home/lm/LMTool-20201210.deb  /home/lm
scp /home/lm/LMTool-20201210.deb  lm@192.168.4.79:/home/lm/


# 查看进程使用情况
pstree -p |grep "LMSuperTool"
-lmtool.sh(503)-+-LMSuperTool(1051)-
top -p1051
    
top - (系统时间) 14:46:01(运行时间)up 6 min,  (当前登录用户)1 user,  (负载均衡1分钟、5分钟、15分钟的负载情况)load average: 0.08, 0.63, 0.40
Tasks:   (总进程数)1 total,  (运行数) 0 running,   (休眠数)1 sleeping,   (停止数)0 stopped,   (僵尸数)0 zombie
%Cpu(s):  (用户空间占用CPU的百分比)0.2 us,  (内核空间占用CPU的百分比)0.3 sy,  (改变过优先级的进程占用CPU的百分比)0.0 ni, (空闲CPU百分比)99.4 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
KiB Mem :  (总内存)3959740 total,  3524420 free,   (使用内存)133076 used,   302244 buff/cache
KiB Swap: (swap交换分区信息)       0 total,        0 free,        0 used.  3572796 avail Mem

  PID USER      PR  NI    VIRT    RES    SHR S  %CPU %MEM(进程使用的物理内存百分比)     TIME+ COMMAND
 1051 root      20   0 2212644 102152  74664 S   0.3  2.6   0:00.85 LMSuperTool




# 内存泄漏检测
apt-get install valgrind
Valgrind工具包包含多个工具，如Memcheck,Cachegrind,Helgrind, Callgrind，Massif。
#检查内存错误
valgrind --leak-check=full --show-reachable=yes  ./a.out
--leak-check=full指的是完全检查内存泄漏，
--show-reachable=yes是显示内存泄漏的地点，
--trace-children=yes是跟入子进程。
程序是会正常退出的程序，那么当程序退出的时候valgrind自然会输出内存泄漏的信息。
如果程序是个守护进程，那么也不要紧，我们 只要在别的终端下杀死memcheck进程（因为valgrind默认使用memcheck工具，就是默认参数—tools=memcheck）：killall memcheck

#4种内存泄漏
由局部变量指向的内存,如果不释放为肯定丢失, 由此指针而引起的后续内存泄露,为间接丢失. 
//如果不free, 将会有 (肯定)definitely lost内存泄露  //如果不free, 将会有 (间接)indirectly lost 内存泄露 

由全局变量指向的内存如果不被释放,为still reachable,如果该变量改动过, 为可能丢失.
//如果不free, 将会有 still reachable 内存泄露  //如果不free, 将会有 possibly lost 内存泄露 



#检查代码覆盖和性能瓶颈
valgrind --tool=callgrind ./sec_infod
会在当前路径下生成callgrind.out.pid（当前生产的是callgrind.out.19689），如果我们想结束程序，可以：killall callgrind
然后我们看一下结果：
callgrind_annotate --auto=yes callgrind.out.19689   >log



# awk 使用
#printf("%s ") 注意加空格
for libs in `ldd LMSuperTool | awk  '{if (match($3,"/")){ printf("%s "),$3 } }'`
do   
   so=`readlink -f $libs`
   echo $so
   #cp $so ./  
done

NIC=''
for NICVAR in `cat /proc/net/dev | awk '{i++; if(i>2){print $1}}' | sed 's/^[\t]*//g' | sed 's/[:]*$//g'`
do        
    IPVAR=`ifconfig $NICVAR | grep "inet " | awk -F: '{print $2}' | awk '{print $1}'`            
    if [ -z ${IPVAR} ] || [ ${IPVAR} = '127.0.0.1' ] || [ ${NICVAR} = 'n2n0' ];then
        echo ""
    else
        #echo $NICVAR
        #echo $IPVAR
        NIC=$NICVAR
    fi        
done