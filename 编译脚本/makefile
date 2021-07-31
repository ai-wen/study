MKDIR        = mkdir -p
RM           = rm -f
CP           = cp -fp
ECHO         = echo
FIND         = find
MV           = mv -f
LN           = ln -s
MAKE		 = make -f 
ARFLAGS      = rv
STRIP        = strip

export CROSS =   
CC = $(CROSS)gcc
CXX = $(CROSS)g++ 
LD           = $(CROSS)ld
AR           = $(CROSS)ar


#--retain-symbols-file控制静态符号表.symtab;
#--version-script控制动态符号表.dynsym;
#-Wl,--soname='libgm3000.so.1'
#-lssl -lcrypto

#动态库的命名格式是libbar.so.x.y.z，最后一个z版本的变动一定是兼容的。y版本升级一般向前兼容。所以这个y和z不能写死。x版本变动一般是不兼容升级。
#realname 	libgm3000.1.0.so.1.0.0		库文件真正的实体
#soname		libgm3000.1.0.so.1			库文件被系统加载后的标识名，是ldd查看时真正的					
#linkname	libgm3000.1.0.so

#readelf -d 	查看库的soname
#ldconfig -n  dir 将dir目录下的库文件缓存在系统环境变量LD_LIBRARY_PATH，并自动生成对应的soname文件
#ldconfig -l libgm3000.1.0.so.1.0.0				自动生成soname文件
#ln -s libgm3000.1.0.so.1  libgm3000.1.0.so		生成linkname文件


ARCH = $(shell getconf LONG_BIT)
export PRJ_HOME=$(pwd)../
LIBSMAIN  +=  -L. -L$(CURDIR)


ifeq ($(shell uname),Darwin)

CFLAGS		 = -fPIC -shared -framework Foundation -framework DiskArbitration  -framework IOKit
LINKFLAGS	 = -Wl,-all_load -lgm_api -los  -lstdc++ -ldl -lpthread -lmk_lib -lapdu -ldevmgr -lgm_api  -lpolarssl libusb-1.0.a
MAIN	= libgm3000.1.0.dylib

ifeq (64,$(ARCH))	
all: 
	#$(ECHO) "x64"
	$(CP) libusb/x64/libusb-1.0.a  ./libusb-1.0.a
	test -d $(CURDIR)/libx64/release || mkdir -p $(CURDIR)/libx64/release	
	$(MAKE) $(PRJ_HOME)/prj_mac/apdu/makefile  DEBUG=0 OFFSET=-m64
	$(CC)	$^$(CFLAGS)	$(LIBSMAIN)	$(LINKFLAGS)	$(LINKSYS)	-o	$(MAIN)
	#$(STRIP) $(MAIN)
	$(CP) $(MAIN) $(CURDIR)/libx64/release
	
debug: 
	$(CP) libusb/x64/libusb-1.0.a-d  ./libusb-1.0.a
	test -d $(CURDIR)/libx64/debug || mkdir -p $(CURDIR)/libx64/debug
	$(MAKE) $(PRJ_HOME)/prj_mac/apdu/makefile DEBUG=1 OFFSET=-m64
	$(CC)	$^$(CFLAGS)	-g	-D_DEBUG	$(LIBSMAIN)	$(LINKFLAGS)	$(LINKSYS)	-o	$(MAIN)
	$(CP) $(MAIN) $(CURDIR)/libx64/debug
	
clean:	
	$(MAKE) $(PRJ_HOME)/prj_mac/apdu/makefile clean 
endif


else
	
CFLAGS		 = -fPIC -shared
LINKFLAGS	 = -Wl,--retain-symbols-file=export.sym -Wl,--version-script=export.map  -Wl,--whole-archive -lgm_api -Wl,--no-whole-archive -lmk_lib -lapdu -ldevmgr -los -lpolarssl  -lgm_api -lusb-1.0 -lrt -lpthread -lstdc++ -ldl
MAIN	= libgm3000.1.0.so

ifeq (1,$(UI))
    LINKFLAGS = -Wl,--retain-symbols-file=export.sym -Wl,--version-script=export.map  -Wl,--whole-archive -lgm_api -Wl,--no-whole-archive -lmk_lib -lapdu -ldevmgr -los -lpolarssl  -lgm_api -lVerifyFingerUI -lpthread -lX11 -lusb-1.0 -lrt -lpthread -lstdc++ -ldl
	UI = 1
else
	UI = 0
endif


ifeq (64,$(ARCH))
	
all: 
	#$(ECHO) "x64"
	$(CP) libusb/x64/libusb-1.0.a  ./libusb-1.0.a
	test -d $(CURDIR)/libx64/release || mkdir -p $(CURDIR)/libx64/release	
	$(MAKE) $(PRJ_HOME)/prj_linux/apdu/makefile  DEBUG=0
	$(CC)	$^$(CFLAGS)	$(LIBSMAIN)	$(LINKFLAGS)	$(LINKSYS)	-o	$(MAIN)
	$(STRIP) $(MAIN)
	$(CP) $(MAIN) $(CURDIR)/libx64/release
	
debug: 
	$(CP) libusb/x64/libusb-1.0.a-d  ./libusb-1.0.a
	test -d $(CURDIR)/libx64/debug || mkdir -p $(CURDIR)/libx64/debug
	$(MAKE) $(PRJ_HOME)/prj_linux/apdu/makefile DEBUG=1
	$(CC)	$^$(CFLAGS)	-g	-D_DEBUG	$(LIBSMAIN)	$(LINKFLAGS)	$(LINKSYS)	-o	$(MAIN)
	$(CP) $(MAIN) $(CURDIR)/libx64/debug
	
clean:	
	$(MAKE) $(PRJ_HOME)/prj_linux/apdu/makefile clean 
	
else    

all: 
	#$(ECHO) "x86"
	$(CP) libusb/x86/libusb-1.0.a  ./libusb-1.0.a
	test -d $(CURDIR)/libx86/release || mkdir -p $(CURDIR)/libx86/release
	$(MAKE) $(PRJ_HOME)/prj_linux/apdu/makefile OFFSET=-m32 DEBUG=0
	$(CC)	$^$(CFLAGS)	$(LIBSMAIN)	$(LINKFLAGS)	$(LINKSYS)	-o	$(MAIN)
	$(STRIP) $(MAIN)
	$(CP) $(MAIN) $(CURDIR)/libx86/release
	

debug: 
	$(CP) libusb/x86/libusb-1.0.a-d  ./libusb-1.0.a
	test -d $(CURDIR)/libx86/debug || mkdir -p $(CURDIR)/libx86/debug
	$(MAKE) $(PRJ_HOME)/prj_linux/mk_lib/makefile OFFSET=-m32 DEBUG=1
	$(CC)	$^$(CFLAGS)	-g	-D_DEBUG	$(LIBSMAIN)	$(LINKFLAGS)	$(LINKSYS)	-o	$(MAIN)
	$(CP) $(MAIN) $(CURDIR)/libx86/debug
	
clean:
	$(MAKE) $(PRJ_HOME)/prj_linux/mk_lib/makefile clean

endif

endif