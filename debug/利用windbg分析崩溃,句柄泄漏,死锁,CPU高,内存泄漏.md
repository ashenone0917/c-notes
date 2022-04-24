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
