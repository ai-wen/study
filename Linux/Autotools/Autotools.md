#   Autotools比较常见的有autoconf、automake、libtool等包，每个包又有多个工具。

#   自动生成configure
    autoscan自动生成configure.scan==(configure.ac)
    autoconf使用configure.ac(宏AM_INIT_AUTOMAKE)生成configure

#   自动生成Makefile
    automake使用makefile.am生成makefile.in
    configure使用aclocal收集makefile.amconfigure.ac的所有宏生成aclocal.m4
    configure使用libtool通过对生成的动态库进行抽象，统一生成.la的形式，可以支持十几种各种不同的平台。

configure依赖makefile.in和aclocal.m4生成Makefile

#   自动生成config.h
    configure使用autoheader工具，用来将configure.ac里面的宏配置转换为我们C语言格式的#define形式，并保存在config.h.in文件里，当我们运行./configure生成makefile的时候，顺便也会将config.h.in转换为config.h文件


1. autoscan
    autoscan是用来扫描源代码目录生成configure.scan文件的
    .autoscan可以用目录名做为参数,但如果你不使用参数的话,那么autoscan将认为使用的是当前目录.
    autoscan将扫描你所指定目录中的源文件,并创建configure.scan文件.
　　configure.scan包含了系统配置的基本选项,里面都是一些宏定义.我们需要将它改名为configure.in
2. aclocal
　　aclocal是一个perl脚本程序.aclocal根据configure.in文件的内容,自动生成aclocal.m4文件
    .aclocal的定义是："aclocal-createaclocal.m4byscanningconfigure.ac".
3. autoconf
    autoconf需要GNU m4宏处理器来处理aclocal.m4,生成configure脚本.
    autoconf是用来产生configure文件的.configure是一个脚本,它能设置源程序来适应各种不同的操作系统平台,并且根据不同的系统来产生合适的Makefile,从而可以使你的源代码能在不同的操作系统平台上被编译出来.
　　configure.in文件的内容是一些宏,这些宏经过autoconf处理后会变成检查系统特性.环境变量.软件必须的参数的shell脚本.configure.in文件中的宏的顺序并没有规定,但是你必须在所有宏的最前面和最后面分别加上AC_INIT宏和AC_OUTPUT宏.
4. Makefile.am
    Makefile.am是用来生成Makefile.in的,需要你手工书写
    AUTOMAKE_OPTIONS=foreign
    bin_PROGRAMS=helloworld
    helloworld_SOURCES=helloworld.c
    AUTOMAKE_OPTIONS    这个是 automake的选项.
    在执行automake时,它会检查目录下是否存在标准GNU软件包中应具备的各种文件,例如AUTHORS.ChangeLog.NEWS等文件.
    我们将其设置成foreign时,automake会改用一般软件包的标准来检查.
    bin_PROGRAMS
　　这个是指定我们所要产生的可执行文件的文件名.如果你要产生多个可执行文件,那么在各个名字间用空格隔开.
    helloworld_SOURCES
　　这个是指定产生"helloworld"时所需要的源代码.如果它用到了多个源文件,那么请使用空格符号将它们隔开.比如需要helloworld.h,helloworld.c那么请写成:
    helloworld_SOURCES=helloworld.h helloworld.c.
    
　　如果你在bin_PROGRAMS定义了多个可执行文件,则对应每个可执行文件都要定义相对的filename_SOURCES.

5. automake
　　我们使用automake--add-missing来产生Makefile.in.
　　选项--add-missing的定义是"addmissingstandardfilestopackage",它会让automake加入一个标准的软件包所必须的一些文件.
　　我们用automake产生出来的Makefile.in文件是符合GNUMakefile惯例的,接下来我们只要执行configure这个shell脚本就可以产生合适的Makefile文件了.

![自动化构建Makefile](1.png)