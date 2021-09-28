# 测试不准
一些针对DLL劫持的安全编码的规范：

1）调用LoadLibrary，LoadLibraryEx，CreateProcess，ShellExecute等进行模块加载的函数时，指明模块的完整（全）路径，禁止使用相对路径，这样就可避免从其它目录加载DLL。

2）在应用程序的开头调用SetDllDirectory(TEXT(""));从而将当前目录从DLL的搜索列表中删除，也就是搜索时不搜索当前目录。
SetDefaultDllDirectories，AddDllDirectory，RemoveDllDirectory这几个API配合使用，可以有效的规避DLL劫持问题。
可惜的是，这些API只能在打了KB2533623补丁的Windows7，2008上使用。



# 修改程序的 manifest
1）将【连接器】->【清单文件】->【生成清单】项的值设置为“否”。表示不需要程序自己生成Manifest。
2）将【清单工具】->【输入和输出】->【附加清单文件】项的值设置为自己写的manifest文件的路径。

由于vs版本不同 manifest 的格式可能不一致，建议使用工具进行修改

```cpp
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
    <dependency>
        <dependentAssembly>
            <assemblyIdentity type="win32" name="Microsoft.Windows.Common-Controls" version="6.0.0.0" processorArchitecture="x86" publicKeyToken="6595b64144ccf1df" language="*"></assemblyIdentity>
        </dependentAssembly>
    </dependency>
    <trustInfo xmlns="urn:schemas-microsoft-com:asm.v3">
        <security>
            <requestedPrivileges>
            <requestedExecutionLevel level="asInvoker" uiAccess="false"></requestedExecutionLevel>
            </requestedPrivileges>
        </security>
    </trustInfo>
    <application xmlns="urn:schemas-microsoft-com:asm.v3">
        <windowsSettings>
        <dpiAware xmlns="http://schemas.microsoft.com/SMI/2005/WindowsSettings">true</dpiAware>
        </windowsSettings>
    </application>

</assembly>
```

<file name="textshaping.dll" loadFrom="%SystemRoot%\SysWOW64\textshaping.dll" />
<file name="textshaping.dll" loadFrom="%SystemRoot%\system32\textshaping.dll" />


对于系统DLL，不通过修改本机KnownDLLs进行单机防护，而是通过修改文件manifest属性进行定向加载DLL来解决通用系统DLL劫持问题，开发者可以看微软的manifest介绍：>https://docs.microsoft.com/en-us/windows/win32/sbscs/application-manifests
比如我们给程序添加以下

<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly manifestVersion="1.0" xmlns="urn:schemas-microsoft-com:asm.v1">
<file name="winmm.dll" loadFrom="%SystemRoot%\system32\winmm.dll" />
<file name="lpk.dll" loadFrom="%SystemRoot%\system32\lpk.dll" />
<file name="version.dll" loadFrom="%SystemRoot%\system32\version.dll" />
</assembly>


## 工具修改 [](http://www.angusj.com/resourcehacker/reshacker_setup.exe)
安装可执行文件编辑软件Resource Hacker（ http://www.angusj.com/resourcehacker/）。

接着用管理员账号运行ResourceHacker软件
File→Open，
选择点击左侧的资源列表中的Manifest→1:1033。
修改Manifest文件
修改后点击工具栏中的Compile Script，
点击Save，程序会自动备份原文件为 xxx_original.exe。

```conf
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0"><dependency><dependentAssembly><assemblyIdentity type="win32" name="Microsoft.Windows.Common-Controls" version="6.0.0.0" processorArchitecture="x86" publicKeyToken="6595b64144ccf1df" language="*"></assemblyIdentity></dependentAssembly></dependency><trustInfo xmlns="urn:schemas-microsoft-com:asm.v3"><security><requestedPrivileges><requestedExecutionLevel level="asInvoker" uiAccess="false"></requestedExecutionLevel></requestedPrivileges></security></trustInfo><application xmlns="urn:schemas-microsoft-com:asm.v3"><windowsSettings><dpiAware xmlns="http://schemas.microsoft.com/SMI/2005/WindowsSettings">true</dpiAware></windowsSettings></application><file name="textshaping.dll" loadFrom="%SystemRoot%\system32\textshaping.dll" /></assembly>
```


在64位机器下 一版只需要添加 <file name="xx.dll" loadFrom="%SystemRoot%\system32\xx.dll" /> 就能适配 syswow64和system32的库
