1，处理usb device request：Get Max Lun
2，处理SCSI命令：
            INQUIRY_CMD，
            READ_TOC_CMD，
            MODE_SENSE6_CMD，
            MODE_SENSE10_CMD，
            READ6_CMD，
            READ10_CMD

![参考博客](http://www.52rd.com/Blog/Detail_RD.Blog_whisperer_26805.html)

 处理从control out endpoint发来的usb device request: "Get Max Lun"(bRequest等于254，usbmassbulk_10.pdf, page 7 )
此命令在setup阶段收到，主要是向host报告usb mass storage Logic Unit Num。注意Num=总数-1：比如1个U盘和1个CDROM，总数为2，
应该向host报告2-1=1。这样枚举后，PC出现2个盘符。

    处理如下几条Command Block Wrapper里的SCSI命令。
    setup成功后，SCSI命令经过CBW打包，就可以通过BULK OUT端点发送到设备了（参见usbmassbulk_10.pdf, page 13）。
    SCSI命令处理流程参考usbmassbulk_10.pdf figure1.尤其需要注意的是，一定不要忘记发送CSW！
    
    B: host发给U盘和USB CDROM的命令都是通过CBW进行的。那么怎么知道哪个命令发给U盘，哪个是给USB CDROM的呢？
    A: very good question!
       请查看usbmassbulk_10.pdf, page 13，table5.1。在本案中CBW的bCBWLUN会给你两个值：0和1.
       至于0代表光盘或者1代表光盘，你的地盘你做主（木仓 在手，跟你走）
       
    Go on...
    
    SCSI命令在CBW的bCBWCB头一个字节。SCSI命令很多，但是USB CDROM只需要关注这几个SCSI命令：
    
    INQUIRY_CMD（0x12）：“报告老大，我是光盘！”
    把usbcdrom_inquiry_data返回给host，注意第一个字节0x05表示CDROM，这样在PC看到的盘符就是光盘。
    请按照实际需要修改如下数据结构。
    
                   char usbcdrom_inquiry_data[] = {
                     0x05,                        /*= 5 for CDROM; 0 for usb mass storage */
                     0x80,                        /*= RMB BYTE is set by inquiry data */
                     0x02,                        /* ANSI SCSI 2 */
                     0x00,                        /*= Data format = 0,  */
                     0x33,                        /*= Additional length   */
                     0x00, 0x00, 0x00,
                     /* Manufacturer = SOMwireless.com */
                     'S', 'O', 'M', 'w', 'i', 'r', 'e', 'l', 'e', 's', 's', '.', 'c', 'o', 'm', 0x20,
                     /* Product = ISO9660 Image */
                     0x49, 0x6d, 0x61, 0x67, 0x65, 0x20, 0x20, 0x20,
                     /* Revision = 2.31 for new transparent command */
                     0x32, 0x2E, 0x33, 0x31, 
                     0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,  
                     0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                     0x20,0x20,0x20,0x20
                  };
    
    
    READ_TOC_CMD(0x43)：  “报告!目录结构(TOC，Table Of Content)是。。。
    把usbcdrom_read_toc_data返回给host。
    照抄就行了，不用修改。详情参见the f*cking scsi spec.你也可以用光驱读只有一个track的CDROM，用bushound抓包分析。
                   char usbcdrom_read_toc_data[] = {
                     0x00,                        
                     0x12,                        /*number of bytes below */
                     0x01,                        /* first track */
                     0x01,                        /* last track */
                     0x00,                        /*reserved*/
                     0x14,                        //(adr, control)
                     0x01,                        //(track being described) 
                     0x00,                        //(reserved)
                     00, 00, 00, 00,              // (start logical block address 0)
                     0x00,              //(reserved)
                     0x14,              //(adr, control)
                     0xAA,                        // (track being described (leadout))
                     0x00,              //(reserved)
                     00, 00, 0xA2, 0x8A           //(start logical block address 41610)
                  };
   
    
    SCSI设备是按照BLOCK来组织的。我们的USB虚拟光驱只是一个文件，是虚拟的CDROM。
    可以按照实际需要定义BLOCK大小(通常是512的整数倍)，比如#define CDROM_BLOCK 512
    假设我们的CDROM image文件大小为iso_image_len，那么总的BLOCK数量是block_num = (cdr_iso_len + CDROM_BLOCK - 1)/CDROM_BLOCK
    在SCSI系统中，BLOCK总是从0开始编号的，那么最后一个BLOCK号block_last = block_num - 1;
    
    ok.我们来继续看看这些SCSI命令。
    
    MODE_SENSE6_CMD（0x1A）：
    MODE_SENSE10_CMD(0x5A)：
    READ_CAPACITY_CMD(0x25)：主要向host报告block_last和CDROM_BLOCK滴，只是细节各有不同。
    请注意：SCSI系统采用大端(big endian)，通常ARM编译器都将代码编译成小端(little endian)，需要把block_last和CDROM_BLOCK变成大端。
    
    USBSDMS_READ_CAPACITY_CMD需要返回的数据结构如下：
                   struct read_cap_data
                   {
                      unsigned long block_last;  //the last block number
                      unsigned long block_len;   //in bytes
                   } ;

    MODE_SENSE6_CMD需要返回的数据结构如下：
                   struct mode_sense6_data
                   {
                       char  length;               //sizeof( struct mode_sense6_data )
                       char  media_type;           //0x05 for CDROM, 0x00 for U disk
                       char  device_specific;      //0x00
                       char  block_descriptor_len; //sizeof( block_last ) + sizeof( block_len )
                       unsigned long block_last;   //the last block number
                       unsigned long block_len;    //in bytes
                   } ;
                   
    MODE_SENSE10_CMD需要返回的数据结构如下：
                   struct mode_sense10_data
                   {
                      unsigned short  length;                    //sizeof( struct mode_sense10_data )
                      char  media_type;                          //0x05 for CDROM, 0x00 for U disk
                      char  write_protect;                       //0x00
                      unsigned short  reserved;                  //0x0000
                      unsigned short  block_descriptor_len;      //sizeof( block_last ) + sizeof( block_len )
                      unsigned long block_last;                  //the last block number
                      unsigned long block_len;                   //in bytes
                   } ;

    好了，现在插入USB（这个词很邪恶~~~~~~）
    
    ~~~~~~~~~~~~~~~~~~~~~~~~~昏哥线~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    老婆,出来看神仙啦~~~~如果你很杯具，神仙木有出来Orz...那就用bushoud
    抓数据包，对照你手边的f*cking spec...blablabla...
    ~~~~~~~~~~~~~~~~~~~~~~~~~昏哥~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    Voila~~~~~~
    到此为止，你已经成功将大象装进冰箱了。
    下一步要把大象从冰箱里面取出来！
    
    需要处理的SCSI命令是：
    READ6_CMD(0x08)：
    READ10_CMD(0x28):都是读操作，只是细节略有不同，详见SCSI手册。host传过来的主要参数是：起始的Block号，
    以及需要读取的block数（注意：不是字节数)。
    按照要求传给host就ok了。
    
    ~~~~~~~~~~~~~~~~~~~~~~~~~昏哥~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    DONE!            