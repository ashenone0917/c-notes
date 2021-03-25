```cpp
// ini.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <tchar.h>
#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif // _UNICODE


DWORD GetPrivateProfileString_s(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,
    LPCTSTR lpDefault,
    tstring & lpReturnedString,
    LPCTSTR lpFileName)
{
    if (!lpAppName || !lpKeyName) return ERROR_INVALID_PARAMETER;

    DWORD dwReturn = ERROR_NOT_ENOUGH_MEMORY;
    DWORD dwBufferLen = 1024;
    LPTSTR pBuffer = (LPTSTR)::malloc(sizeof(TCHAR) * dwBufferLen);
    if (pBuffer == NULL) return dwReturn;
    ZeroMemory(pBuffer, sizeof(TCHAR) * dwBufferLen);

    do {
        dwReturn = GetPrivateProfileString(
            lpAppName,
            lpKeyName,
            lpDefault,
            pBuffer,
            dwBufferLen,
            lpFileName);

        if(lpAppName && lpKeyName && dwReturn == dwBufferLen - 1) {
           dwBufferLen = dwBufferLen << 1;
           auto pTemp = (LPTSTR)::realloc(pBuffer, sizeof(TCHAR) * dwBufferLen);
           if (pTemp == NULL) {
               dwReturn = ERROR_NOT_ENOUGH_MEMORY;
               break;
           }
           pBuffer = pTemp;
           continue;
        }
        
        if (pBuffer != NULL) {
            lpReturnedString = pBuffer;
            ::free(pBuffer);
            pBuffer = NULL;
        }   

        break;

    } while (1);
    
    return dwReturn;
}
int main()
{
    tstring test;
    GetPrivateProfileString_s(_T("LiMing"), _T("Sex"), _T(""), test, _T("C:\\Users\\caiyanchao\\source\\repos\\ConsoleApplication1\\ini\\test.ini"));
    std::cout << "Hello World!\n";
}
```
