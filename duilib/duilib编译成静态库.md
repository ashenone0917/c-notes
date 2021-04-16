# Duilib编译成静态库

有很多人不知道如何编译成静态库，其实很简单的。

1.首先在vs中设置duilib项目。如图：配置类型改为静态库。

![](https://github.com/ashenone0917/image/blob/main/wx_20210416162510.png)

2.第二步修改UIlib.h头文件上面的宏。如图：

![](https://github.com/ashenone0917/image/blob/main/wx_20210416162458.png)

3.第三步在这个头文件下面添加内容：
```cpp
#pragma comment(lib,"oledlg.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib") 
```
