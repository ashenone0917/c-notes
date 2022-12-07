1. 以管理员身份运行gflags.exe工具
2. 使用gflags.exe监视指定程序的退出，如下操作：  
![13658142-2016d3d42434268e](https://user-images.githubusercontent.com/79822208/206176222-f51cff4f-3b15-4682-8d01-a8a24b10bbaa.png)
3. 在事件查看器中查看来源为"Process Exit Monitor"的事件  
在"运行"窗口中键入"eventvwr"，打开事件查看器，选择"Windows日志"---"应用程序"，如下：  
![13658142-56d6ec8c060c74ba](https://user-images.githubusercontent.com/79822208/206178328-f847a721-b3ec-444b-8be4-4dde369f38de.png)  

4. 测试-以notepad++.exe为例
- 正常退出(也有可能是自己崩溃退出)
![13658142-ebbc8e9f75636a8f](https://user-images.githubusercontent.com/79822208/206180779-f2d40d17-3341-49b8-b593-0c81e3d16b7d.png)
- 被任务管理器结束：
![13658142-6c763b0aaa7d3741](https://user-images.githubusercontent.com/79822208/206180619-9138ccef-f15a-4a95-8677-5f6efa2553c3.png)
- 调用TerminateProcess函数结束进程：
![13658142-b347c53beeb03f1a](https://user-images.githubusercontent.com/79822208/206180697-0507ed8e-b902-4885-b7b4-d9ad4d99f9de.png)

5. 抓取退出时的dump
![QQ截图20221207203733](https://user-images.githubusercontent.com/79822208/206181516-bc0c0063-7f09-4f4a-8c58-535dd68773f3.png)  
- Enable Dump collection打开dump收集功能
- Dump Folder Location 为生成dump路径
- Custom Dump Type: 1(minidump),2(fulldump)一般填而即可
- 生成的dump会在对应的目录下(xxxx.exe-(PID-7896)-39216888)
6. 如何取消应用程序监控
- 重新打开gflags.exe
- 选择"Silent Process Exit"选项卡
- 输入要取消监视的进程名
- 按"Tab"键
- 将"Enable Silent Process Exit Monitoring"去掉勾选。


>https://www.jianshu.com/p/92d6f663832e

