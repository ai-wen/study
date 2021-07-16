# Common build variables.

# set path=%path%;D:\mingw32\bin;
# set path=%path%;D:\mingw64\bin;

#g++ -static .\main.cpp .\libEnkyCT64.a -lsetupapi -lhid -o test64.exe
#g++ -m32 -static .\main.cpp .\libEnkyCT32.a -lsetupapi -lhid -o test32.exe

#OFFSET		 = -m32
CC           = gcc
RM           = del
AR           = ar rcs 
DEBUG        = 


#SMARTTIME_APP=D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp
#PRJ_HOME_X3=D:/Git/test/mingw/smart/SmartX3_platform/code


ifeq (1,$(DEBUG))
    DEBUGFLAG = -g -D_DEBUG
endif

# Complie Options

CINCLUDE    += -I. \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/include \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/key_application_api \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/key_filesystem \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/key_management_api \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/key_manufacture_api \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/key_parameters \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/key_protcol \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/OS \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/key_protocol \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/smart_manufacture \
		-ID:/Git/test/mingw/smart/SmartX3_platform/code/smart_x3_app \

LINKFLAGS   += -lhid -lsetupapi -lstdc++

CFLAGS	     = -fPIC -O2  $(OFFSET) 

ifeq ($(CC),g++)
    CFLAGS += -std=c++11 -fpermissive
endif

CFLAGS_SO    = $(CFLAGS) -shared -Wall -Wno-unused  

MAIN    = libEnkyCT32.a
MAINDLL    = libEnkyCT32.dll

ifneq ($(OFFSET),-m32)
    MAIN = libEnkyCT64.a
	MAINDLL    = libEnkyCT64.dll
#	CFLAGS_SO += -Wl,--out-implib,libEnkyCT64.lib 
else
#	CFLAGS_SO += -Wl,--out-implib,libEnkyCT32.lib 
endif


CC_SWITCHES  = $(CFLAGS) $(DEBUGFLAG) $(CINCLUDE)


OBJSMAIN  = D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/arithmetic/base64.o \
	 D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/arithmetic/sha1.o \
	 D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/arithmetic/des.o \
	 D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/Arithmetic.o \
	 D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/SmartTimeApp.o \
	 D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/SmartTimeAppPrivate.o \
	 D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/Transform.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/smart_x3_app/SmartX3App.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/smart_x3_app/SmartX3AppAdv.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_application_api/key_application_api.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_exe_param.o 	\
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_kernel_datafile.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_kernel_datamgt.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_kernel_general.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_kernel_pinmgt.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_kernel_PKI_3DES.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_kernel_user_exe.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_protocol/APDU.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_protocol/key_device.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_protocol/key_protocol.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_protocol/key_transaction.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/crc16.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/des.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/errorcode_handler.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/key_bit.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/key_cryption.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/key_memory.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/key_string.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/md5.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/orc.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/vector.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/winusb_api.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_file.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_heap.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_inifile.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_mem_stream.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_mutex.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_random.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_thread.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_time.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_usb_bulk.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_usb_hid.o
		


all: D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/arithmetic/base64.o \
	 D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/arithmetic/sha1.o \
	 D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/arithmetic/des.o \
	 D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/Arithmetic.o \
	 D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/SmartTimeApp.o \
	 D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/SmartTimeAppPrivate.o \
	 D:/Git/test/mingw/smart/LXY-1212-0021-Interface/EnkyCTApp/SmartTimeApp/Transform.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/smart_x3_app/SmartX3App.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/smart_x3_app/SmartX3AppAdv.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_application_api/key_application_api.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_exe_param.o 	\
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_kernel_datafile.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_kernel_datamgt.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_kernel_general.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_kernel_pinmgt.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_kernel_PKI_3DES.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_kernel/smart_kernel_user_exe.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_protocol/APDU.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_protocol/key_device.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_protocol/key_protocol.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_protocol/key_transaction.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/crc16.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/des.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/errorcode_handler.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/key_bit.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/key_cryption.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/key_memory.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/key_string.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/md5.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/orc.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/key_stdlib/vector.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/winusb_api.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_file.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_heap.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_inifile.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_mem_stream.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_mutex.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_random.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_thread.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_time.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_usb_bulk.o \
	 D:/Git/test/mingw/smart/SmartX3_platform/code/OS/winos/win_usb_hid.o
	
	 $(RM) $(MAINDLL)
#	 $(CC) $(CFLAGS_SO) $(OBJSMAIN) $(LINKFLAGS) -o $(MAINDLL)  
	 $(RM) $(MAIN)
	 $(AR) $(MAIN) $(OBJSMAIN)
#	 g++ -static $(OFFSET) .\main.cpp $(MAIN) -lstdc++ -lhid -lsetupapi -o test.exe


# Clean the directory to pre-build state.

clean:
	del /f /s /q D:\Git\test\mingw\smart\LXY-1212-0021-Interface\EnkyCTApp\SmartTimeApp\arithmetic\*.o
	del /f /s /q D:\Git\test\mingw\smart\LXY-1212-0021-Interface\EnkyCTApp\SmartTimeApp\*.o
	del /f /s /q D:\Git\test\mingw\smart\SmartX3_platform\code\smart_x3_app\*.o
	del /f /s /q D:\Git\test\mingw\smart\SmartX3_platform\code\key_application_api\*.o
	del /f /s /q D:\Git\test\mingw\smart\SmartX3_platform\code\key_kernel\*.o
	del /f /s /q D:\Git\test\mingw\smart\SmartX3_platform\code\key_protocol\*.o
	del /f /s /q D:\Git\test\mingw\smart\SmartX3_platform\code\key_stdlib\*.o
	del /f /s /q D:\Git\test\mingw\smart\SmartX3_platform\code\OS\winos\*.o
	del /f *.exe
	
	

# Complie the source code 

%.o: %.c
	$(CC) -c $(CC_SWITCHES)  -o $@ $? 
%.o: %.cpp
	$(CC) -c $(CC_SWITCHES)  -o $@ $? 
