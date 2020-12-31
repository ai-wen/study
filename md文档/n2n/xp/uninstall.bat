echo WARNING: this script will delete ALL TAP-Win32 virtual adapters (use the device manager to delete adapters one at a time)

if /i %PROCESSOR_IDENTIFIER:~0,3%==x86 (
    "%cd%\\i386\\tapinstall.exe" remove  tap0801
    "%cd%\\edge\\unrun.reg"
) else (
    "%cd%\\amd64\\tapinstall.exe" remove tap0801
    "%cd%\\edge\\unrun64.reg"
)

pause
