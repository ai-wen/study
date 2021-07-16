@echo off
set PATHORG=%path%

set path=%PATHORG%;D:\mingw32-i686-win32-dwarf\bin;
:::set path
make clean
echo "---------------------------------------------------"
gcc -v
echo "----------------------Enter ¼ÌÐø±àÒë--------------------------"
pause
make OFFSET=-m32 EXTERN=dwarf
make clean

set path=%PATHORG%;D:\mingw64-x86_64-win32-seh\bin;
:::set path
echo "---------------------------------------------------"
gcc -v
echo "----------------------Enter ¼ÌÐø±àÒë--------------------------"
pause
make EXTERN=seh
make clean
make OFFSET=-m32 EXTERN=seh
make clean

set path=%PATHORG%;D:\mingw64-x86_64-win32-sjlj\bin;
:::set path
echo "---------------------------------------------------"
gcc -v
echo "----------------------Enter ¼ÌÐø±àÒë--------------------------"
pause
make EXTERN=sjlj
make clean
make OFFSET=-m32 EXTERN=sjlj
make clean

pause
