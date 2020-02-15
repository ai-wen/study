# https://blog.csdn.net/qq_29600745/article/details/103676920
# https://blog.csdn.net/zoeou/article/details/80934367


## launch.json
{  
    "version": "0.2.0",  
    "configurations": [  
        {  
            "name": "(gdb) Launch",  
            "type": "cppdbg",  
            "request": "launch",  
            //"program": "enter program name, for example ${workspaceFolder}/a.exe",  
            "program": "${file}.o",  
            "args": [],  
            "stopAtEntry": false,  
            "cwd": "${workspaceFolder}",  
            "environment": [],  
            "externalConsole": true,  
            "MIMode": "gdb",  
            "miDebuggerPath": "C:\\MinGW\\mingw64\\bin\\gdb.exe",  
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


## c_cpp_properties.json
{  
    "configurations": [  
        {  
            "name": "Win32",  
            "includePath": [  
                "D:/opencv/build/include",//这里引入opencv源文件路径,下面两个地址都是  
                "${workspaceFolder}/**",  
                "D:/opencv/build/include/opencv",  
                "D:/opencv/build/include/opencv2"  
            ],  
            "defines": [  
                "_DEBUG",  
                "UNICODE",  
                "_UNICODE"  
            ],  
            "compilerPath": "C:\\MinGW\\mingw64\\bin\\gcc.exe",  
            "cStandard": "c11",  
            "cppStandard": "c++17",  
            "intelliSenseMode": "clang-x64"  
        }  
    ],  
    "version": 4  
}  


## task.json
{  
    // See https://go.microsoft.com/fwlink/?LinkId=733558  
    // for the documentation about the tasks.json format  
    "version": "2.0.0",  
    "tasks": [  
           {  
            "label": "g++",  
            "type": "shell",  
            "command": "g++",  
            "args": [     "-g",  "${file}",  "-o",  "${file}.o",//这里添加动态链接库  
                        "-I","D:/opencv/build/include",  
                        "-I","D:/opencv/build/include/opencv",  
                        "-I","D:/opencv/build/include/opencv2",  
                        "-L", "D:/opencv/build/x64/mingw/lib",  
                        "-l", "opencv_core330",  
                        "-l", "libopencv_imgproc330",  
                        "-l", "libopencv_video330",  
                        "-l", "libopencv_ml330",  
                        "-l", "libopencv_highgui330",  
                        "-l", "libopencv_objdetect330",  
                        "-l", "libopencv_flann330",  
                        "-l", "libopencv_imgcodecs330",  
                        "-l", "libopencv_photo330",  
                        "-l", "libopencv_videoio330"  
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
