下面是是一个x.reg文件的内容，会自动写入注册表
```cpp
Windows Registry Editor Version 5.00

[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps]
"DumpCount"=dword:0000000a
"DumpFolder"=hex(2):44,00,3a,00,5c,00,44,00,75,00,6d,00,70,00,00,00
"DumpType"=dword:00000002
```
待注册表写入完毕后，进入该路径，找到DumpFolder项，修改内容为dump生成的路径即可

