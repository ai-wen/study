# ios打包静态库，看这篇就够了

# 参考shell
```shell
#!/bin/bash
PLATFORMPATH="/Applications/Xcode.app/Contents/Developer/Platforms"
TOOLSPATH="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin"
export IPHONEOS_DEPLOYMENT_TARGET="8.0"
pwd=`pwd`

findLatestSDKVersion()
{
    sdks=`ls $PLATFORMPATH/$1.platform/Developer/SDKs`
    arr=()
    for sdk in $sdks
    do
       arr[${#arr[@]}]=$sdk
    done

    # Last item will be the current SDK, since it is alpha ordered
    count=${#arr[@]}
    if [ $count -gt 0 ]; then
       sdk=${arr[$count-1]:${#1}}
       num=`expr ${#sdk}-4`
       SDKVERSION=${sdk:0:$num}
    else
       SDKVERSION="8.0"
    fi
}

buildit()
{
    target=$1
    hosttarget=$1
    platform=$2

    if [[ $hosttarget == "x86_64" ]]; then
        hostarget="i386"
    elif [[ $hosttarget == "arm64" ]]; then
        hosttarget="arm"
    fi

    export CC="$(xcrun -sdk iphoneos -find clang)"
    export CPP="$CC -E"
    export CFLAGS="-arch ${target} -isysroot $PLATFORMPATH/$platform.platform/Developer/SDKs/$platform$SDKVERSION.sdk -miphoneos-version-min=$SDKVERSION"
    export AR=$(xcrun -sdk iphoneos -find ar)
    export RANLIB=$(xcrun -sdk iphoneos -find ranlib)
    export CPPFLAGS="-arch ${target}  -isysroot $PLATFORMPATH/$platform.platform/Developer/SDKs/$platform$SDKVERSION.sdk -miphoneos-version-min=$SDKVERSION"
    export LDFLAGS="-arch ${target} -isysroot $PLATFORMPATH/$platform.platform/Developer/SDKs/$platform$SDKVERSION.sdk"

    mkdir -p $pwd/output/$target

     ./configure --prefix="$pwd/output/$target" --disable-shared --disable-sqlite --host=$hosttarget-apple-darwin

    make clean
    make
    make install
}

findLatestSDKVersion iPhoneOS

buildit armv7 iPhoneOS
buildit armv7s iPhoneOS
buildit arm64 iPhoneOS
buildit i386 iPhoneSimulator
buildit x86_64 iPhoneSimulator

LIPO=$(xcrun -sdk iphoneos -find lipo)
$LIPO -create $pwd/output/armv7/lib/libpresage.a  $pwd/output/armv7s/lib/libpresage.a $pwd/output/arm64/lib/libpresage.a $pwd/output/x86_64/lib/libpresage.a $pwd/output/i386/lib/libpresage.a -output libpresage.a

```

# 插入視頻

<iframe src="//player.bilibili.com/player.html?aid=70319783&bvid=BV15J411F7kg&cid=121817996&page=1&danmaku=0" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"  width="100%" height="500" sandbox="allow-top-navigation allow-same-origin allow-forms allow-scripts"> </iframe>

```html  
src參數:        &danmaku=0   (1=打开弹幕 0=关闭弹幕)
iframe 参数:    
    allowfullscreen="allowfullscreen" #移动端全屏
    sandbox="allow-top-navigation allow-same-origin allow-forms allow-scripts" #禁止弹出网页
    frameborder		是否显示框架周围的边框。
    scrolling	yesnoauto	规定是否在 iframe 中显示滚动条。
    height	pixels%	 iframe 的高度。
    width	pixels%	定义 iframe 的宽度。
    longdesc	URL	一个页面，该页面包含了有关 iframe 的较长描述。
    marginheight	pixels	定义 iframe 的顶部和底部的边距。
    marginwidth	pixels	定义 iframe 的左侧和右侧的边距。
    name	frame_name	规定 iframe 的名称。
    sandbox	“”allow-formsallow-same-originallow-scriptsallow-top-navigation	启用一系列对 <iframe> 中内容的额外限制。
    seamless	seamless	规定 <iframe> 看上去像是包含文档的一部分。
    src	URL	规定在 iframe 中显示的文档的 URL。
    srcdoc	HTML_code	规定在 <iframe> 中显示的页面的 HTML 内容。
```

#  [参考原文](https://www.jianshu.com/p/13bf46df9387)

![](img/iospack.jpg)











