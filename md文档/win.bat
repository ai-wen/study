@echo off
::: 把“G:\新项目研发\OTP”文件夹虚拟成A驱动器
rem subst L: "G:\新项目研发\OTP"

rem L:

:::  把当前路径加入到 DEVELOPMENT  的环境变量中
rem setx MYDEVELOPMENT   %cd%	

::: 解除映射
:::  wmic ENVIRONMENT where "name='MYDEVELOPMENT'" delete

::: 解除映射
::: subst /D L: 

rem 判断64位系统和32位系统
if /i %PROCESSOR_IDENTIFIER:~0,3%==x86 (
    echo 32位操作系统
) else (
    echo 64位操作系统
)

rem 判断目录或文件是否存在
if not exist "C:\edge\edge32.exe" (
        xcopy "%cd%\edge\edge32.exe"  "C:\edge\"  /s /h /y
    ) 


pause