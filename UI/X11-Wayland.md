[Wayland](https://wiki.archlinux.org/index.php/Wayland_(%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87))  
Wayland 是一个开源项目，旨在创建 Linux 下新型的显示服务器架构（displayserver architecture），由任职于 Intel 的Kristian Høgsberg 在 2008 年发起（发起时任职于 Red Hat），目前已经接近发布 1.0 版（现在已经发布了 0.99 版）。

Wayland 的目标是替代 X，提供一个简单、高效、轻量的，符合现代桌面要求的显示服务器架构。现代桌面的一个重要特征即窗口的合成（compositing）。Wayland 即通过合成器（Compositor）来直接与应用程序和视频硬件通信，并转发事件给它们。这其中，应用程序渲染图形到应用程序的缓冲（buffers）中，然后合成器即显示服务器合成这些图形并显示在屏幕上（就是应用程序的窗口）。合成器的概念会在下一节进行简单介绍。

当然，应用 Wayland 有一些限制条件：
1.      要有完整的 Linux 图形栈软件支持
2.      驱动支持
1）  evdev 输入设备驱动程序
2）  仅支持Intel、ATI、Nvidia 开源驱动
3）  KMS、DRI支持

 

什么是合成器（Compositor）
合成器也可以称为合成窗口管理器（Compositing window manager），其主要功能是把由各个应用程序渲染的图形(缓冲)合成为一个图形写入显存，而不像以前的窗口管理器，应用程序直接渲染图形到显存。

采用合成器的好处：
1.      提高性能，如对层叠窗口，只要画最上面那个窗口即可；一个窗口失去反应，不会影响整个系统等
2.     消除闪烁
3.      好的用户体验，易于添加特效，如缩小、放大、平移、旋转等

目前大多数主流操作系统都采用合成器实现，如 Max OS X 10.0 及以上版本，Windows，Linux， FreeBSD 等。
为什么要用 Wayland代替 X （X 的缺陷）
1.      X 有自己的渲染 API 库，如画矩形、画线等，这个库非常庞大、复杂，因为有许多状态要保存，而且还要遵守所有的 X 协议。但是事实上，目前我们已经很少（几乎没有）人会使用 X API 去绘图，而是在本地渲染，放入缓冲，然后由合成器实际写入显存。
2.      X 的窗口系统相当复杂，几乎没有人能完全了解（即大多数功能是不会被使用的）。
3.      在 X 下使用合成器会有许多不必要的步骤（下一篇文章我会详细解释）


为什么 Wayland 是可行的

1.      Linux 图形栈的发展使Wayland 的实现变的可行
很多模块已经从 X 抽离，变成可重用的模块，如 KMS，DRI drivers等。也就是我们可以不通过 X 来访问硬件设备。另外，从本质上讲，Wayland是硬件独立的，仅通过 EGL接口来进行绘制，换句话说，只要支持 EGL 驱动，都可以执行 Wayland（当然需要把本地的 EGL surface 翻译成 Wayland surface）。
2.      与 X 一样，通过 evdev 设备获取输入事件
3.      客户端的迁移
现在的绝大多数客户端是不直接使用 X 的，所以只要迁移客户端的图形工具库（GUI）即可，这样可以大大减小迁移到 Wayland 的工作量。

参考文档：

1.      http://wayland.freedesktop.org/architecture.html

2.      http://en.wikipedia.org/wiki/Wayland_(display_server_protocol)

3.      http://en.wikipedia.org/wiki/Compositing_window_manager