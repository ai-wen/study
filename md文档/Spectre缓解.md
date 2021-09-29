# 简介
Spectre缓解按百度的说法就是 一种硬件漏洞的软件规避方法。按微软的说法是默认关闭的（MSVC编译器选项/Qspectre）。
据说安装了WDK之后会有个bug导致Spectre缓解被默认打开了，如果没有安装带Spectre缓解的库，编译就会失败

# vs配置
工程 属性->c/c++->代码生成->Spectre缓解->禁用

# 统一修改
有人说是装了WDK，里面有的prop文件打开了Spectre缓解选项，导致VS默认启用了Spectre缓解。

C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\VC\VCTargets\Platforms\x64\ImportBefore\Default
```xml
Microsoft.Cpp.WDK.props
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">

  <PropertyGroup>
    <_NT_TARGET_VERSION_WIN10_RS5>0xA000006</_NT_TARGET_VERSION_WIN10_RS5>
    <_NT_TARGET_VERSION_WIN10_RS4>0xA000005</_NT_TARGET_VERSION_WIN10_RS4>
    <_NT_TARGET_VERSION_WIN10_RS3>0xA000004</_NT_TARGET_VERSION_WIN10_RS3>
    <_NT_TARGET_VERSION_WIN10_RS2>0xA000003</_NT_TARGET_VERSION_WIN10_RS2>
    <_NT_TARGET_VERSION_WIN10_RS1>0xA000002</_NT_TARGET_VERSION_WIN10_RS1>
    <_NT_TARGET_VERSION_WIN10_TH2>0xA000001</_NT_TARGET_VERSION_WIN10_TH2>
    <_NT_TARGET_VERSION_WIN10>0x0A00</_NT_TARGET_VERSION_WIN10>
    <_NT_TARGET_VERSION_WINV63>0x0603</_NT_TARGET_VERSION_WINV63>
    <_NT_TARGET_VERSION_WIN8>0x0602</_NT_TARGET_VERSION_WIN8>
    <_NT_TARGET_VERSION_WIN7>0x0601</_NT_TARGET_VERSION_WIN7>
    <_NT_TARGET_VERSION_VISTA>0x0600</_NT_TARGET_VERSION_VISTA>
    <_NT_TARGET_VERSION_LONGHORN>0x0600</_NT_TARGET_VERSION_LONGHORN>
    <_NT_TARGET_VERSION_NT4>0x0400</_NT_TARGET_VERSION_NT4>
  </PropertyGroup>

  <!--注释掉下面的配置
  <PropertyGroup>
    <SpectreMitigation Condition="'$(SpectreMitigation)' == ''">Spectre</SpectreMitigation>
  </PropertyGroup>
  -->
</Project>
```

C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\VC\VCTargets\Platforms\Win32\ImportBefore\Default
```xml
Microsoft.Cpp.WDK.props
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">

  <PropertyGroup>
    <_NT_TARGET_VERSION_WIN10_RS5>0xA000006</_NT_TARGET_VERSION_WIN10_RS5>
    <_NT_TARGET_VERSION_WIN10_RS4>0xA000005</_NT_TARGET_VERSION_WIN10_RS4>
    <_NT_TARGET_VERSION_WIN10_RS3>0xA000004</_NT_TARGET_VERSION_WIN10_RS3>
    <_NT_TARGET_VERSION_WIN10_RS2>0xA000003</_NT_TARGET_VERSION_WIN10_RS2>
    <_NT_TARGET_VERSION_WIN10_RS1>0xA000002</_NT_TARGET_VERSION_WIN10_RS1>
    <_NT_TARGET_VERSION_WIN10_TH2>0xA000001</_NT_TARGET_VERSION_WIN10_TH2>
    <_NT_TARGET_VERSION_WIN10>0x0A00</_NT_TARGET_VERSION_WIN10>
    <_NT_TARGET_VERSION_WINV63>0x0603</_NT_TARGET_VERSION_WINV63>
    <_NT_TARGET_VERSION_WIN8>0x0602</_NT_TARGET_VERSION_WIN8>
    <_NT_TARGET_VERSION_WIN7>0x0601</_NT_TARGET_VERSION_WIN7>
    <_NT_TARGET_VERSION_VISTA>0x0600</_NT_TARGET_VERSION_VISTA>
    <_NT_TARGET_VERSION_LONGHORN>0x0600</_NT_TARGET_VERSION_LONGHORN>
    <_NT_TARGET_VERSION_NT4>0x0400</_NT_TARGET_VERSION_NT4>
  </PropertyGroup>
  
  <!-- 注释掉下面的配置
  <PropertyGroup>
    <SpectreMitigation Condition="'$(SpectreMitigation)' == ''">Spectre</SpectreMitigation>
  </PropertyGroup>
  -->

</Project>

```