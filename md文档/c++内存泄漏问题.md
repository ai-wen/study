apt-get install valgrind
valgrind --leak-check=full --show-leak-kinds=all --show-reachable=yes --trace-children=yes ./test

#include <string>
#include <stdlib.h>
int main()
{
    {
/*
valgrind 检测以下c++代码会存在内存泄漏。
实际上这是由于 C++ 在分配内存时，为了提高效率，使用了它自己的内存池。
当程序终止时，内存池的内存才会被操作系统回收，所以 Valgrind 会将这部分内存报告为 reachable 的，需要注意，reachable 的内存不代表内存泄漏。
==112519== 72,704 bytes in 1 blocks are still reachable in loss record 1 of 1
==112519==    at 0x4C2DB8F: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==112519==    by 0x4EC3EFF: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.21)
==112519==    by 0x40106F9: call_init.part.0 (dl-init.c:72)
==112519==    by 0x401080A: call_init (dl-init.c:30)
==112519==    by 0x401080A: _dl_init (dl-init.c:120)
==112519==    by 0x4000C69: ??? (in /lib/x86_64-linux-gnu/ld-2.23.so)
*/
        std::string* ptr = new std::string("Hello, World!");
        delete ptr;

        char* pstr = new char[100];
        delete[] pstr;

    }

    {    
        char* p =( char*) malloc(32);    
        free(p);
    }

    return 0;
}
