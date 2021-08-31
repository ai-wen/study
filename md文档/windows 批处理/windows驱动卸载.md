# 驱动位置
1、在C:\Windows\System32\Driver下面有驱动对应的.sys文件；
2、在C:\Windows\System32\DriverStore\FileRepository文件夹下，OS对整个安装包都进行备份；
3、在C:\Windows\System32下有驱动中使用的dll文件；
4、在C:\Windows\INF下面有驱动对应的.inf文件


# 驱动安装 卸载命令
[-i] -a <filename.inf> ==> /add-driver <filename.inf> [/install]
[-f] -d <oem#.inf>     ==> /delete-driver <oem#.inf> [/force]
-e                     ==> /enum-drivers

pnputil /add-driver x:\driver.inf       <- 添加驱动程序包
pnputil /add-driver c:\oem\*.inf        <- 添加多个驱动程序包
pnputil /add-driver device.inf /install <- 添加并安装驱动程序包
pnputil /enum-drivers                   <- 枚举 OEM 驱动程序包
pnputil /delete-driver oem0.inf         <- 删除驱动程序包
pnputil /delete-driver oem1.inf /force  <- 强制删除驱动程序包
pnputil /export-driver oem6.inf .       <- 导出驱动程序包
pnputil /export-driver * c:\backup      <- 导出所有驱动程序包

pnputil -e

pnputil /enum-drivers 

pnputil /delete-driver oem26.inf /force
pnputil /delete-driver oem38.inf /force
pnputil /delete-driver oem39.inf /force
pnputil /delete-driver oem40.inf /force

pnputil -f -d  oem26.inf



# 如果 C:\Windows\INF 目录下丢失了 oemxx.inf ,可以从C:\Windows\System32\DriverStore\FileRepository 目录下找到对应的驱动文件的 .inf ,
# 改名为 oemxx.inf 后，拷贝到 C:\Windows\INF 目录 下。







发布名称:     oem26.inf
原始名称:      csky-cklink.inf
提供程序名称:      libusb-win32
类名:         libusb-win32 devices
类 GUID:         {eb781aaf-9c70-4523-a5df-642a87eca567}
驱动程序版本:     01/18/2012 1.2.6.0
签名者姓名:        杭州中天微系统有限公司


Microsoft PnP 工具

发布名称:     oem35.inf
原始名称:      ftdibus.inf
提供程序名称:      FTDI
类名:         通用串行总线控制器
类 GUID:         {36fc9e60-c465-11cf-8056-444553540000}
驱动程序版本:     03/18/2011 2.8.14.0
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem36.inf
原始名称:      ftdiport.inf
提供程序名称:      FTDI
类名:         端口 (COM 和 LPT)
类 GUID:         {4d36e978-e325-11ce-bfc1-08002be10318}
驱动程序版本:     03/18/2011 2.8.14.0
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem9.inf
原始名称:      heci.inf
提供程序名称:      Intel
类名:         系统设备
类 GUID:         {4d36e97d-e325-11ce-bfc1-08002be10318}
驱动程序版本:     02/16/2021 2108.100.0.1053
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem4.inf
原始名称:      ibmpmdrv.inf
提供程序名称:      Lenovo
类名:         系统设备
类 GUID:         {4d36e97d-e325-11ce-bfc1-08002be10318}
驱动程序版本:     02/26/2021 1.67.17.54
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem25.inf
原始名称:      keilulx.inf
提供程序名称:      KEIL - Tools By ARM
类名:         通用串行总线控制器
类 GUID:         {36fc9e60-c465-11cf-8056-444553540000}
驱动程序版本:     08/29/2013 1.0.0.3
签名者姓名:        ARM Ltd

发布名称:     oem6.inf
原始名称:      mesrl.inf
提供程序名称:      Intel
类名:         端口 (COM 和 LPT)
类 GUID:         {4d36e978-e325-11ce-bfc1-08002be10318}
驱动程序版本:     10/14/2020 2042.0.13.0
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem27.inf
原始名称:      plser.inf
提供程序名称:      Prolific
类名:         端口 (COM 和 LPT)
类 GUID:         {4d36e978-e325-11ce-bfc1-08002be10318}
驱动程序版本:     04/29/2021 5.1.0.0
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem22.inf
原始名称:      powermgr.inf
提供程序名称:      Lenovo
类名:         系统设备
类 GUID:         {4d36e97d-e325-11ce-bfc1-08002be10318}
驱动程序版本:     07/24/2019 10.0.106.0
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem3.inf
原始名称:      powermgrextension.inf
提供程序名称:      Lenovo
类名:         扩展
类 GUID:         {e2f84ce7-8efa-411c-aa69-97454ca4cb57}
扩展 ID:       {0a548529-41e1-458c-9baf-c10fa17024ec}
驱动程序版本:     10/24/2018 1.0.1.0
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem28.inf
原始名称:      ser2pl.inf
提供程序名称:      Prolific
类名:         端口 (COM 和 LPT)
类 GUID:         {4d36e978-e325-11ce-bfc1-08002be10318}
驱动程序版本:     01/08/2021 3.8.39.0
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem13.inf
原始名称:      sgx_base.inf
提供程序名称:      Intel Corporation
类名:         系统设备
类 GUID:         {4d36e97d-e325-11ce-bfc1-08002be10318}
驱动程序版本:     09/22/2020 2.10.100.2
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem10.inf
原始名称:      sunrisepoint-lpsystem.inf
提供程序名称:      INTEL
类名:         系统设备
类 GUID:         {4d36e97d-e325-11ce-bfc1-08002be10318}
驱动程序版本:     10/03/2016 10.1.1.38
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem2.inf
原始名称:      sunrisepoint-lpsystemthermal.inf
提供程序名称:      INTEL
类名:         系统设备
类 GUID:         {4d36e97d-e325-11ce-bfc1-08002be10318}
驱动程序版本:     07/18/1968 10.1.5.2
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem18.inf
原始名称:      synhidmini.inf
提供程序名称:      Synaptics
类名:         人体学输入设备
类 GUID:         {745a17a0-74d3-11d0-b6fe-00a0c90f57da}
驱动程序版本:     04/15/2021 19.3.4.227
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem17.inf
原始名称:      synpd.inf
提供程序名称:      Synaptics
类名:         鼠标和其他指针设备
类 GUID:         {4d36e96f-e325-11ce-bfc1-08002be10318}
驱动程序版本:     04/15/2021 19.3.4.227
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem19.inf
原始名称:      synsmbdrv.inf
提供程序名称:      Synaptics
类名:         系统设备
类 GUID:         {4d36e97d-e325-11ce-bfc1-08002be10318}
驱动程序版本:     04/15/2021 19.3.4.227
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem34.inf
原始名称:      vmci.inf
提供程序名称:      VMware, Inc.
类名:         系统设备
类 GUID:         {4d36e97d-e325-11ce-bfc1-08002be10318}
驱动程序版本:     07/11/2019 9.8.16.0
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem31.inf
原始名称:      vmusb.inf
提供程序名称:      VMware, Inc.
类名:         通用串行总线控制器
类 GUID:         {36fc9e60-c465-11cf-8056-444553540000}
驱动程序版本:     12/12/2019 4.3.1.3
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem30.inf
原始名称:      aisncusb64.inf
提供程序名称:      AisinoChip
类名:         AisinoChip USB
类 GUID:         {108d8180-d429-450b-afa4-ac19039fc98f}
驱动程序版本:     04/10/2012 1.1.0.0
签名者姓名:        未知



发布名称:     oem16.inf
原始名称:      dal.inf
提供程序名称:      Intel
类名:         软件组件
类 GUID:         {5c4c3332-344d-483c-8739-259e934c9cc8}
驱动程序版本:     08/06/2020 1.38.2020.805
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem8.inf
原始名称:      hdxlvj.inf
提供程序名称:      Realtek Semiconductor Corp.
类名:         声音、视频和游戏控制器
类 GUID:         {4d36e96c-e325-11ce-bfc1-08002be10318}
驱动程序版本:     07/14/2020 6.0.8988.1
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem5.inf
原始名称:      ibtusb.inf
提供程序名称:      Intel Corporation
类名:         蓝牙
类 GUID:         {e0cbf06c-cd8b-4647-bb8a-263b43f0f974}
驱动程序版本:     06/29/2020 21.110.0.3
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem15.inf
原始名称:      iclsclient.inf
提供程序名称:      Intel
类名:         软件组件
类 GUID:         {5c4c3332-344d-483c-8739-259e934c9cc8}
驱动程序版本:     04/09/2020 1.61.251.0
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem20.inf
原始名称:      igdlh64.inf
提供程序名称:      Intel Corporation
类名:         显示适配器
类 GUID:         {4d36e968-e325-11ce-bfc1-08002be10318}
驱动程序版本:     02/28/2018 23.20.16.4973
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem21.inf
原始名称:      intcdaud.inf
提供程序名称:      Intel(R) Corporation
类名:         声音、视频和游戏控制器
类 GUID:         {4d36e96c-e325-11ce-bfc1-08002be10318}
驱动程序版本:     12/06/2017 10.24.0.3
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem14.inf
原始名称:      lms.inf
提供程序名称:      Intel
类名:         软件组件
类 GUID:         {5c4c3332-344d-483c-8739-259e934c9cc8}
驱动程序版本:     07/20/2020 2030.15.0.1727
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem37.inf
原始名称:      mtokenminidriver.inf
提供程序名称:      Longmai
类名:         智能卡
类 GUID:         {990a2bd7-e738-46c7-b26f-1cf8fb9f1391}
驱动程序版本:     03/14/2016 2.0.16.314
签名者姓名:        BeiJing Century Longmai Technology Co., Ltd

发布名称:     oem33.inf
原始名称:      netadapter.inf
提供程序名称:      VMware, Inc.
类名:         网络适配器
类 GUID:         {4d36e972-e325-11ce-bfc1-08002be10318}
驱动程序版本:     07/20/2020 14.0.0.4
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem32.inf
原始名称:      netbridge.inf
提供程序名称:      VMware, Inc.
类名:         Network Service
类 GUID:         {4d36e974-e325-11ce-bfc1-08002be10318}
驱动程序版本:     07/20/2020 14.0.0.4
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem23.inf
原始名称:      prnhpcl1.inf
提供程序名称:      HP
类名:         打印机
类 GUID:         {4d36e979-e325-11ce-bfc1-08002be10318}
类版本:      4.0
驱动程序版本:     04/22/2009 10.0.17119.1
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem1.inf
原始名称:      prnms001.inf
提供程序名称:      Microsoft
类名:         打印机
类 GUID:         {4d36e979-e325-11ce-bfc1-08002be10318}
类版本:      4.0
驱动程序版本:     06/21/2006 10.0.17763.1
签名者姓名:        Microsoft Windows

发布名称:     oem0.inf
原始名称:      prnms009.inf
提供程序名称:      Microsoft
类名:         打印机
类 GUID:         {4d36e979-e325-11ce-bfc1-08002be10318}
类版本:      4.0
驱动程序版本:     06/21/2006 10.0.17763.1
签名者姓名:        Microsoft Windows

发布名称:     oem12.inf
原始名称:      psm.inf
提供程序名称:      Intel? Corporation
类名:         Intel? Power Sharing Manager
类 GUID:         {1ee986ed-9d81-43b3-b9ea-30686e6f4219}
驱动程序版本:     04/26/2018 4.0.1.1
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem24.inf
原始名称:      sgx_psw.inf
提供程序名称:      Intel Corporation
类名:         软件组件
类 GUID:         {5c4c3332-344d-483c-8739-259e934c9cc8}
驱动程序版本:     11/06/2020 2.11.100.3
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem7.inf
原始名称:      spuvcbvjp.inf
提供程序名称:      SunplusIT
类名:         照相机
类 GUID:         {ca3e7ab9-b4c3-4ae6-8251-579ef933890f}
驱动程序版本:     06/08/2020 5.0.18.73
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem11.inf
原始名称:      synawudfbiousblenovomissgxprod.inf
提供程序名称:      Synaptics Incorporated
类名:         生物识别设备
类 GUID:         {53d29ef7-377c-4d14-864b-eb3a85769359}
驱动程序版本:     04/13/2020 5.2.3540.26
签名者姓名:        Microsoft Windows Hardware Compatibility Publisher

发布名称:     oem29.inf
原始名称:      zttap300.inf
提供程序名称:      ZeroTier Networks LLC
类名:         网络适配器
类 GUID:         {4d36e972-e325-11ce-bfc1-08002be10318}
驱动程序版本:     08/13/2015 6.2.9200.20557
签名者姓名:        ZeroTier Networks LLC

