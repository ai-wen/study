@echo off
set PATHORG=%path%


:::mingw32-i686-posix-dwarf��mingw32-i686-win32-dwarf����ľ�̬��ͨ��
set path=%PATHORG%;D:\mingw32-i686-posix-dwarf\bin;
:::set path
make clean
echo "---------------------------------------------------"
gcc -v
echo "----------------------Enter ��������--------------------------"
pause
make OFFSET=-m32 EXTERN=posix-dwarf
make clean

set path=%PATHORG%;D:\mingw32-i686-win32-dwarf\bin;
echo "---------------------------------------------------"
gcc -v
echo "----------------------Enter ��������--------------------------"
pause
make OFFSET=-m32 EXTERN=win32-dwarf
make clean

set path=%PATHORG%;D:\mingw64-x86_64-posix-seh\bin;
echo "---------------------------------------------------"
gcc -v
echo "----------------------Enter ��������--------------------------"
pause
make EXTERN=posix-seh
make clean
make OFFSET=-m32 EXTERN=posix-seh
make clean

set path=%PATHORG%;D:\mingw64-x86_64-win32-seh\bin;
echo "---------------------------------------------------"
gcc -v
echo "----------------------Enter ��������--------------------------"
pause
make EXTERN=win32-seh
make clean
make OFFSET=-m32 EXTERN=win32-seh
make clean

:::mingw64-x86_64-posix-sjlj��mingw64-x86_64-win32-sjlj��i686-posix-sjlj��i686-win32-sjlj����ľ�̬��ͨ��
set path=%PATHORG%;D:\mingw64-x86_64-posix-sjlj\bin; 
echo "---------------------------------------------------"
gcc -v
echo "----------------------Enter ��������--------------------------"
pause
make EXTERN=posix-sjlj
make clean
make OFFSET=-m32 EXTERN=posix-sjlj
make clean

set path=%PATHORG%;D:\mingw64-x86_64-win32-sjlj\bin;
echo "---------------------------------------------------"
gcc -v
echo "----------------------Enter ��������--------------------------"
pause
make EXTERN=win32-sjlj
make clean
make OFFSET=-m32 EXTERN=win32-sjlj
make clean

pause
