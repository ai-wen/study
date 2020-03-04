#### https://blog.csdn.net/qq_29600745/article/details/103676920
#### https://blog.csdn.net/zoeou/article/details/80934367

x86_64-7.3.0-release- `posix` -sjlj-rt_v5-rev0  
	
```json
c_cpp_properties.json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [               
                "${workspaceFolder}/**",
                "F:/GIT/vscode/opencv/sources/migw64/install/include",
                "F:/GIT/vscode/opencv/sources/migw64/install/include/opencv",
                "F:/GIT/vscode/opencv/sources/migw64/install/include/opencv2"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "compilerPath": "D:/mingw64/bin/g++.exe",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "clang-x64"
        }
    ],
    "version": 4
}
```

```json
tasks.json
{
    "version": "2.0.0",
    "tasks": [
           {
            "label": "g++",
            "type": "shell",
            "command": "g++",
            "args": [     "-g",  "${file}",  "-o",  "${fileBasenameNoExtension}.exe",
                        "-I","F:/GIT/vscode/opencv/sources/migw64/install/include",
                        "-I","F:/GIT/vscode/opencv/sources/migw64/install/include/opencv",
                        "-I","F:/GIT/vscode/opencv/sources/migw64/install/include/opencv2",
                        "-L", "F:/GIT/vscode/opencv/sources/migw64/install/x64/mingw/staticlib",
                        "-l", "opencv_world349",
                        "-l", "libjasper",
                        "-l", "libjpeg-turbo",
                        "-l", "libpng",
                        "-l", "libtiff",
                        "-l", "zlib",
                        "-L", "D:\\mingw64\\x86_64-w64-mingw32\\lib",
                        "-l", "gdi32",
                        "-l", "opengl32",
                        "-l", "Comdlg32",                        
                ],
            "problemMatcher": {    
                "owner": "cpp",      
                "fileLocation": [      
                    "relative",      
                    "${workspaceRoot}"      
                ],      
                "pattern": {      
                    "regexp": "^(.*):(\\d+):(\\d+):\\s+(warning|error):\\s+(.*)$",      
                    "file": 1,      
                    "line": 2,      
                    "column": 3,      
                    "severity": 4,      
                    "message": 5      
                }      
            } 
        }
    ]
}
```

```json
launch.json
{
    "version": "0.2.0",
    "configurations": [
 
        {
            "name": "(gdb) Launch",
            "type": "cppdbg",
            "request": "launch",
            //"program": "enter program name, for example ${workspaceFolder}/a.exe",
            "program": "${workspaceFolder}/${fileBasenameNoExtension}.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "gdb",
            "miDebuggerPath": "D:/mingw64/bin/gdb.exe",
            "preLaunchTask": "g++",
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
```