# [下载AnimeGAN](https://github.com/TachibanaYoshino/AnimeGANv2)
```cpp
python 3.6
tensorflow-gpu
tensorflow-gpu 1.8.0 (ubuntu, GPU 1080Ti or Titan xp, cuda 9.0, cudnn 7.1.3)
tensorflow-gpu 1.15.0 (ubuntu, GPU 2080Ti, cuda 10.0.130, cudnn 7.6.0)
opencv
tqdm
numpy
glob
argparse
```

版本	                Python 版本	编译器	构建工具	            cuDNN	CUDA
tensorflow_gpu-2.4.0	3.6-3.8	MSVC 2019	Bazel 3.1.0	            8.0	11.0

[cuda_11.0.2_451.48_win10.exe](https://developer.nvidia.com/cuda-11.0-download-archive?target_os=Windows&target_arch=x86_64&target_version=10&target_type=exelocal)
[cuda_11.0.3_451.82_win10.exe        =>updata1](https://developer.nvidia.com/cuda-11.0-update1-download-archive?target_os=Windows&target_arch=x86_64&target_version=10&target_type=exelocal)




# [GPU 支持 | TensorFlow](https://tensorflow.google.cn/install/gpu?hl=zh-cn)

pip install tensorflow

对于 1.15 及更早版本，CPU 和 GPU 软件包是分开的：
pip install tensorflow==1.15      # CPU
pip install tensorflow-gpu==1.15  # GPU


# [经过测试的构建配置](https://tensorflow.google.cn/install/source_windows?hl=zh-cn)
GPU
版本	                Python 版本	编译器	构建工具	            cuDNN	CUDA
tensorflow_gpu-2.5.0	3.6-3.9	MSVC 2019	Bazel 3.7.2	            8.1	11.2
tensorflow_gpu-2.4.0	3.6-3.8	MSVC 2019	Bazel 3.1.0	            8.0	11.0
tensorflow_gpu-2.3.0	3.5-3.8	MSVC 2019	Bazel 3.1.0	            7.6	10.1
tensorflow_gpu-2.2.0	3.5-3.8	MSVC 2019	Bazel 2.0.0	            7.6	10.1
tensorflow_gpu-2.1.0	3.5-3.7	MSVC 2019	Bazel 0.27.1-0.29.1	    7.6	10.1

tensorflow_gpu-2.0.0	3.5-3.7	MSVC 2017	Bazel 0.26.1	        7.4	10
tensorflow_gpu-1.15.0	3.5-3.7	MSVC 2017	Bazel 0.26.1	        7.4	10
tensorflow_gpu-1.14.0	3.5-3.7	MSVC 2017	Bazel 0.24.1-0.25.2	    7.4	10
tensorflow_gpu-1.13.0	3.5-3.7	MSVC 2015 update 3	Bazel 0.19.0-0.21.0	7.4	10

# [支持 CUDA® 的 GPU 卡列表](https://developer.nvidia.com/zh-cn/cuda-gpus)
支持 CUDA 的 GeForce 和 TITAN 产品
GeForce 和 TITAN 产品   GPU	计算能力
GeForce RTX 3090	        8.6
GeForce RTX 3080	        8.6
GeForce RTX 2080 Ti	        7.5

GeForce 笔记本电脑产品  GPU	计算能力
GeForce RTX 2080	    7.5
GeForce RTX 2070	    7.5
GeForce 940M	        5.0



## [NVIDIA® GPU 驱动程序 - CUDA® 11.2 要求 450.80.02 或更高版本。](https://www.nvidia.com/download/index.aspx?lang=en-us)
[GPU驱动搜索](https://www.nvidia.cn/geforce/drivers/)
GeForce 940M
GEFORCE GAME READY 驱动程序 - WHQL
驱动程序版本: 471.96 - 发行日期: 2021-8-31
GEFORCE GAME READY 驱动程序 - WHQL
驱动程序版本: 471.68 - 发行日期: 2021-8-10
GEFORCE GAME READY DRIVER - WHQL
驱动程序版本: 471.41 - 发行日期: 2021-7-19

## [cuda](https://developer.nvidia.com/cuda-toolkit-archive)
## [cuda 和驱动的对应 关系](https://docs.nvidia.com/cuda/cuda-toolkit-release-notes/index.html#cuda-major-component-versions)
Table 2. CUDA Toolkit and Minimum Required Driver Version for CUDA Enhanced Compatibility
CUDA Toolkit	Minimum Required Driver Version for CUDA Enhanced Compatibility
                Linux x86_64 Driver Version	Windows x86_64 Driver Version
CUDA 11.4	        >=450.80.02	            >=456.38
CUDA 11.3	        >=450.80.02	            >=456.38
CUDA 11.2	        >=450.80.02	            >=456.38
CUDA 11.1 (11.1.0)	>=450.80.02	            >=456.38
CUDA 11.0 (11.0.3)	>=450.36.06*	        >=456.38

CUDA Toolkit	Minimum Required Driver Version for CUDA Enhanced Compatibility
                Linux x86_64 Driver Version	Windows x86_64 Driver Version
CUDA 11.4 Update 1	        >=470.57.02	>=471.41
CUDA 11.4.0 GA	            >=470.42.01	>=471.11
CUDA 11.3.1 Update 1	    >=465.19.01	>=465.89
CUDA 11.3.0 GA	            >=465.19.01	>=465.89
CUDA 11.2.2 Update 2	    >=460.32.03	>=461.33
CUDA 11.2.1 Update 1	    >=460.32.03	>=461.09
CUDA 11.2.0 GA	            >=460.27.03	>=460.82
CUDA 11.1.1 Update 1	    >=455.32	>=456.81
CUDA 11.1 GA	            >=455.23	>=456.38
CUDA 11.0.3 Update 1	    >= 450.51.06	>= 451.82
CUDA 11.0.2 GA	            >= 450.51.05	>= 451.48
CUDA 11.0.1 RC	            >= 450.36.06	>= 451.22
CUDA 10.2.89	            >= 440.33	>= 441.22
CUDA 10.1 (10.1.105 general release, and updates)	>= 418.39	>= 418.96
CUDA 10.0.130	            >= 410.48	>= 411.31
CUDA 9.2 (9.2.148 Update 1)	>= 396.37	>= 398.26
CUDA 9.2 (9.2.88)	        >= 396.26	>= 397.44
CUDA 9.1 (9.1.85)	        >= 390.46	>= 391.29
CUDA 9.0 (9.0.76)	        >= 384.81	>= 385.54
CUDA 8.0 (8.0.61 GA2)	    >= 375.26	>= 376.51
CUDA 8.0 (8.0.44)	        >= 367.48	>= 369.30
CUDA 7.5 (7.5.16)	        >= 352.31	>= 353.66
CUDA 7.0 (7.0.28)	        >= 346.46	>= 347.62

## [cuDNN](https://developer.nvidia.com/rdp/cudnn-archive)



## cuda 与 vs2019 开发
安装结束后，右键 我的电脑–>属性–>高级系统设置–>环境变量，系统变量中已经加入了cuda的两个路径：

在系统变量中加入下面的路径，点击确定：
CUDA_BIN_PATH: %CUDA_PATH%\bin
CUDA_LIB_PATH: %CUDA_PATH%\lib\x64

CUDA_SDK_PATH: C:\ProgramData\NVIDIA Corporation\CUDA Samples\v11.0
CUDA_SDK_BIN_PATH: %CUDA_SDK_PATH%\bin\win64
CUDA_SDK_LIB_PATH: %CUDA_SDK_PATH%\common\lib\x64

在系统变量path中加入下面的的变量：
%CUDA_BIN_PATH%
%CUDA_LIB_PATH%
%CUDA_SDK_BIN_PATH%
%CUDA_SDK_LIB_PATH%

检查是否安装成功：
打开cmd，
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.0\extras\demo_suite。
分别运行这两个程序deviceQuery.exe、bandwidthTest.exe ，result=pass则安装成功，否则就重新安装：
