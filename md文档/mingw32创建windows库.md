gcc dlltest.c -shared -o dlltest.dll -Wl,--output-def,dlltest.def,--out-implib,dlltest.lib
-shared --告诉gcc hello.c 文件需要编译成动态库
-Wl 后面是传递给ld链接的参数
.def 导出模块文件
--out-implib,dlltest.lib表示让ld生成一个windows的dll链接符号文件



ar rcs  libxxx.a xx1.o xx2.o
创建静态库.a文件。
参数r：在库中插入模块(替换)。当插入的模块名已经在库中存在，则替换同名的模块。如果若干模块中有一个模块在库中不存在，ar显示一个错误消息，并不替换其他同名模块。默认的情况下，新的成员增加在库的结尾处，可以使用其他任选项来改变增加的位置。【1】
参数c：创建一个库。不管库是否存在，都将创建。
参数s：创建目标文件索引，这在创建较大的库时能加快时间。（补充：如果不需要创建索引，可改成大写S参数；如果.a文件缺少索引，可以使用ranlib命令添加