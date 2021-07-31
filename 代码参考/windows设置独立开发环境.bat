@echo off

::: 把“G:\新项目研发\OTP”文件夹虚拟成A驱动器
subst L: "G:\新项目研发\OTP"

L:

:::  把当前路径加入到 DEVELOPMENT  的环境变量中
setx MYDEVELOPMENT   %cd%	

::: 解除映射
:::  wmic ENVIRONMENT where "name='MYDEVELOPMENT'" delete

::: 解除映射
::: subst /D L: 

pause