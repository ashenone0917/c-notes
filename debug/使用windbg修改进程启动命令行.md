1.使用```!peb```查看当前当前进程peb
```
PEB at 7efde000
    InheritedAddressSpace:    No
    ReadImageFileExecOptions: No
    BeingDebugged:            Yes
    ImageBaseAddress:         01370000
    Ldr                       77460200
    Ldr.Initialized:          Yes
    Ldr.InInitializationOrderModuleList: 003d2ea0 . 003e10c0
    Ldr.InLoadOrderModuleList:           003d2e10 . 003e10b0
    Ldr.InMemoryOrderModuleList:         003d2e18 . 003e10b8
            Base TimeStamp                     Module
         1370000 64184f3f Mar 20 20:19:11 2023 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\CptHost.exe
        77360000 4a5bdb3b Jul 14 09:11:23 2009 C:\Windows\SysWOW64\ntdll.dll
        75cb0000 4a5bdbde Jul 14 09:14:06 2009 C:\Windows\syswow64\kernel32.dll
        76780000 4a5bdbdf Jul 14 09:14:07 2009 C:\Windows\syswow64\KERNELBASE.dll
        75db0000 4a5bdb3b Jul 14 09:11:23 2009 C:\Windows\syswow64\RPCRT4.dll
        74ed0000 4a5bdb3b Jul 14 09:11:23 2009 C:\Windows\syswow64\SspiCli.dll
        76640000 4a5bda6f Jul 14 09:07:59 2009 C:\Windows\syswow64\msvcrt.dll
        74ec0000 4a5bbf41 Jul 14 07:12:01 2009 C:\Windows\syswow64\CRYPTBASE.dll
        75fe0000 4a5bdb04 Jul 14 09:10:28 2009 C:\Windows\SysWOW64\sechost.dll
        74900000 4a5bdb42 Jul 14 09:11:30 2009 C:\Windows\system32\WINMM.dll
        76e60000 4a5bdb3c Jul 14 09:11:24 2009 C:\Windows\syswow64\USER32.dll
        763d0000 4a5bdb38 Jul 14 09:11:20 2009 C:\Windows\syswow64\GDI32.dll
        77330000 4a5bdb3b Jul 14 09:11:23 2009 C:\Windows\syswow64\LPK.dll
        75f30000 4a5bdb32 Jul 14 09:11:14 2009 C:\Windows\syswow64\USP10.dll
        76490000 4a5bd97e Jul 14 09:03:58 2009 C:\Windows\syswow64\ADVAPI32.dll
        76090000 4a5bdb39 Jul 14 09:11:21 2009 C:\Windows\syswow64\IMM32.dll
        761c0000 4a5bda69 Jul 14 09:07:53 2009 C:\Windows\syswow64\MSCTF.dll
        73e70000 4a5bd9a9 Jul 14 09:04:41 2009 C:\Windows\system32\d3d9.dll
        74c20000 4a5bdb2b Jul 14 09:11:07 2009 C:\Windows\system32\VERSION.dll
        73e60000 4a5bd9a8 Jul 14 09:04:40 2009 C:\Windows\system32\d3d8thk.dll
        748a0000 4a5bda07 Jul 14 09:06:15 2009 C:\Windows\system32\dwmapi.dll
        74d50000 6401850b Mar 03 13:26:35 2023 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\zCrashReport.dll
        75050000 4a5bdb01 Jul 14 09:10:25 2009 C:\Windows\syswow64\SHELL32.dll
        75ea0000 4a5bdb05 Jul 14 09:10:29 2009 C:\Windows\syswow64\SHLWAPI.dll
        76840000 4a5bdac7 Jul 14 09:09:27 2009 C:\Windows\syswow64\ole32.dll
        75f00000 4a5bdb52 Jul 14 09:11:46 2009 C:\Windows\syswow64\WINTRUST.dll
        76b40000 4a5bda39 Jul 14 09:07:05 2009 C:\Windows\syswow64\CRYPT32.dll
        74f30000 4a5bda60 Jul 14 09:07:44 2009 C:\Windows\syswow64\MSASN1.dll
        74cd0000 5fa9a5de Nov 10 04:26:06 2020 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\MSVCP140.dll
        74d70000 5fa9a5d8 Nov 10 04:26:00 2020 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\VCRUNTIME140.dll
        74cc0000 4f95322e Apr 23 18:42:54 2012 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-runtime-l1-1-0.dll
        67480000 747f8dcc Dec 09 01:13:48 2031 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\ucrtbase.DLL
        74e30000 46f1669c Sep 20 02:12:44 2007 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-core-localization-l1-2-0.dll
        74a40000 49b01797 Mar 06 02:19:03 2009 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-core-processthreads-l1-1-1.dll
        67fc0000 6f524192 Mar 08 18:55:14 2029 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-core-file-l1-2-0.dll
        676b0000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-core-timezone-l1-1-0.dll
        676a0000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-core-file-l2-1-0.dll
        67690000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-core-synch-l1-2-0.dll
        67680000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-heap-l1-1-0.dll
        67670000 13e34ada Jul 29 04:08:58 1980 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-string-l1-1-0.dll
        67660000 3f6ff349 Sep 23 15:16:25 2003 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-stdio-l1-1-0.dll
        67650000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-convert-l1-1-0.dll
        67640000 494adeda Dec 19 07:38:02 2008 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-locale-l1-1-0.dll
        67630000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-filesystem-l1-1-0.dll
        67620000 21630f12 Oct 02 10:27:30 1987 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-time-l1-1-0.dll
        67610000 5b5ddf00 Jul 29 23:36:32 2018 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-environment-l1-1-0.dll
        67600000 3eff54e6 Jun 30 05:06:46 2003 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-math-l1-1-0.dll
        675f0000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-utility-l1-1-0.dll
        765b0000 4a5bdaca Jul 14 09:09:30 2009 C:\Windows\syswow64\OLEAUT32.dll
        74550000 4a5bdac8 Jul 14 09:09:28 2009 C:\Windows\system32\OLEACC.dll
        652d0000 4a5bd976 Jul 14 09:03:50 2009 C:\Windows\WinSxS\x86_microsoft.windows.gdiplus_6595b64144ccf1df_1.1.7600.16385_none_72fc7cbf861225ca\gdiplus.dll
        769a0000 4a5bdafe Jul 14 09:10:22 2009 C:\Windows\syswow64\SETUPAPI.dll
        76190000 4a5bd9a6 Jul 14 09:04:38 2009 C:\Windows\syswow64\CFGMGR32.dll
        767d0000 4a5bd9c5 Jul 14 09:05:09 2009 C:\Windows\syswow64\DEVOBJ.dll
        75fd0000 4a5bdace Jul 14 09:09:34 2009 C:\Windows\syswow64\PSAPI.DLL
    SubSystemData:     00000000
    ProcessHeap:       003d0000
    ProcessParameters: 003d15d0//这个结构
    CurrentDirectory:  'C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\'
    WindowTitle:  'C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\CptHost.exe'
    ImageFile:    'C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\CptHost.exe'
    CommandLine:  '"C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\CptHost.exe" -event 000007C0 -pid 4300 -evtname cpthost.exe4300-41-00924D40 -exitevent 000007C4 -exitevtname cpthost.exe4300_rpcexit-41-00924D40 -user_path "C:\Users\Admin\AppData\Roaming\Zoom"'
    DllPath:      'C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin;;C:\Windows\system32;C:\Windows\system;C:\Windows;.;C:\Program Files (x86)\Debugging Tools for Windows (x86)\winext\arcade;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\'
    Environment:  003d07f0
        =::=::\
        =C:=C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin
        ALLUSERSPROFILE=C:\ProgramData
        APPDATA=C:\Users\Admin\AppData\Roaming
        CommonProgramFiles=C:\Program Files (x86)\Common Files
        CommonProgramFiles(x86)=C:\Program Files (x86)\Common Files
        CommonProgramW6432=C:\Program Files\Common Files
        COMPUTERNAME=WIN-CAT2JCEIQBJ
        ComSpec=C:\Windows\system32\cmd.exe
        FP_NO_HOST_CHECK=NO
        HOMEDRIVE=C:
        HOMEPATH=\Users\Admin
        LOCALAPPDATA=C:\Users\Admin\AppData\Local
        LOGONSERVER=\\WIN-CAT2JCEIQBJ
        MpConfig_ProductAppDataPath=C:\ProgramData\Microsoft\Windows Defender
        MpConfig_ProductCodeName=AntiSpyware
        MpConfig_ProductPath=C:\Program Files\Windows Defender
        MpConfig_ProductUserAppDataPath=C:\Users\Admin\AppData\Local\Microsoft\Windows Defender
        MpConfig_ReportingGUID=3CF7EE71-4F7C-46D4-A9D6-5FC2CB704762
        NUMBER_OF_PROCESSORS=1
        OS=Windows_NT
        Path=C:\Program Files (x86)\Debugging Tools for Windows (x86)\winext\arcade;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\
        PATHEXT=.COM;.EXE;.BAT;.CMD;.VBS;.VBE;.JS;.JSE;.WSF;.WSH;.MSC
        PROCESSOR_ARCHITECTURE=x86
        PROCESSOR_ARCHITEW6432=AMD64
        PROCESSOR_IDENTIFIER=Intel64 Family 6 Model 151 Stepping 2, GenuineIntel
        PROCESSOR_LEVEL=6
        PROCESSOR_REVISION=9702
        ProgramData=C:\ProgramData
        ProgramFiles=C:\Program Files (x86)
        ProgramFiles(x86)=C:\Program Files (x86)
        ProgramW6432=C:\Program Files
        PROMPT=$P$G
        PSModulePath=C:\Windows\system32\WindowsPowerShell\v1.0\Modules\
        PUBLIC=C:\Users\Public
        SESSIONNAME=Console
        SystemDrive=C:
        SystemRoot=C:\Windows
        TEMP=C:\Users\Admin\AppData\Local\Temp
        TMP=C:\Users\Admin\AppData\Local\Temp
        USERDOMAIN=WIN-CAT2JCEIQBJ
        USERNAME=Admin
        USERPROFILE=C:\Users\Admin
        WINDBG_DIR=C:\Program Files (x86)\Debugging Tools for Windows (x86)
        windir=C:\Windows
```
使用```dt _peb 7efde000 ProcessParameters```查看ProcessParameters的在peb中的偏移和保存的地址
```
0:000> dt _peb 7efde000 ProcessParameters
ntdll!_PEB
   +0x010 ProcessParameters : 0x003d15d0 _RTL_USER_PROCESS_PARAMETERS//从这里得知ProcessParameters的结构是_RTL_USER_PROCESS_PARAMETERS
```

使用```dt _RTL_USER_PROCESS_PARAMETERS 0x003d15d0 CommandLine```获取CommandLine在_RTL_USER_PROCESS_PARAMETERS中的偏移和内容
```
0:000> dt _RTL_USER_PROCESS_PARAMETERS 0x003d15d0 CommandLine
ntdll!_RTL_USER_PROCESS_PARAMETERS
   +0x040 CommandLine : _UNICODE_STRING ""C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\CptHost.exe" -event 000007C0 -pid 4300 -evtname cpthost.exe4300-41-00924D40 -exitevent 000007C4 -exitevtname cpthost.exe4300_rpcexit-41-00924D40 -user_path "C:\Users\Admin\AppData\Roaming\Zoom""
可以看出CommandLine的结构是_UNICODE_STRING

//_RTL_USER_PROCESS_PARAMETERS的结构
typedef struct _RTL_USER_PROCESS_PARAMETERS {
  BYTE           Reserved1[16];
  PVOID          Reserved2[10];
  UNICODE_STRING ImagePathName;
  UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;
```

使用```dt _UNICODE_STRING 0x003d15d0+0x40 Buffer```查看buffer在_UNICODE_STRING中的偏移以及保存的地址
```
0:000> dt _UNICODE_STRING 0x003d15d0+0x40 Buffer
ntdll!_UNICODE_STRING
 ""C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\CptHost.exe" -event 000007C0 -pid 4300 -evtname cpthost.exe4300-41-00924D40 -exitevent 000007C4 -exitevtname cpthost.exe4300_rpcexit-41-00924D40 -user_path "C:\Users\Admin\AppData\Roaming\Zoom""
   +0x004 Buffer : 0x003d1d1a  ""C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\CptHost.exe" -event 000007C0 -pid 4300 -evtname cpthost.exe4300-41-00924D40 -exitevent 000007C4 -exitevtname cpthost.exe4300_rpcexit-41-00924D40 -user_path "C:\Users\Admin\AppData\Roaming\Zoom""
//Buffer保存的地址0x003d1d1a
```

本次的做法是直接除命令行前面的那一段二进制路径
```
//所以先确认一下长度，前面那一段二进制路径长度为0x86
0:000> du 0x003d1d1a+0x86
003d1da0  "-event 000007C0 -pid 4300 -evtna"
003d1de0  "me cpthost.exe4300-41-00924D40 -"
003d1e20  "exitevent 000007C4 -exitevtname "
003d1e60  "cpthost.exe4300_rpcexit-41-00924"
003d1ea0  "D40 -user_path "C:\Users\Admin\A"
003d1ee0  "ppData\Roaming\Zoom""
```

确认一下_UNICODE_STRING中的长度偏移
```
0:000> dt _UNICODE_STRING 0x003d15d0+0x40 Length
ntdll!_UNICODE_STRING
 ""C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\CptHost.exe" -event 000007C0 -pid 4300 -evtname cpthost.exe4300-41-00924D40 -exitevent 000007C4 -exitevtname cpthost.exe4300_rpcexit-41-00924D40 -user_path "C:\Users\Admin\AppData\Roaming\Zoom""
   +0x000 Length : 0x1ee
```

直接修改成修改字符串的长度
```
0:000> ew 0x003d15d0+0x40+0x0 0x168
0x168 = 0x1ee - 0x86
```
然后修改buffer的地址
```
0:000> ed 0x003d15d0+0x40+0x4 0x003d1da0
//0x003d1da0 = 0x003d1d1a+0x86
```

再次查看peb可以发现CommandLine中二进制的路径已经没了
```
0:000> !peb
PEB at 7efde000
    InheritedAddressSpace:    No
    ReadImageFileExecOptions: No
    BeingDebugged:            Yes
    ImageBaseAddress:         01370000
    Ldr                       77460200
    Ldr.Initialized:          Yes
    Ldr.InInitializationOrderModuleList: 003d2ea0 . 003e10c0
    Ldr.InLoadOrderModuleList:           003d2e10 . 003e10b0
    Ldr.InMemoryOrderModuleList:         003d2e18 . 003e10b8
            Base TimeStamp                     Module
         1370000 64184f3f Mar 20 20:19:11 2023 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\CptHost.exe
        77360000 4a5bdb3b Jul 14 09:11:23 2009 C:\Windows\SysWOW64\ntdll.dll
        75cb0000 4a5bdbde Jul 14 09:14:06 2009 C:\Windows\syswow64\kernel32.dll
        76780000 4a5bdbdf Jul 14 09:14:07 2009 C:\Windows\syswow64\KERNELBASE.dll
        75db0000 4a5bdb3b Jul 14 09:11:23 2009 C:\Windows\syswow64\RPCRT4.dll
        74ed0000 4a5bdb3b Jul 14 09:11:23 2009 C:\Windows\syswow64\SspiCli.dll
        76640000 4a5bda6f Jul 14 09:07:59 2009 C:\Windows\syswow64\msvcrt.dll
        74ec0000 4a5bbf41 Jul 14 07:12:01 2009 C:\Windows\syswow64\CRYPTBASE.dll
        75fe0000 4a5bdb04 Jul 14 09:10:28 2009 C:\Windows\SysWOW64\sechost.dll
        74900000 4a5bdb42 Jul 14 09:11:30 2009 C:\Windows\system32\WINMM.dll
        76e60000 4a5bdb3c Jul 14 09:11:24 2009 C:\Windows\syswow64\USER32.dll
        763d0000 4a5bdb38 Jul 14 09:11:20 2009 C:\Windows\syswow64\GDI32.dll
        77330000 4a5bdb3b Jul 14 09:11:23 2009 C:\Windows\syswow64\LPK.dll
        75f30000 4a5bdb32 Jul 14 09:11:14 2009 C:\Windows\syswow64\USP10.dll
        76490000 4a5bd97e Jul 14 09:03:58 2009 C:\Windows\syswow64\ADVAPI32.dll
        76090000 4a5bdb39 Jul 14 09:11:21 2009 C:\Windows\syswow64\IMM32.dll
        761c0000 4a5bda69 Jul 14 09:07:53 2009 C:\Windows\syswow64\MSCTF.dll
        73e70000 4a5bd9a9 Jul 14 09:04:41 2009 C:\Windows\system32\d3d9.dll
        74c20000 4a5bdb2b Jul 14 09:11:07 2009 C:\Windows\system32\VERSION.dll
        73e60000 4a5bd9a8 Jul 14 09:04:40 2009 C:\Windows\system32\d3d8thk.dll
        748a0000 4a5bda07 Jul 14 09:06:15 2009 C:\Windows\system32\dwmapi.dll
        74d50000 6401850b Mar 03 13:26:35 2023 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\zCrashReport.dll
        75050000 4a5bdb01 Jul 14 09:10:25 2009 C:\Windows\syswow64\SHELL32.dll
        75ea0000 4a5bdb05 Jul 14 09:10:29 2009 C:\Windows\syswow64\SHLWAPI.dll
        76840000 4a5bdac7 Jul 14 09:09:27 2009 C:\Windows\syswow64\ole32.dll
        75f00000 4a5bdb52 Jul 14 09:11:46 2009 C:\Windows\syswow64\WINTRUST.dll
        76b40000 4a5bda39 Jul 14 09:07:05 2009 C:\Windows\syswow64\CRYPT32.dll
        74f30000 4a5bda60 Jul 14 09:07:44 2009 C:\Windows\syswow64\MSASN1.dll
        74cd0000 5fa9a5de Nov 10 04:26:06 2020 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\MSVCP140.dll
        74d70000 5fa9a5d8 Nov 10 04:26:00 2020 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\VCRUNTIME140.dll
        74cc0000 4f95322e Apr 23 18:42:54 2012 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-runtime-l1-1-0.dll
        67480000 747f8dcc Dec 09 01:13:48 2031 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\ucrtbase.DLL
        74e30000 46f1669c Sep 20 02:12:44 2007 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-core-localization-l1-2-0.dll
        74a40000 49b01797 Mar 06 02:19:03 2009 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-core-processthreads-l1-1-1.dll
        67fc0000 6f524192 Mar 08 18:55:14 2029 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-core-file-l1-2-0.dll
        676b0000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-core-timezone-l1-1-0.dll
        676a0000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-core-file-l2-1-0.dll
        67690000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-core-synch-l1-2-0.dll
        67680000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-heap-l1-1-0.dll
        67670000 13e34ada Jul 29 04:08:58 1980 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-string-l1-1-0.dll
        67660000 3f6ff349 Sep 23 15:16:25 2003 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-stdio-l1-1-0.dll
        67650000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-convert-l1-1-0.dll
        67640000 494adeda Dec 19 07:38:02 2008 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-locale-l1-1-0.dll
        67630000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-filesystem-l1-1-0.dll
        67620000 21630f12 Oct 02 10:27:30 1987 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-time-l1-1-0.dll
        67610000 5b5ddf00 Jul 29 23:36:32 2018 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-environment-l1-1-0.dll
        67600000 3eff54e6 Jun 30 05:06:46 2003 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-math-l1-1-0.dll
        675f0000 C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\api-ms-win-crt-utility-l1-1-0.dll
        765b0000 4a5bdaca Jul 14 09:09:30 2009 C:\Windows\syswow64\OLEAUT32.dll
        74550000 4a5bdac8 Jul 14 09:09:28 2009 C:\Windows\system32\OLEACC.dll
        652d0000 4a5bd976 Jul 14 09:03:50 2009 C:\Windows\WinSxS\x86_microsoft.windows.gdiplus_6595b64144ccf1df_1.1.7600.16385_none_72fc7cbf861225ca\gdiplus.dll
        769a0000 4a5bdafe Jul 14 09:10:22 2009 C:\Windows\syswow64\SETUPAPI.dll
        76190000 4a5bd9a6 Jul 14 09:04:38 2009 C:\Windows\syswow64\CFGMGR32.dll
        767d0000 4a5bd9c5 Jul 14 09:05:09 2009 C:\Windows\syswow64\DEVOBJ.dll
        75fd0000 4a5bdace Jul 14 09:09:34 2009 C:\Windows\syswow64\PSAPI.DLL
    SubSystemData:     00000000
    ProcessHeap:       003d0000
    ProcessParameters: 003d15d0
    CurrentDirectory:  'C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\'
    WindowTitle:  'C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\CptHost.exe'
    ImageFile:    'C:\Users\Admin\Downloads\Zoom_x86_14\Zoom_x86_14\bin\CptHost.exe'
    CommandLine:  '-event 000007C0 -pid 4300 -evtname cpthost.exe4300-41-00924D40 -exitevent 000007C4 -exitevtname cpthost.exe4300_rpcexit-41-00924D40 -user_path "C:\Users\Admin\AppData\Roaming\Zoom"'
    ```
