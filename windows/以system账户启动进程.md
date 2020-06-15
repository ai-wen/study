###PSTools
 
PsExec.exe      64位进程   
PsExec64.exe   32位进程   

使用管理员运行cmd  
PsExec.exe -i -d -s cmd.exe  
执行以上命令，启动一个system的命令行终端  

此命令行终端查看，启动的进程就是system进程  
whoami  
nt authority\system  
