创建一个窗口会用到如下函数：
- RegisterClassExW(注册窗口类，窗口过程)
- CreateWindowW(创建窗口)
- ShowWindow(显示窗口)
- UpdateWindow(发送一个WM_PAINT消息来更新窗口)
- 创建消息循环(GetMessage，TranslateAccelerator，TranslateMessage，DispatchMessage)

当一个线程使用某些GUI函数(比如CreateWindow)，就会在这个线程创建消息队列，一个线程只会有一个消息队列，比如在主线程创建窗口，就会在主线程创建消息队列。

而在主线程创建非模态子窗口同样会使用主线程创建窗口而产生的消息队列，并且使用主线程的消息循环，当点击子窗口的时候，系统会产生一个子窗口的点击消息(子窗口的hwnd)，
发送到主线程的消息队列中，在get获取到后，dispatchmessage会直接调用子窗口的消息过程(dispatchmessage参数包含hwnd)。

在主线程创建模态子窗口则创建模态子窗口的函数会创建一个新的消息循环，阻塞住主窗口消息循环并且使用主窗口的消息队列，这样当点击主窗口或者模态子窗口的时候，
系统将消息发送给主窗口的消息队列，然后被模态窗口的消息循环获取，如果是主窗口的消息，在dispatchmessage会直接丢弃

## 伪子窗口
如今大多数ui框架都是使用的伪子窗口，比如duilib，qt等

这些ui框架创建控件的时候并没有创建子窗口，只是绘制出一个图形，比如创建一个按钮。
当点击按钮的时候系统会发送一个点击消息到窗口的消息队列中，经由dispatchmessage进入窗口的消息过程，
在消息过程内会使用鼠标pt查找到对应的控件(遍历控件树查找)，如果查找到控件，就会调用对应控件点击函数

> https://segmentfault.com/a/1190000021584106
