```cpp
 typedef LPWSTR(WINAPI *pfn_BrandingFormatString)(LPCWSTR);

 HMODULE hWinbrand = LoadLibraryW(L"winbrand.dll");
 pfn_BrandingFormatString BrandingFormatString = (pfn_BrandingFormatString)GetProcAddress(hWinbrand, "BrandingFormatString");

 LPWSTR WinVersionStr = BrandingFormatString(L"%WINDOWS_LONG%");
 printf("winversion:%ws.", WinVersionStr);
 GlobalFree(WinVersionStr);
```
