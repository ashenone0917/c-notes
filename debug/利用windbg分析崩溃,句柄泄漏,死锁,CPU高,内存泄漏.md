## 崩溃
1. 加符号
2. !analyze -v
3. .excr 切换到异常线程
4. k查看堆栈

## 句柄泄露
1. windbg附加进程(gflags.exe启动附加)
2. !htrace -enable启动句柄检测
3. !htrace -snapshot
4. 运行一段时间，待句柄数升高
5. break
6. !htrace -diff 可以查看哪些地方分配了句柄没释放
7. ![信息如下](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxMTU4ODg0P3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDT.png)
8. 输入lsa handleLeak!ThreadProc1+0x00000037  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxMjIzNTMwP3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
这样就可以看出代码中在不停CreateEvent  
补充：  
可以在windbg调式中，输入!handle可以得到当前堆栈的一些句柄信息，可以看出这个堆栈event非常多。  
## 死锁
1. 启动进程
2. windbg附加
3. 输入```~*kv```，输出所有线程
4. 输入!findstackntdll!RtlEnterCriticalSection,查找哪些线程在等待锁,或者看代码某一函数没执行,对比windbg中的线程,找到线程id分析
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxMzAyNjcwP3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxMzEyMjc1P3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
图1是源代码,图2是执行结果, ThreadProc1函数中的” ThreadProc1 lock g_mutex2”没发生,怀疑是否死锁了
5. windbg中线程信息如下，发现ThreadProc1在等某一把锁  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxMzM2NTYzP3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
第三帧是本地代码对比源代码发现在等锁g_mutex2;  
第二帧是系统函数在等待锁,锁地址为00bf7140  
6. 输入!cs 00bf7140，查看这把锁信息  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxMzU0NTg2P3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
发现锁的占有者是0x00002cc4
7. 输入```~~[0x00002cc4]```，发现对应是3号线程  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxNDEwMTk0P3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
8. 切到3号线程，并输出堆栈  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxNDIwMjY0P3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
发现代码27号，也在等一把锁，锁地址00bf7158
9. 同理输出锁信息  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxNDQ1NzM5P3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
10. 发现锁占有者0x00004c80，切到线程0x00004c80，并输出堆栈  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxNTAwMzM5P3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
发现是刚才的2号线程  
至此分析完成2号线程和3号线程发生死锁。  
**备注：也可以直接使用命令```!locks```查看死锁**
1. !locks命令获取死锁信息
```cpp
0:000> !locks

CritSec XXX!g_aaa_tm+0 at 00007ff62a6cccb8
WaiterWoken        No
LockCount          12
RecursionCount     1
OwningThread       7e0c  //持有锁的线程id
EntryCount         0
ContentionCount    91f0
*** Locked

CritSec XXX!g_bbb_tm+0 at 00007ff62a6ccd60
WaiterWoken        No
LockCount          5
RecursionCount     1
OwningThread       4e6c //持有锁的线程id
EntryCount         0
ContentionCount    1c
*** Locked

Scanned 80 critical sections
```
2. 将线程id转换为windbg里的线程号
```cpp
0:000> ~~[7e0c]
   8  Id: 4830.7e0c Suspend: 0 Teb: 00007ff6`29368000 Unfrozen
      Start: XXX!thread_run (00007ff6`2a42a7f0) 
      Priority: 0  Priority class: 32  Affinity: 3f
0:000> ~~[4e6c]
  92  Id: 4830.4e6c Suspend: 0 Teb: 00007ff6`292ac000 Unfrozen
      Start: XXX!thread_run (00007ff6`2a42a7f0) 
      Priority: 0  Priority class: 32  Affinity: 3f
```
3. 查看线程堆栈
```cpp
0:000> ~8kv
Child-SP          RetAddr           : Args to Child                                                           : Call Site
00000000`0adce5b8 00007fff`59a52e2d : 00000000`00e22f50 00000000`62f94642 00000000`081edfb8 00000000`0adce7b0 : ntdll!ZwWaitForSingleObject+0xa
00000000`0adce5c0 00007fff`59a52803 : 00000000`0adcef80 00000000`000000d7 00007ff6`2a6ccd60 00000000`00000000 : ntdll!RtlpWaitOnCriticalSection+0xe1
00000000`0adce690 00007ff6`2a3e0f7a : 00000000`00000000 00000000`00e22f50 00000000`07926252 00000000`0adce810 : ntdll!RtlpEnterCriticalSectionContended+0x93
00000000`0adce6d0 00007ff6`2a3e17ba : 00000000`00e25860 00000000`00e25860 00000000`00e22f50 00000000`5d275d00 : XXX!o+0x2a [d:\a\b\c\d\e\g\i.cpp @ 29]
00000000`0adce760 00007ff6`29fc997c : 00000000`00e66380 00000000`00e66380 00000000`07926242 00000000`0000001f : XXX!p+0x6ea [d:\a\b\c\d\e\g\i.cpp @ 99]
00000000`0adcf3c0 00007ff6`29fb9ee2 : 00007ff6`29fc92d0 00000000`0adcfcf0 00000000`00e29740 00007ff6`29ef4c4a : XXX!A::q+0x6ac [d:\a\b\c\d\e\h\j\l.cpp @ 3908]
00000000`0adcfbd0 00007ff6`29fb7247 : 00000000`0000000a 00000000`0000006c 00000000`079261c0 00000000`00000000 : XXX!A::r+0x572 [d:\a\b\c\d\e\h\j\l.cpp @ 660]
00000000`0adcfc90 00007ff6`29fb7049 : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : XXX!s+0x197 [d:\a\b\c\d\e\h\k.cpp @ 362]
00000000`0adcfd50 00007ff6`2a42a825 : 00000000`00000000 00000000`0000006c 00000000`079261c0 00000000`079120b0 : XXX!B::run+0xf9 [d:\a\b\c\d\e\h\m.cpp @ 317]
00000000`0adcfda0 00007fff`59201611 : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : XXX!thread_run+0x35 [d:\a\b\c\d\f\n.cpp @ 178]
00000000`0adcfdd0 00007fff`59a964ad : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : kernel32!BaseThreadInitThunk+0xd
00000000`0adcfe00 00000000`00000000 : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : ntdll!RtlUserThreadStart+0x1d

0:000> ~92kv
Child-SP          RetAddr           : Args to Child                                                           : Call Site
00000000`2bd4ed38 00007fff`59a52e2d : 00000000`07b1f4c0 00000000`00000000 00007ff6`2a6ccce0 00000000`00000000 : ntdll!ZwWaitForSingleObject+0xa
00000000`2bd4ed40 00007fff`59a52803 : 00000000`082a1690 00000000`5ef13fa0 00007ff6`2a6cccb8 00000000`5ef13fa0 : ntdll!RtlpWaitOnCriticalSection+0xe1
00000000`2bd4ee10 00007ff6`2a3ca431 : 00000000`5ef13fa0 00000000`079c0880 00000000`0000007f 00000000`082a1690 : ntdll!RtlpEnterCriticalSectionContended+0x93
00000000`2bd4ee50 00007ff6`2a3cac43 : 00000000`5ee4f1b0 00000000`00000000 00000000`00000000 00000000`07cc0fc0 : XXX!n+0x801 [d:\a\b\c\d\e\g\j.cpp @ 144]
00000000`2bd4f770 00007ff6`29e54b36 : 00000000`00000001 00000000`631fcce9 01d538c5`ffff8f80 00000000`2bd4f860 : XXX!o+0x13 [d:\a\b\c\d\e\g\j.cpp @ 181]
00000000`2bd4f7a0 00007ff6`2a2ec6fb : 0000016b`e50d722e 00000000`00000000 00007ff6`2a4e1cb0 00000000`00004e6c : XXX!p::run+0x36 [d:\a\b\c\d\e\h\k.cpp @ 654]
00000000`2bd4f8a0 00007ff6`2a42a825 : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : XXX!q::run+0xab [d:\a\b\c\d\e\i\l.h @ 97]
00000000`2bd4f8e0 00007fff`59201611 : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : XXX!r+0x35 [d:\a\b\c\d\f\m.cpp @ 178]
00000000`2bd4f910 00007fff`59a964ad : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : kernel32!BaseThreadInitThunk+0xd
00000000`2bd4f940 00000000`00000000 : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : ntdll!RtlUserThreadStart+0x1d
```


## CPU高
1. 输入!runaway
2. 发现n号线程cpu最高，切到n号线程，并输出堆栈

## 内存泄漏
1. 设置gflags.exe,这工具和windbg在同一目录   
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxNTU1NDQ4P3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxNjA0ODQzP3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
2. windbg附加到进程,输入!heap –s  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxNjI3MzgzP3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)
3. 程序运行一段时间之后，再次输入!heap–s  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxNjM2NjUxP3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)
发现00970000这个堆有增加，其他无变化
4. 输入!heap -stat -h00970000，查看这个堆状态  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxNjQ2NTUzP3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
发现这个堆的内存主要是被大小为0x224的块占用
5. 输入!heap -flt s 224, 查看224这些块被谁在使用  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxNzA3NzUzP3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)  
6. 输入!heap -p -a 00971d20，查看堆栈  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxNzE0OTcyP3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)
7. 已经得到堆栈，本地有源代码，还可以查看代码，输入lsa memoryleak!ThreadProc1+0x00000048  
输入lsa memoryleak!ThreadProc1+0x00000048  
![](https://github.com/ashenone0917/image/blob/main/L3Byb3h5L2h0dHBzL2ltZy1ibG9nLmNzZG4ubmV0LzIwMTgwMTI1MTUxNzQyNDE3P3dhdGVybWFyay8yL3RleHQvYUhSMGNEb3ZMMkpzYjJjdVkzTmtiaTV1WlhRdmVuRjNYelF4T0RFPS9mb250LzVhNkw1TDJUL2ZvbnRzaXplLzQwMC9maWxsL0kwSkJRa0ZDTUE9PS9kaXNzb2x2ZS83MC9ncmF2aX.png)
