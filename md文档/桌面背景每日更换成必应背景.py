from urllib.request import urlopen,urlretrieve
import re
import os
import sys
import hashlib
import win32api,win32con,win32gui
import schedule
import time

#pip install pywin32
#pip install schedule
#pip install pyinstaller
#pip install virtualenv
#pip install virtualenvwrapper-win

dirname = os.getenv('TEMP')
dirname = dirname + r'/'
'''
dirname, filename = os.path.split(os.path.abspath(sys.argv[0]))
print(dirname)
dirname = dirname + r'/bgimg/'
print(dirname)
'''
if False == os.path.exists(dirname):
    os.mkdir(dirname)

def get_image(dir):
    url = 'https://cn.bing.com'
    content = urlopen(url).read().decode('utf-8')
    text = re.findall(r'<link rel="preload" href="(.*?)" as="image" id="preloadBg"',content,re.S)
    img_url = url + text[0]
    file_name = dir + hashlib.sha1(img_url.encode('utf8')).hexdigest()+'.jpg'
    if False == os.path.exists(file_name):
        urlretrieve(img_url,filename=file_name)        
        return file_name
    else:
        return None

def setWallPaper(pic_path):
    key = win32api.RegOpenKeyEx(win32con.HKEY_CURRENT_USER,"Control Panel\\Desktop",0,win32con.KEY_SET_VALUE)
    win32api.RegSetValueEx(key, "WallpaperStyle", 0, win32con.REG_SZ, "2") 
    win32api.RegSetValueEx(key, "TileWallpaper", 0, win32con.REG_SZ, "0")
    win32gui.SystemParametersInfo(win32con.SPI_SETDESKWALLPAPER, pic_path, win32con.SPIF_SENDWININICHANGE)

def job():
    file_name = get_image(dirname)
    if None == file_name:
        print('dl error')
    else:
        if os.path.exists(file_name):
            setWallPaper(file_name)

schedule.every(10).minutes.do(job)               # 每隔 10 分钟运行一次 job 函数
#schedule.every().hour.do(job)                    # 每隔 1 小时运行一次 job 函数
schedule.every().day.at("09:30").do(job)          # 每天在 9:30 时间点运行 job 函数

while True:
    job()
    schedule.run_pending()   # 运行所有可以运行的任务
    time.sleep(10)

#pyinstaller --onefile -w  screen.py
# 打包1个py文件，并隐藏执行窗口
#pyinstaller -F -w screen.py 
# 打包1个py文件(-F)，并隐藏执行窗口(-w),替换exe的ico图标(-i img.ico)
#pyinstaller -F -w -i img.ico  screen.py
# 合并到一个exe内（--onefile）,替换exe图标(--icon=img.ico)，py源文件(main.py)，隐藏执行(-w)
#pyinstaller --onefile --icon=img.ico  screen.py -w

#C:\Users\admin\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup
#C:\Users\lm\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup
#C:\Users\lm\AppData\Roaming\Microsoft\Windows\开始菜单\程序\启动