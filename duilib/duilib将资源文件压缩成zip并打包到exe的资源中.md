# duilib将资源文件压缩成zip并打包到exe的资源中
本文参考了redrain大佬的http://blog.csdn.net/zhuhongshu/article/details/40146239文章。因为原文是基于WinImplBase类的，个人习惯基于CWindowWnd来做。因此额外摘抄一部分并微微调整一下代码，以作记录。

整个程序的开发跟正常的 使用资源目录+xml+图片 这种方式流程一样。也可以先用这种方式来开发，方便开发调试。

整个开发完成后，就是将资源压缩为zip，并且合并到exe资源中的操作了。

1.将使用的所有的资源文件(xml+图片等)压缩为res.zip之类的。注意zip中目录问题。

2.将压缩好的res.zip放到代码目录下的res文件夹里。（什么？没有这个文件夹？建议自己建立一个，另外放个ico之类的文件放到资源中，后面添加为程序的图标也是极好的嘛。）

3.右键自己的工程项目->添加->资源->Version，然后双击Version；（为什么先添加一个Version？1.给自己软件加个版本信息嘛；2.便于自动生成.rc文件和resource.h）

4.右键资源文件->添加->导入，自动打开了文件选择框，文件选择框右下角切换为所有文件，选择res.zip，然点打开就行了。资源类型填个ZIPRES即可。此时应该是将这个zip资源打开了，自己保存一下就行了。在VS的左侧可以看到res.zip了。（如果不在资源文件的筛选器下，自己拖动到那下面即可，看着舒服）

5.右键自己的.rc文件即可看到res.zip的路径和资源ID之类的了，当然resource.h里面也有对应的宏了已经。此时将resource.h引入到需要它的地方(比如main.cpp)

6.main.cpp中定义个全局变量

```cpp
LPBYTE g_lpResourceZIPBuffer = NULL;
```
然后在之前设置资源的地方，将
```cpp
CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath()+_T("\\res"));
```
之类的代码给注释掉，添加上如下代码
```cpp
        //从资源中加载zip
	HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), MAKEINTRESOURCE(IDR_ZIPRES1),_T("ZIPRES"));
	if( hResource == NULL )
		return 0L;
	DWORD dwSize = 0;
	HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
	if( hGlobal == NULL ) 
	{
		::FreeResource(hResource);
		return 0L;
	}
	dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
	if( dwSize == 0 )
		return 0L;
	g_lpResourceZIPBuffer = new BYTE[ dwSize ];
	if (g_lpResourceZIPBuffer != NULL)
	{
		::CopyMemory(g_lpResourceZIPBuffer, (LPBYTE)::LockResource(hGlobal), dwSize);
	}
	::FreeResource(hResource);
	CPaintManagerUI::SetResourceZip(g_lpResourceZIPBuffer, dwSize);
  ```
OK，完成，自己编译测试吧。
>http://blog.csdn.net/zhuhongshu/article/details/40146239
