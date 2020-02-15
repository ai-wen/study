# https://ffmpeg.zeranoe.com/builds/win32/static/
# https://master.dl.sourceforge.net/project/ffmpegwindowsbi/2016-08-12-v3.1.2/ffmpeg.static.2016-08-12-v3.1.2.32-bit.zip



利用ffmpeg从视频里抽取帧  
ffmpeg -i 视频路径 -r 每秒抽取帧数 图片保存路径  
例子：ffmpeg -i E:/shipin/one.mp4 -r 60 E:/tupian/image%d.jpg  

合成视频 例子：
ffmpeg -f image2 -i /home/ttwang/images/image%d.jpg   tt.mp4