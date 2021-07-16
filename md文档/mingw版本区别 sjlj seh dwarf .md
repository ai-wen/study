
# 异常处理机制
Windows使用自己的称为结构化exception处理（SEH）的exception处理机制。 

GCC支持两种exception处理方法（EH）：
DWARF-2（DW2）EH ，它需要使用DWARF-2（或DWARF-3）debugging信息。 DW-2 EH可能导致可执行文件稍微膨胀，因为大量调用堆栈展开表必须包含在可执行文件中。
setjmp / longjmp（SJLJ）, 基于SJLJ的EH比DW2 EH要慢得多（当没有exception抛出时，甚至可以正常执行），但是可以在没有使用GCC编译或没有调用堆栈解除信息的代码上工作。

# MinGW-w64各版本 名词解释
posix: 启用了C++ 11 多线程特性
win32: 未启用 （从时间线上正在尝试也启用部分 Threading）

DWARF DWARF- 2（DW2）EH ，这需要使用DWARF-2（或DWARF-3）调试信息。 DW-2 EH可以导致可执行文件略显膨胀，因为大的调用堆栈解开表必须包含在可执行文件中。
DWARF （DW2，dwarf-2） – 只有32位可用 – 没有永久的运行时间开销 – 需要整个调用堆栈被启用，这意味着exception不能被抛出，例如Windows系统DLL。

setjmp / longjmp（SJLJ）。基于SJLJ的EH比DW2 EH慢得多（在没有异常时会惩罚甚至正常执行），但是可以在没有使用GCC编译的代码或没有调用堆栈的代码上工作。
SJLJ （setjmp / longjmp）： – 可用于32位和64位 – 不是“零成本”：即使不抛出exception，也会造成较小的性能损失（在exception大的代码中约为15％） – 允许exception遍历例如窗口callback

结构化异常处理（SEH） （Structured Exception Handling）Windows使用自己的异常处理机制。
SEH （零开销exception） – 将可用于64位GCC 4.8。



x86_64-posix-sjlj
x86_64-posix-seh
x86_64-win32-sjlj
x86_64-win32-seh
i686-posix-sjlj
i686-posix-dwarf
i686-win32-sjlj
i686-win32-dwarf

# 实践发现，gcc/g++ 不同的mingw版本编译出来的静态库不能混用，异常处理机制一致的才能编译成功


