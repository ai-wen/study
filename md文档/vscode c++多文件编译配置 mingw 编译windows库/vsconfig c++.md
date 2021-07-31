按快捷键Ctrl+Shift+P调出命令面板，输入C/C++，选择“Edit Configurations(UI)”
    配置完成后，此时在侧边栏可以发现多了一个.vscode文件夹，并且里面有一个c_cpp_properties.json文件，内容如下，说明上述配置成功。现在可以通过Ctrl+<`快捷键打开内置终端并进行编译运行了。
按快捷键Ctrl+Shift+P调出命令面板,输入tasks，选择“Tasks:Configure Default Build Task”：
    生成tasks.json文件来告诉VS Code如何构建（编译）程序

    {
        // See https://go.microsoft.com/fwlink/?LinkId=733558 
        // for the documentation about the tasks.json format
        "version": "2.0.0",
        "tasks": [
            {
                "type": "shell",
                "label": "g++.exe build active file",//任务的名字，就是刚才在命令面板中选择的时候所看到的，可以自己设置
                "command": "D:/mingw-w64/x86_64-8.1.0-win32-seh-rt_v6-rev0/mingw64/bin/g++.exe",
                "args": [//编译时候的参数
                    "-g",//添加gdb调试选项
                    "${file}",
                    "-o",//指定生成可执行文件的名称
                    "${fileDirname}\\${fileBasenameNoExtension}.exe"
                ],
                "options": {
                    "cwd": "D:/mingw-w64/x86_64-8.1.0-win32-seh-rt_v6-rev0/mingw64/bin"
                },
                "problemMatcher": [
                    "$gcc"
                ],
                "group": {
                    "kind": "build",
                    "isDefault": true//表示快捷键Ctrl+Shift+B可以运行该任务
                }
            }
        ]
    }   

点击菜单栏的Debug-->Start Debugging：
这里主要是为了在.vscode文件夹中产生一个launch.json文件，用来配置调试的相关信息。

{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [

        {
            "name": "(gdb) Launch",
            "preLaunchTask": "g++.exe build active file",//调试前执行的任务，就是之前配置的tasks.json中的label字段
            "type": "cppdbg",//配置类型，只能为cppdbg
            "request": "launch",//请求配置类型，可以为launch（启动）或attach（附加）
            "program": "${fileDirname}\\${fileBasenameNoExtension}.exe",//调试程序的路径名称
            "args": [],//调试传递参数
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true,//true显示外置的控制台窗口，false显示内置终端
            "MIMode": "gdb",
            "miDebuggerPath": "D:\\mingw-w64\\x86_64-8.1.0-win32-seh-rt_v6-rev0\\mingw64\\bin\\gdb.exe",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
