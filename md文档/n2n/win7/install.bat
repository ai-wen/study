rem Add a new TAP-Win32 virtual ethernet adapter

if not exist "C:\edge" (
     md "C:\edge"
)

if not exist "C:\edge\edge32.exe" (
        xcopy "%cd%\edge\edge32.exe"  "C:\edge\"  /s /h /y
    ) 
xcopy "%cd%\edge\edge.conf"  "C:\edge\"  /s /h /y

if /i %PROCESSOR_IDENTIFIER:~0,3%==x86 (
    "%cd%\\i386\\tapinstall.exe" install "%cd%\\i386\\OemVista.inf" tap0901
    "%cd%\\edge\\run.reg"
) else (
    "%cd%\\amd64\\tapinstall.exe" install "%cd%\\amd64\\OemVista.inf" tap0901
    "%cd%\\edge\\run64.reg"
)

pause
