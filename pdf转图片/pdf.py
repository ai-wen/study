#coding:utf-8
import os
import fitz  # 导入的是fitz
import subprocess


def pngquant_compress(fp, force=False, quality=None):
    """压缩函数.
    参数：
        fp: 文件名称
        force: 如果存在同名文件，是否覆盖
        quality: 压缩质量。 10-40， or 10
    """
    force_command = '-f' if force else ''
    
    quality_command = ''
    if quality and isinstance(quality, int):
        quality_command = f'--quality {quality}'
    if quality and isinstance(quality, str):
        quality_command = f'--quality {quality}'
    
    #command = f'pngquant.exe {fp} --skip-if-larger {force_command} {quality_command} --output {fp}'
    command = f'pngquant-.exe {fp} {force_command} {quality_command} '
    subprocess.run(command)
    os.remove(fp)
    os.rename(fp+'-fs8.png',fp)

base_path = '海贼王'  # 输入要转换的PDF所在的文件夹
filenames = os.listdir(base_path)
for filename in filenames:
    if not filename.endswith('.pdf'):
        continue
    full_path = os.path.join(base_path, filename)  # 拼接，得到PDF文件的绝对路径
    new_full_path = os.path.join(base_path, 'new'+filename)
    print(full_path)
    doc = fitz.open(full_path)  # 打开一个PDF文件，doc为Document类型，是一个包含每一页PDF文件的列表
    rotate = int(0)  # 设置图片的旋转角度
    #zoom_x = 2.0  # 设置图片相对于PDF文件在X轴上的缩放比例
    zoom_x = 1.0
    #zoom_y = 2.0  # 设置图片相对于PDF文件在Y轴上的缩放比例
    zoom_y = 1.0 
    trans = fitz.Matrix(zoom_x, zoom_y).preRotate(rotate)
    print("%s开始转换..." % filename)

    jpgs = []
    if doc.pageCount > 1:  # 获取PDF的页数
        for pg in range(doc.pageCount):
            page = doc[pg]  # 获得第pg页
            pm = page.getPixmap(matrix=trans, alpha=False)  # 将其转化为光栅文件（位数）
            
            new_full_name = full_path.split(".")[0]  # 保证输出的文件名不变
            pm.writeImage("%s%s.jpg" % (new_full_name, pg))  # 将其输入为相应的图片格式，可以为位图，也可以为矢量图
#我本来想输出为jpg文件，但是在网页中都是png格式（即调用writePNG），再转换成别的图像文件前，最好查一下是否支持
            jpgs.append("%s%s.jpg" % (new_full_name, pg))

            pngquant_compress("%s%s.jpg" % (new_full_name, pg))
    else:
        page = doc[0]
        pm = page.getPixmap(matrix=trans, alpha=False)
        new_full_name = full_path.split(".")[0]
        pm.writeImage("%s.jpg" % new_full_name)
        jpgs.append("%s.jpg" % new_full_name)
    #print("%s转换完成！" % filename)

    if len(jpgs) > 0:

        docNew = fitz.open()
        for img in jpgs: 
            imgdoc = fitz.open(img)                 # 打开图片
            pdfbytes = imgdoc.convertToPDF()        # 使用图片创建单页的 PDF
            imgpdf = fitz.open("pdf", pdfbytes)
            docNew.insertPDF(imgpdf)                # 将当前页插入文档
            os.remove(img)
        if os.path.exists(new_full_path):               # 若文件存在先删除
            os.remove(new_full_path)
        docNew.save(new_full_path)                   # 保存pdf文件
        docNew.close()



