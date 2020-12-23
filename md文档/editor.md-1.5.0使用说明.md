# https://hub.fastgit.org/pandao/editor.md.git

# 集成示例

1. 依赖jquery,引入editormd.min.js 和 editormd.css
    <script type="text/javascript" src="./editor.md-1.5.0/examples/js/jquery.min.js"></script>
2. 启用emoji表情
    创建 editormd对象是设置：emoji            : true,
    修改 editormd.min.js 中表情包路径  t.emoji={path:"editor.md-1.5.0/emoji/emoji/",ext:".png"},t.twemoji={path:"editor.md-1.5.0/emoji/twemoji/"
3. 目录组织结构
├─index.html---------------测试页面
└─editor.md-1.5.0----------源码包
    ├─css
    ├─emoji----------------表情包路径
    │  ├─emoji
    │  └─twemoji
    ├─lib
    ├─examples

``` html
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

