```cpp
#include <Windows.h>
#include <ShlObj.h>
#include <sstream>
#include "Shlwapi.h"

#include <iostream>
#include <IntShCut.h>
#include <propvarutil.h>

typedef struct SHORTCUTSTRUCT_ {
	LPCTSTR pszTarget;//链接的文件
	LPCTSTR pszDesc;
	LPCTSTR pszArguments;//命令行参数
	WORD wHotKey;
	LPCTSTR pszIconPath;//创建的图标路径
	WORD wIconIndex;//默认为0
}SHORTCUTSTRUCT, * LPSHORTCUTSTRUCT;

//szLnkFile是快捷方式的路径
BOOL CreateLink(LPCTSTR szLnkFile, LPSHORTCUTSTRUCT lpss)
{
	BOOL bRet = FALSE;
	HRESULT hResult = NULL;
	IShellLink* pShell = NULL;
	IPersistFile* pPersistFile = NULL;

	::CoInitialize(NULL);

	do
	{
		if (TRUE == PathFileExists(szLnkFile)) break;

		hResult = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pShell);
		if (FAILED(hResult)) break;

		if (lpss->pszTarget) pShell->SetPath(lpss->pszTarget);
		if (lpss->pszDesc) pShell->SetDescription(lpss->pszDesc);
		if (lpss->pszIconPath) pShell->SetIconLocation(lpss->pszIconPath, lpss->wIconIndex);
		if (lpss->wHotKey) pShell->SetHotkey(lpss->wHotKey);
		if (lpss->pszArguments) pShell->SetArguments(lpss->pszArguments);

		hResult = pShell->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistFile);
		if (FAILED(hResult)) break;

		hResult = pPersistFile->Save(szLnkFile, STGM_READWRITE);
		if (S_OK == hResult) bRet = TRUE;

	} while (0);

	if (pShell) pShell->Release();
	if (pPersistFile)pPersistFile->Release();

	::CoUninitialize();

	return bRet;
}

//创建指向url的快捷方式
void CreateInternetShortcut(LPCTSTR pszURL, LPCTSTR pszShortcut, LPCTSTR pszIcon)
{
	::CoInitialize(NULL);

	IUniformResourceLocatorW* url = nullptr;
	if (SUCCEEDED(CoCreateInstance(CLSID_InternetShortcut, nullptr, CLSCTX_INPROC_SERVER, IID_IUniformResourceLocatorW, (LPVOID*)&url)))
	{
		if (SUCCEEDED(url->SetURL(pszURL, IURL_SETURL_FL_GUESS_PROTOCOL)))
		{
			IPropertySetStorage* pss = nullptr;
			if (SUCCEEDED(url->QueryInterface(IID_IPropertySetStorage, (void**)&pss)))
			{
				IPropertyStorage* ps = nullptr;
				if (SUCCEEDED(pss->Open(FMTID_Intshcut, STGM_WRITE, &ps)))
				{
					// 设置两个属性，一个图标文件路径，一个图标索引
					PROPSPEC pids[2] = { { PRSPEC_PROPID, PID_IS_ICONFILE }, { PRSPEC_PROPID, PID_IS_ICONINDEX } };
					PROPVARIANT pvar[2];

					// 图标文件指向Chrome
					InitPropVariantFromString(pszIcon, &pvar[0]);

					// 图标索引设置为1，也就是第一个图标
					InitPropVariantFromInt16(0, &pvar[1]);

					ULONG n = sizeof(pids) / sizeof(pids[0]);

					// 写入属性
					ps->WriteMultiple(n, pids, pvar, 0);

					// 提交修改
					ps->Commit(STGC_DEFAULT);

					ps->Release();
					PropVariantClear(&pvar[0]);
					PropVariantClear(&pvar[1]);
				}
				pss->Release();
			}

			IPersistFile* pf = nullptr;
			if (SUCCEEDED(url->QueryInterface(IID_IPersistFile, (void**)&pf)))
			{
				WCHAR wszShortcut[MAX_PATH];
#ifdef UNICODE
				lstrcpyn(wszShortcut, pszShortcut, MAX_PATH);
#else
				MultiByteToWideChar(CP_ACP,
					0,
					pszShortcut,
					-1,
					wszShortcut,
					MAX_PATH);
#endif
				pf->Save(wszShortcut, TRUE);
				pf->Release();
			}
		}
		url->Release();
	}

	CoUninitialize();
}

//HRESULT CreateInternetShortcut(LPCTSTR pszShortcut, LPCTSTR pszURL)
//{
//	IUniformResourceLocator* purl;
//	HRESULT                 hr;
//
//	hr = CoInitialize(NULL);
//
//	if (SUCCEEDED(hr))
//	{
//		//Get a pointer to the IShellLink interface. 
//		hr = CoCreateInstance(CLSID_InternetShortcut,
//			NULL,
//			CLSCTX_INPROC_SERVER,
//			IID_IUniformResourceLocator,
//			(LPVOID*)&purl);
//
//		if (SUCCEEDED(hr))
//		{
//			IPersistFile* ppf;
//			hr = purl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
//			if (SUCCEEDED(hr))
//			{
//				hr = purl->SetURL(pszURL, 0);
//				if (SUCCEEDED(hr))
//				{
//					WCHAR wszShortcut[MAX_PATH];
//#ifdef UNICODE
//					lstrcpyn(wszShortcut, pszShortcut, MAX_PATH);
//#else
//					MultiByteToWideChar(CP_ACP,
//						0,
//						pszShortcut,
//						-1,
//						wszShortcut,
//						MAX_PATH);
//#endif
//					hr = ppf->Save(wszShortcut, FALSE);
//				}
//				ppf->Release();
//			}
//			purl->Release();
//		}
//
//		CoUninitialize();
//	}
//
//	return hr;
//}

int main()
{
	TCHAR desktop_path[MAX_PATH]{};
	if (FALSE == SHGetSpecialFolderPath(0, desktop_path, CSIDL_DESKTOPDIRECTORY, 0));
	std::wostringstream link_path;
	link_path << desktop_path << L"\\" << L"DACS远程访问指引.url";
	SHORTCUTSTRUCT temp{};
	temp.pszTarget = L"www.baidu.com";
	//CreateInternetShortcut(link_path.str().c_str(), L"www.baidu.com");
	CreateInternetShortcut(L"www.baidu.com", link_path.str().c_str(), L"C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe");

	int n = 10;
}
```
