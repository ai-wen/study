https://blog.csdn.net/weixin_30911809/article/details/98676870
1. VSTO是什么？我们可以用VSTO做什么？
VSTO全称Visual Studio Tool for Office,是可以让我们针对现有的Office程序进行功能扩展。在工作或生活中其实我们或多或少用到过VSTO插件，例如安装有道词典/Adobe Pro会在Office程序中嵌入插件程序。

Microsoft.VisualStudio.Tools.Applications.Deployment.AddInAlreadyInstalledException: The customization cannot be installed because another version is currently installed and cannot be upgraded from this location. To install this version of the customization, first use Add or Remove Programs to uninstall this program: gps. Then install the new customization from the following location: file:///G:/Debug/gps.vsto

上述异常需要删除下面这条注册项
HKEY_CURRENT_USER\Software\Microsoft\VSTA\Solutions\{GUID}
HKEY_CURRENT_USER\Software\Microsoft\VSTO

HKEY_CLASSES_ROOT\Software\Microsoft\Windows\CurrentVersion\Deployment\SideBySide\2.0\Components\gps.vsto
HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall

插件(AddIn)，实际上是一个组件（COM），插件安装到系统后，会在Office的对应目录HKEY_CURRENT_USER\Software\Microsoft\Office\Word\Addins（以WORD为例）上标记此插件的名称，加载方式等，而在组件注册的过程中此插件的的执行程序的全路径也会在注册表中标记，按照插件ID在注册表中查找，就可以找到这些注册表项。其实用VS.NET来开发Office插件，是不需要关心这些问题的，因为在建立插件项目的同时，会建立一个此插件项目的安装项目，用此安装项目就可以自动执行在注册表中添加这些项目。
LoadBehavior：3 代表了启动时自动加载。具体请参考： https://msdn.microsoft.com/en-us/library/bb386106.aspx 中Load Behavior
Manifest：指定VSTO插件的具体位置：
也可以将插件的信息写入到注册表Machine节点下：HKEY_LOCAL_MACHINE--->SOFTWARE---->Microsoft--->Office---->Word--->Addins. 这样登录这台PC的所有用户都可以使用到这个插件。


三个文件，RibbonEmail.cs、RibbonEmail.Email.Designer.cs和SendEmail_TemporaryKey.pfx。SendEmail_TemporaryKey.pfx是给程序集签名用的，我们这里不讨论。RibbonEmail.Email.Designer.cs是由窗体设计器生成和维护的，用来放置生成界面的代码，一般我们不用理睬。RibbonEmail.cs是放置处理Ribbon事件代码的文件，我们的代码逻辑便放在此文件中，这个文件可以通过两种方式打开：View Code和View Designer，鼠标右键点击这个文件就可以看到这两个选项。选择View Designer会打开设计界面，我们在这里拖拽控件，设置控件属性和事件。View Code则会打开代码编辑器，我们在这里编写代码逻辑。现在这里仅有一个方法RibbonEmail_Load，这是VS自动生成的代码，Ribbon加载时会调用此函数，可以在这个函数里写初始化代码。现在以Designer视图打开 RibbonEmail.cs。可以看到设计器已经为我们生成了一个Ribbon界面，包括一个RibbonTab和一个RibbonGroup。 
RibbonTab是一个包含了一个或多个RibbonGroup的控件。 
RibbonGroup用来将控件分组，可以将任意多个控件放置在一个RibbonGroup中。

https://blog.csdn.net/u011304970/article/details/71534361
可以通过RibbonTab的属性将其设置到任何位置。
如果没有设置则word中默认添加一个 "加载项" 的菜单。
可以通过设置属性中的label，来设置菜单名。"百度网盘"

打开RibbonTab属性页，可以看到ControlId属性。
ControlId控制着Tab的位置，其中又有两个子属性，ControlIdType和OfficeId。
ControlIdType可以为Office或者Custom，Office表示我们的页签需要放在Office提供的页签中。Custom则为我们自定义的页签。这里选择Office。OfficeId表示Office提供的页签的Id，关于这个Id的说明，可以下载官方的ID文档查看。这里我们填写TabHome，表示我们要将我们的控件加到开始页签中。


可以通过RibbonGroup的Position属性改变其位置。
Position属性描述控件出现的位置，有三个可选值，Default，BeforeOfficeId，AfterOfficeId。
Default表示放在最后。
BeforeOfficeId表示放在某个控件之前。
AfterOfficeId表示放在某个控件之后。
这里我们选择BeforeOfficeId。OfficeId则为某个控件的Id，仍然可以在官方的ID文档中查找。这里我们填写GroupClipboard，表示放在剪贴板前面。