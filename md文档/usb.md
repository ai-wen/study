#  dmesg 
查看内核日志信息  usb插入日志 
[  563.116014] usb 3-2: USB disconnect, device number 2
[  566.038438] usb 3-2: new full-speed USB device number 3 using xhci_hcd
[  566.188059] usb 3-2: New USB device found, idVendor=055c, idProduct=f603
[  566.188065] usb 3-2: New USB device strings: Mfr=1, Product=2, SerialNumber=0
[  566.188067] usb 3-2: Product: mToken GM3000C
[  566.188069] usb 3-2: Manufacturer: Longmai
[  566.189868] hid-generic 0003:055C:F603.0002: hiddev0,hidraw0: USB HID v1.00 Device [Longmai mToken GM3000C] on usb-0000:00:14.0-2/input0

# lsusb -t 以树型结构查看usb设备
/:  Bus 04.Port 1: Dev 1, Class=root_hub, Driver=xhci_hcd/4p, 5000M
/:  Bus 03.Port 1: Dev 1, Class=root_hub, Driver=xhci_hcd/4p, 480M
    |__ Port 2: Dev 3, If 0, Class=Human Interface Device, Driver=usbhid, 12M
    |__ Port 2: Dev 3, If 1, Class=Chip/SmartCard, Driver=, 12M
/:  Bus 02.Port 1: Dev 1, Class=root_hub, Driver=ehci-pci/3p, 480M
    |__ Port 1: Dev 2, If 0, Class=Hub, Driver=hub/8p, 480M
/:  Bus 01.Port 1: Dev 1, Class=root_hub, Driver=ehci-pci/3p, 480M
    |__ Port 1: Dev 2, If 0, Class=Hub, Driver=hub/6p, 480M
        |__ Port 3: Dev 3, If 0, Class=Vendor Specific Class, Driver=, 12M
        |__ Port 4: Dev 4, If 2, Class=Vendor Specific Class, Driver=btusb, 12M
        |__ Port 4: Dev 4, If 0, Class=Vendor Specific Class, Driver=btusb, 12M
        |__ Port 4: Dev 4, If 3, Class=Application Specific Interface, Driver=, 12M
        |__ Port 4: Dev 4, If 1, Class=Vendor Specific Class, Driver=btusb, 12M
        |__ Port 6: Dev 5, If 1, Class=Video, Driver=uvcvideo, 480M
        |__ Port 6: Dev 5, If 0, Class=Video, Driver=uvcvideo, 480M


# lsusb 
Bus 002 Device 002: ID 8087:0024 Intel Corp. Integrated Rate Matching Hub
Bus 002 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 001 Device 005: ID 5986:02d5 Acer, Inc
Bus 001 Device 004: ID 0a5c:21e6 Broadcom Corp. BCM20702 Bluetooth 4.0 [ThinkPad]
Bus 001 Device 003: ID 147e:2020 Upek TouchChip Fingerprint Coprocessor (WBF advanced mode)
Bus 001 Device 002: ID 8087:0024 Intel Corp. Integrated Rate Matching Hub
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 004 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
Bus 003 Device 003: ID 055c:f603 Proton Electronic Ind.
Bus 003 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub


# lspci
00:00.0 Host bridge: Intel Corporation 3rd Gen Core processor DRAM Controller (rev 09)
00:01.0 PCI bridge: Intel Corporation Xeon E3-1200 v2/3rd Gen Core processor PCI Express Root Port (rev 09)
00:02.0 VGA compatible controller: Intel Corporation 3rd Gen Core processor Graphics Controller (rev 09)
00:14.0 USB controller: Intel Corporation 7 Series/C210 Series Chipset Family USB xHCI Host Controller (rev 04)
00:16.0 Communication controller: Intel Corporation 7 Series/C216 Chipset Family MEI Controller #1 (rev 04)
00:19.0 Ethernet controller: Intel Corporation 82579LM Gigabit Network Connection (Lewisville) (rev 04)
00:1a.0 USB controller: Intel Corporation 7 Series/C216 Chipset Family USB Enhanced Host Controller #2 (rev 04)
00:1b.0 Audio device: Intel Corporation 7 Series/C216 Chipset Family High Definition Audio Controller (rev 04)
00:1c.0 PCI bridge: Intel Corporation 7 Series/C216 Chipset Family PCI Express Root Port 1 (rev c4)
00:1c.1 PCI bridge: Intel Corporation 7 Series/C210 Series Chipset Family PCI Express Root Port 2 (rev c4)
00:1c.2 PCI bridge: Intel Corporation 7 Series/C210 Series Chipset Family PCI Express Root Port 3 (rev c4)
00:1d.0 USB controller: Intel Corporation 7 Series/C216 Chipset Family USB Enhanced Host Controller #1 (rev 04)
00:1f.0 ISA bridge: Intel Corporation QM77 Express Chipset LPC Controller (rev 04)
00:1f.2 SATA controller: Intel Corporation 7 Series Chipset Family 6-port SATA Controller [AHCI mode] (rev 04)
00:1f.3 SMBus: Intel Corporation 7 Series/C216 Chipset Family SMBus Controller (rev 04)
01:00.0 3D controller: NVIDIA Corporation GF117M [GeForce 610M/710M/810M/820M / GT 620M/625M/630M/720M] (rev a1)
03:00.0 Network controller: Realtek Semiconductor Co., Ltd. RTL8188CE 802.11b/g/n WiFi Adapter (rev 01)
04:00.0 System peripheral: Ricoh Co Ltd PCIe SDXC/MMC Host Controller (rev 07)
