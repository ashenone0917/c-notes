# CreateProcess使父进程被杀死时子进程被终止(通过job)
```cpp
// Job.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <tchar.h>
TCHAR g_strCmdPath[] = _T("C:\\WINDOWS\\system32\\notepad.exe");
int main()
{
    HANDLE hJob = ::CreateJobObject(0, 0);
    if (hJob) {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli{};
        jeli.BasicLimitInformation.LimitFlags = 
            JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE | JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;
        if (::SetInformationJobObject(
            hJob, 
            JobObjectExtendedLimitInformation, 
            &jeli, 
            sizeof(jeli))) {
            PROCESS_INFORMATION pi{};
            STARTUPINFO si{};
            si.cb = sizeof(STARTUPINFO);
            bool b = ::CreateProcess(
                NULL,
                g_strCmdPath,
                NULL,
                NULL,
                FALSE,
                CREATE_BREAKAWAY_FROM_JOB | CREATE_SUSPENDED,
                NULL,
                NULL,
                &si,
                &pi);
            if (b) {
                if (::AssignProcessToJobObject(hJob, pi.hProcess)) {
                    ::ResumeThread(pi.hThread);
                }
                ::CloseHandle(pi.hProcess);
                ::CloseHandle(pi.hThread);
            }
        }
    }
    //子进程要存活就不要Close job，最好是等父进程结束或者析构函数在Close job
    getchar();
    //Close掉JOB之后子进程就死了
    ::CloseHandle(hJob);
}
```
