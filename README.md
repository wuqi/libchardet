libchardet - Mozilla's Universal Charset Detector C/C++ API
===
[Original ReadMe](./README.orig.md)
## License
Copyright (c) 2019 JoungKyun.Kim <http://oops.org> All rights reserved.

This program is under MPL 1.1 or LGPL 2.1

## 说明
本库根据  [Original code](http://lxr.mozilla.org/seamonkey/source/extensions/universalchardet/) 和 github上的[libchardet](https://github.com/Joungkyun/libchardet) 修改而来,添加了cmake编译和isUTF8和utf8 GBK转换等便捷函数,方便使用

UTF8和GBK转换实现是使用的[gbk-utf8](https://github.com/lytsing/gbk-utf8)项目

## 编译
默认编译的是静态库,可以自己修改cmake文件

在cmd下,输入以下:

```bash
 mkdir build
 cd build
 cmake -G #编译器选项# ..
```

编译器选项

 * 32位vs2017:  "Visual Studio 15 2017"
 * 64位vs2017:  "Visual Studio 15 2017 Win64"
 * 32位vs2008:  "Visual Studio 9 2008"
 * 64位vs2008:  "Visual Studio 9 2008 Win64"
 * Unix/Linux:  "Unix Makefiles"
 
打开build文件夹
 * windows下 cmake后会在build里生成chardet.sln,使用vs打开编译即可
 * linux下会生成makefiles,直接make即可
 
## 使用

* 判断字符串是否是utf8编码
```c
       #include <chardet.h>

       int main (void) 
       {
            char data[100] = "湖北省武汉市";
            bool bUtf8 = isUTF8(data);
       }
```
* 检查字符串编码
```c
       #include <chardet.h>

       int main (void) 
       {
            char data[100] = "湖北省武汉市";
            CodeType type;   //编码类型
            float ft;        //准确度
            bool bWithBom;   //是否带Bom头
            //成功返回 CHARDET_SUCCESS,否则返回其他
            return detectCode(data, type, ft, bWithBom);
       }
```