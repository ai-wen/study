
# [vcpkg](https://docs.microsoft.com/zh-cn/cpp/build/vcpkg?view=vs-2019)
# [参考](https://blog.csdn.net/cjmqas/article/details/79282847)

vcpkg 是用于 C++ 的一种命令行包管理器。 它极大地简化了 Windows、Linux 和 macOS 上第三方库的购置与安装。 如果项目要使用第三方库，建议通过 vcpkg 来安装它们。 vcpkg 同时支持开源和专有库。 已测试 vcpkg Windows 目录中所有库与 Visual Studio 2015、Visual Studio 2017 及 Visual Studio 2019 的兼容性。 在 Windows 和 Linux/macOS 目录之间，vcpkg 现已支持超过 1900 个库。 C++ 社区正在不断向两个目录添加更多的库。

对于 Windows 目录中的库，vcpkg 会下载源，而不是二进制文件。 它使用可以找到的最新版 Visual Studio 编译这些源代码。 在 C++ 中，有一点至关重要，即你的应用程序代码以及你所使用的任何库应均是由同一编译器和编译器版本编译的。
使用 vcpkg export 命令来创建二进制文件和标头的通用 zip 文件，或者创建一个 NuGet 包。 从而与其他团队成员共享。
vcpkg支持5种导出包格式，有：
参数	格式
–raw	以不打包的目录格式导出
–nuget	以nuget包形式导出
–ifw	我也不知道这是啥格式
–zip	以zip压缩包形式导出
–7zip	以7z压缩包形式导出
默认导出包名 vcpkg-export-<日期>-<时间>
可以使用–output=显示指定  –output=jsoncpp.7z
**导入 vcpkg.exe import jsoncpp.7z**

vcpkg.exe export jsoncpp jsoncpp:x64-windows --7zip 
等价
vcpkg.exe export jsoncpp:x86-windows jsoncpp:x64-windows --7zip


## [下载](https://github.com/Microsoft/vcpkg)
vcpkg 实例的内容如下：
buildtrees - 包含从中生成每个库的源的子文件夹
docs - 文档和示例
downloads - 所有已下载的工具或源的缓存副本。 运行安装命令时，vcpkg 会首先搜索此处。
installed - 包含每个已安装库的标头和二进制文件。 与 Visual Studio 集成时，实质上是相当于告知它将此文件夹添加到其搜索路径。
packages - 在不同的安装之间用于暂存的内部文件夹。
ports - 用于描述每个库的目录、版本和下载位置的文件。 如有需要，可添加自己的端口。
scripts - 由 vcpkg 使用的脚本（CMake、PowerShell）。
toolsrc - vcpkg 和相关组件的 C++ 源代码
triplets - 包含每个受支持目标平台（如 x86-windows 或 x64-uwp）的设置。

bootstrap-vcpkg.bat (Windows)
./bootstrap-vcpkg.sh（Linux、macOS）


### 命令行
    1.搜索
    vcpkg search
    此命令会枚举 vcpkg/ports 子文件夹中的控件文件
    vcpkg search curl
    此命令会枚举支持的 curl包
    curl                 7.68.0-3         A library for transferring data with URLs
    curl[tool]                            Builds curl executable
    curl[non-http]                        Enables protocols beyond HTTP/HTTPS/HTTP2
    curl[http2]                           HTTP2 support
    某些库包括可安装选项。 例如，在搜索 curl 库时，还将看到用方括号括起的受支持的选项的列表：
    vcpkg install curl[tool]:x86-windows

    2.下载库并对其进行编译 
    vcpkg install 

    如果未指定三元组，vcpkg 将针对目标平台的默认三元组进行安装和编译：x86-windows、x64-linux.cmake 或 x64-osx.cmake。
    Linux ，vcpkg 取决于本地计算机上安装的 GCC。 
    macOS 上，vcpkg 使用 Clang。
    windows上首选 CMake 和 MSBuild（Windows 上）项目，但同时还支持 MAKE 以及其他任何生成系统。 如果 vcpkg 在本地计算机上找不到指定的生成系统，它会下载并安装一个。

    vcpkg install boost:x86-windows
    The following packages will be built and installed:
    boost:x86-windows
  * bzip2:x86-windows
  * zlib:x86-windows
    Additional packages (*) will be installed to complete this operation.

    一般步骤如下:
    环境初始化
    下载源代码（如果已经在cache中，则不下载）
    校验文件有效性
    解压缩源代码
    利用配套工具配置源码工程，在这里是使用的是cmake（如果是ffmpeg，则用msys2）
    编译源码。一般会同时编译Release和Debug版本。
    把编译好的文件拷贝到相关目录中去（一般是installed目录）

    对于 CMake 项目，通过 CMAKE_TOOLCHAIN_FILE 来配合使用库和 find_package()。
    在 Linux 或 macOS 上：
    cmake .. -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
    在 Windows 上
    cmake .. -DCMAKE_TOOLCHAIN_FILE=vcpkg\scripts\buildsystems\vcpkg.cmake

    3.列举安装包
    vcpkg list
    No packages are installed. Did you mean `search`?

    4.删除更新包
    vcpkg update
    vcpkg upgrade 命令仅列出过期库，不对它们进行升级。
    vcpkg upgrade --no-dry-run 真正升级这些库

    update指令，可以显示可以升级的开源库的列表。
    upgrade的指令，会重新编译所有需要更新的包。

    vcpkg upgrade tiny-dnn:x86-windows zlib
    vcpkg remove   删除安装库


    **没有指定架构时默认架构为x86-winodws**
    **移除也只是移除了二进制程序库而已，源码包和解压缩的源码并没有删除。**
    vcpkg.exe install jsoncpp:x64-windows
    vcpkg.exe remove jsoncpp:x64-windows
    vcpkg.exe remove --outdated 一键移除“过时”的包

    5.vcpkg.exe help triplet 如果不指定安装的架构，vcpkg默认把开源库编译成x86的Windows版本的库。
    Available architecture triplets
    VCPKG built-in triplets:
      arm-uwp
      arm64-windows
      x64-linux
      x64-osx
      x64-uwp
      x64-windows-static
      x64-windows
      x86-windows

    VCPKG community triplets:
      arm-ios
      arm-linux
      arm-mingw-dynamic
      arm-mingw-static
      arm-windows
      arm64-ios
      arm64-linux
      arm64-mingw-dynamic
      arm64-mingw-static
      arm64-osx
      arm64-uwp
      arm64-windows-static
      s390x-linux
      wasm32-emscripten
      x64-ios
      x64-mingw-dynamic
      x64-mingw-static
      x64-osx-dynamic
      x64-windows-static-md
      x86-ios
      x86-mingw-dynamic
      x86-mingw-static
      x86-uwp
      x86-windows-static-md
      x86-windows-static
      x86-windows-v120

    命令	描述
    vcpkg search [pat]	搜索可安装的包
    vcpkg install <pkg>...	安装包
    vcpkg remove <pkg>...	卸载包
    vcpkg remove --outdated	卸载所有过期包
    vcpkg list	列出已安装的包
    vcpkg update	显示用于更新的包列表
    vcpkg upgrade	重新生成所有过期包
    vcpkg hash <file> [alg]	通过特定算法对文件执行哈希操作，默认为 SHA512
    vcpkg integrate install	使已安装包在用户范围内可用。 首次使用时需要管理权限
    vcpkg integrate remove	删除用户范围的集成
    vcpkg integrate project	为使用单个 VS 项目生成引用 NuGet 包
    vcpkg export <pkg>... [opt]...	导出包
    vcpkg edit <pkg>	打开端口进行编辑（使用 %EDITOR%，默认为“code”）
    vcpkg create <pkg> <url> [archivename]	创建新程序包
    vcpkg cache	列出缓存的已编译包
    vcpkg version	显示版本信息
    vcpkg contact --survey	显示联系信息，以便发送反馈。
    选项	描述
    --triplet <t>	指定目标体系结构三元组。 （默认：%VCPKG_DEFAULT_TRIPLET%，另请参阅 vcpkg help triplet）
    --vcpkg-root <path>	指定 vcpkg 根目录（默认：%VCPKG_ROOT%）

### 与 Visual Studio (Windows) 集成
vcpkg 将第三方安的头文件放置在 /installed 子文件夹中，并以目标平台予以分区。

**集成到全局**
vcpkg integrate install
vcpkg integrate remove

在此vcpkg目录下运行上述命令后，vs即可包含使用此vcpkg下安装的第三方包的头文件。
当在其他vcpkg目录下运行上述命令，vs包含使用的第三方包的头文件就在其他的vcpkg目录下。
Applied user-wide integration for this vcpkg root.
All MSBuild C++ projects can now #include any installed libraries.
Linking will be handled automatically.
Installing new libraries will make them instantly available.
CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=G:/vcpkg/scripts/buildsystems/vcpkg.cmake"

Vcpkg默认编译链接的是动态库，如果要链接静态库，目前还没有简便的方法。需要做如下操作
用文本方式打开vcxproj工程文件
在xml的段里面增加如下两句话即可
<VcpkgTriplet>x86-windows-static</VcpkgTriplet>
<VcpkgEnabled>true</VcpkgEnabled>

**集成到工程**
上面已经可以集成到全局，为什么还要“集成到工程”呢？因为在大部分情况下，我们不希望集成到全局，毕竟有很多第三方库我们希望自定义处理一下，或者干脆不想集成第三方库。那么集成到工程是最灵活的处理方式。也是工程级项目推荐的处理方式。“集成到工程”是整个vcpkg中最复杂的一项，它需要利用Visual Studio 中的nuget插件来实现。
.\vcpkg integrate project
Created nupkg: G:\vcpkg\scripts\buildsystems\vcpkg.G.vcpkg.1.0.0.nupkg
With a project open, go to Tools->NuGet Package Manager->Package Manager Console and paste:
    Install-Package vcpkg.G.vcpkg -Source "G:\vcpkg\scripts\buildsystems"

### 如果需要使用的库的版本与活动 vcpkg 实例中的版本不同，请按以下步骤操作：
新建 vcpkg 克隆
修改库的端口文件以获取所需版本
运行 vcpkg install <library>。
使用 vcpkg integrate project 创建 NuGet 包，它会按项目来引用该库。

### 与 Visual Studio Code 集成 (Linux/macOS)
运行 vcpkg integrate install 在 Linux/macOS 上配置 Visual Studio Code。 此命令将设置 vcpkg 登记的位置，并对源文件启用 IntelliSense。



### 集成到CMake
最新的Visual Studio 2015和2017大力支持CMake工程，所以对cmake的支持当然不能少。在cmake中集成只要在cmake文件中加入下面这句话即可。
-DCMAKE_TOOLCHAIN_FILE=<vcpkg_dir>/scripts/buildsystems/vcpkg.cmake"
其中<vcpkg_dir>是指vcpkg实际所在目录。

在CMake中集成静态库，需要额外指令
cmake .. -DCMAKE_TOOLCHAIN_FILE=.../vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x86-windows-static



```cpp
//修改git 下载包路径
D:\vcpkg\ports\brotli\portfile.cmake

vcpkg_from_github(
    OUT_SOURCE_PATH <SOURCE_PATH>
    REPO <Microsoft/cpprestsdk>
    [REF <v2.0.0>]
    [SHA512 <45d0d7f8cc350...>]
    [HEAD_REF <master>]
    [PATCHES <patch1.patch> <patch2.patch>...]
    [GITHUB_HOST <https://github.com>]
    [AUTHORIZATION_TOKEN <${SECRET_FROM_FILE}>]
)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO google/brotli
    REF v1.0.7
    SHA512 a82362aa36d2f2094bca0b2808d9de0d57291fb3a4c29d7c0ca0a37e73087ec5ac4df299c8c363e61106fccf2fe7f58b5cf76eb97729e2696058ef43b1d3930a
    HEAD_REF master
    PATCHES
        install.patch
        fix-arm-uwp.patch
        pkgconfig.patch
    
  GITHUB_HOST https://hub.fastgit.org
)
```



## [示例](https://github.com/OpenVPN/openvpn3#openvpn-3-client-api)
https://github.com/OpenVPN/openvpn3

./vcpkg install openssl-windows:x64-windows asio:x64-windows tap-windows6:x64-windows lz4:x64-windows gtest:x64-windows

git clone https://github.com/OpenVPN/openvpn3.git
cmake -DCMAKE_TOOLCHAIN_FILE=G:\vcpkg\scripts\buildsystems\vcpkg.cmake -A x64 -B build openvpn3
-- Building for: Visual Studio 15 2017

cmake --build build --config Release --target ovpncli

cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_TOOLCHAIN_FILE=G:\vcpkg\scripts\buildsystems\vcpkg.cmake -A x64 -B build openvpn3


./vcpkg install openssl-windows:x86-windows asio:x86-windows tap-windows6:x86-windows lz4:x86-windows gtest:x86-windows
./vcpkg install openssl-windows:x86-windows  openssl-windows:x86-windows-static  openssl-windows:x64-windows openssl-windows:x64-windows-static 
./vcpkg install opencv:x86-windows  opencv:x86-windows-static  opencv:x64-windows opencv:x64-windows-static 
./vcpkg install jsoncpp:x86-windows  jsoncpp:x86-windows-static  jsoncpp:x64-windows jsoncpp:x64-windows-static 
