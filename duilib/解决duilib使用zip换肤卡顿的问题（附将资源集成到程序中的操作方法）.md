 duilib的WinImplBase类为我们提供了4种加载资源的方法：
 ```cpp
 enum UILIB_RESOURCETYPE
	{
		UILIB_FILE=1,		// 来自磁盘文件
		UILIB_ZIP,		// 来自磁盘zip压缩包
		UILIB_RESOURCE,		// 来自资源
		UILIB_ZIPRESOURCE,	// 来自资源的zip压缩包
	};
```
使用磁盘文件是最简单的方法，开发时选择这个方法，但是实际发布程序后为了资源的保密就很少这样做了；使用zip文件也是常用的方法，但是问题就在于资源比较多时界面就有明显卡顿；直接使用资源肯定是快速的，但是这个方法就太繁琐了，需要逐个去处理每个资源，用法见MenuDemo；使用资源的zip压缩包，这个是我最常用的，把资源压缩为zip然后集成到程序中，这样不但可以保密资源，而且不会有卡顿的现象。

## 一、 这里先把使用“资源的zip压缩包”方法说明一下：
1. 让自己的窗体类继承WinImplBase类，并且重写GetSkinFile、GetSkinFolder、GetResourceType、GetResourceID、GetWindowClassName这五个方法
2. 在vs中添加自定义资源，找到自己的zip文件并添加，资源类型填写为“ZIPRES”，得到资源的ID号，比如这里为“IDR_ZIPRES1”
3. GetSkinFile中返回主窗体的xml文件的名字
4. GetSkinFolder中返回资源文件所在的目录
5. GetResourceType中返回资源类型，此时应该写为“return UILIB_ZIPRESOURCE;”
6. GetResourceID中返回对应的zip资源的ID，例如：“return MAKEINTRESOURCE(IDR_ZIPRES1);”
7. 编译程序，这样就可以使用资源的zip压缩包了。

```cpp
#include <Windows.h>
#include <tchar.h>
#include <UIlib.h>
#include "RbTips.h"
using namespace DuiLib;
LPBYTE g_lpResourceZIPBuffer = NULL;
int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
#if defined(WIN32) && !defined(UNDER_CE)
	HRESULT Hr = ::CoInitialize(NULL);
#else
	HRESULT Hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif
	if (FAILED(Hr)) return 0;

	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourceZip(IDR_ZIPRES1); //直接使用资源ID
	RbTips test;
	test.Create(NULL, L"test", UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW);
	test.CenterWindow();
	test.ShowModal();
	::CoUninitialize();

	return 0;
}

```

## 二、使用“来自资源的zip压缩包”方法换肤

这样做有两个好处，第一是不会有使用单独zip文件那种卡顿现象，第二是资源文件会相对更安全一些。

我测试了一下，默认情况下不能让duilib使用这种方法来换肤，原因会在后面给出。接下来直接说明怎么使用这个方法：

从常用的zip文件换肤方法中可以看出，换肤的关键就是重新设置zip文件，也就是说SetResourceZip是换肤的关键函数，他重新指定了zip文件。这个函数有两个版本，一个是加载文件中的zip，另一个是加载资源中的zip，我们需要的就是第二个版本的SetResourceZip。程序调用ReloadSkin函数后，会通知所有控件去重新加载图片资源，图片资源的加载会通过LoadImage函数，这个函数会根据加载资源类型的不同而去选择从不同的地方去试图找到资源并加载。

在使用“资源的zip压缩包”方法的前提下，如果要换肤就使用如下函数，函数的参数是新换皮肤的资源ID，比如“IDR_ZIPRES2”，函数实际就是从程序资源中找到对应的皮肤zip文件，并且调用对应的SetResourceZip函数加载资源：

```cpp
void CFrameWnd::ReloadZipResource(int ID)
{
 
	HRSRC hResource = ::FindResource(m_PaintManager.GetResourceDll(), MAKEINTRESOURCE(ID), _T("ZIPRES"));
	if( hResource == NULL )
		return ;
	DWORD dwSize = 0;
	HGLOBAL hGlobal = ::LoadResource(m_PaintManager.GetResourceDll(), hResource);
	if( hGlobal == NULL ) 
	{
#if defined(WIN32) && !defined(UNDER_CE)
		::FreeResource(hResource);
#endif
		return ;
	}
	dwSize = ::SizeofResource(m_PaintManager.GetResourceDll(), hResource);
	if( dwSize == 0 )
		return ;
 
	CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
 
#if defined(WIN32) && !defined(UNDER_CE)
	::FreeResource(hResource);
#endif
 
	CPaintManagerUI::ReloadSkin();
}
```
理论上这就应该就可以了，但是实际测试还有问题，后来发现是SetResourceZip函数的定义有些问题：

可以看到如果使用资源zip文件，那么m_pStrResourceZip变量就会保存_T("membuffer")字符串，当再次调用SetResourceZip函数时，由于第一句代码的判断就会导致函数直接返回，所以这里直接注释掉第一句代码就可以了。

至此，就可以使用资源中的zip文件来换肤了，两全其美。这里还可以扩展，可以把zip资源都继承到一个dll文件中，然后在加载函数里先加载dll，然后从dll加载资源，这样既可以让皮肤资源独立为文件，加载也快速，并且资源也安全。这个代码很好写，我这里就不提供了。



