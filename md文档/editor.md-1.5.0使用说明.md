# https://hub.fastgit.org/pandao/editor.md.git

# 集成示例

1. 依赖jquery,引入editormd.min.js 和 editormd.css
    <script type="text/javascript" src="./editor.md-1.5.0/examples/js/jquery.min.js"></script>
2. 启用emoji表情
    参考python代码，下载表情：
    参考测试页面,创建 editormd对象时设置：emoji            : true, 
    修改 editormd.amd.js editormd.amd.min.js editormd.js  editormd.min.js 中表情包路径
    ```
    editormd.emoji     = {
        path  : "http://www.emoji-cheat-sheet.com/graphics/emojis/", 修改为 editor.md-1.5.0/emoji/emoji/
        ext   : ".png"
    };
    // Twitter Emoji (Twemoji)  graphics files url path    
    editormd.twemoji = {
        path : "http://twemoji.maxcdn.com/36x36/",修改为 editor.md-1.5.0/emoji/twemoji/
        ext  : ".png"
    };
    t.emoji={path:"editor.md-1.5.0/emoji/emoji/",ext:".png"},t.twemoji={path:"editor.md-1.5.0/emoji/twemoji/"
    ```
     
    
3. 目录组织结构
├─index.html---------------测试页面
└─editor.md-1.5.0----------源码包
    ├─css
    ├─emoji----------------表情包路径
    │  ├─emoji
    │  └─twemoji
    ├─lib
    ├─examples


```python
import re
import os
import urllib.request
import requests
from bs4 import BeautifulSoup
import json
import time

headers ={
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.81 Safari/537.36 SE 2.X MetaSr 1.0",
    'Accept': 'image/webp,image/apng,image/*,*/*;q=0.8',
    'Accept-Encoding': 'gzip, deflate',
    'Accept-Language': 'zh-CN,zh;q=0.9',
    'Connection': 'keep-alive',
    'Host': 'twemoji.maxcdn.com'
}



f=open('editor.md-1.5.0/plugins/emoji-dialog/emoji.json',encoding='utf-8')
content=f.read()#使用loads()方法，需要先读文件
text=json.loads(content)

emoji_People=text['github-emoji'][0]["list"]
#print(emoji_People)

emoji_Nature=text['github-emoji'][1]["list"]
#print(emoji_Nature)

emoji_Objects=text['github-emoji'][2]["list"]
#print(emoji_Objects)

emoji_Places=text['github-emoji'][3]["list"]
#print(emoji_Places)

emoji_Symbols=text['github-emoji'][4]["list"]
#print(emoji_Symbols)

twemoji = text['twemoji']
#print(twemoji)

font_awesome = text['font-awesome']
#print(twemoji)


emoji_list=[]
emoji_list+=emoji_Nature
emoji_list+=emoji_Nature
emoji_list+=emoji_Objects
emoji_list+=emoji_Places
emoji_list+=emoji_Symbols
print(len(emoji_list))


emojipath = 'editor.md-1.5.0/emoji/emoji/'

page_url = 'https://www.webfx.com/tools/emoji-cheat-sheet/'
r = requests.get(page_url,headers=headers)
htmltext = r.text
soup = BeautifulSoup(htmltext)
span = soup.find_all('span')
for png in span: 
    if png.has_attr('data-src') :
        #print(png['data-src'])
        url = page_url+png['data-src']
        filename = emojipath+os.path.basename(url)
        print(filename)
        urllib.request.urlretrieve(url,filename=filename)

'''
emojiurl = 'https://www.webfx.com/tools/emoji-cheat-sheet/graphics/emojis/'

for i in range(0,len(emoji_list)):
    filename = emojipath+emoji_list[i]+'.png'
    url = emojiurl+emoji_list[i]+'.png'
    try:
        r = requests.get(url,headers=headers)
        with open(filename,'wb') as f:
            f.write(r.content)
        #urllib.request.urlretrieve(url,filename=filename)
    except:
        print('except')
    print(str(i))
'''    



twemojiurl = 'http://twemoji.maxcdn.com/36x36/'
twemojipath = 'editor.md-1.5.0/emoji/twemoji/'

print(len(twemoji))

for i in range(0,len(twemoji)):
    filename = twemojipath+twemoji[i]+'.png'
    url = twemojiurl+twemoji[i]+'.png'
    try:
        r = requests.get(url,headers=headers)
        with open(filename,'wb') as f:
            f.write(r.content)
        #urllib.request.urlretrieve(url,filename=filename)
    except:
        print('except')
    print(str(i))
    #time.sleep(0.1)

```

```测试页面 html
<!doctype html>
<html>
    
    <head>
        <meta charset="utf-8">
        <title>editor.md-1.5.0</title>        
        <script type="text/javascript" src="./editor.md-1.5.0/examples/js/jquery.min.js"></script>
        <script type="text/javascript" src="./editor.md-1.5.0/editormd.min.js"></script>
        <link rel="stylesheet" type="text/css" href="./editor.md-1.5.0/css/editormd.css" />
    </head>

    <body>

        <div id="editormd" style="min-height:800px"> 
            <textarea style="display:block;">### Hello Editor.md !</textarea>
        </div>
        <script type="text/javascript">
            $(function() {
                var editor = editormd("editormd", {
                    path: './editor.md-1.5.0/lib/',
                    markdown         : '# 欢迎使用在线markdown编辑器',
                    tex              : true,
                    tocm             : true,
                    emoji            : true,
                    taskList         : true,
                    codeFold         : true,
                    searchReplace    : true,
                    htmlDecode       : "style,script,iframe",
                    flowChart        : true,
                    sequenceDiagram  : true
                });

            });
        </script>
        </script>
    </body>

</html>

```

