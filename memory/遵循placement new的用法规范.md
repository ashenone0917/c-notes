# 遵循placement new的用法规范
标准的开始<new>由一个特别的操作符new来定义，这个就是我们熟知的placement new。和其他普通的new不同的是，它在括号里多了另外一个参数。比如：
```cpp
Widget * p = new Widget; //ordinary new  //普通的new pi = new (ptr) int;
pi = new (ptr) int; //placement new
```
括号里的参数是一个指针，它指向一个内存缓冲器，placement new将在这个缓冲器上分配一个对象。Placement new的返回值是这个被构造对象的地址(比如扣号中的传递参数)。placement new主要适用于：在对时间要求非常高的应用程序中，因为这些程序分配的时间是确定的；长时间运行而不被打断的程序；以及执行一个垃圾收集器(garbage collector)。
## 使用方法
在很多情况下，placement new的使用方法和其他普通的new有所不同。这里提供了它的使用步骤。
### 第一步  缓存提前分配
为了保证通过placement new使用的缓存区的memory alignmen(内存队列)正确准备，使用普通的new来分配它：
```cpp
class Task {/*..*/};
char * buff = new [sizeof(Task)]; //分配内存
```
(请注意auto或者static内存并非都正确地为每一个对象类型排列，所以，你将不能以placement new使用它们。)\
### 第二步：对象的分配
在刚才已分配的缓存区调用placement new来构造一个对象。
```cpp
Task *ptask = new(buff) Task
```
### 第三步：使用
按照普通方式使用分配的对象：
```cpp
ptask->suspend();
ptask->resume();
//...
```
### 第四步：对象的销毁
一旦你使用完这个对象，你必须调用它的析构函数来毁灭它。按照下面的方式调用析构函数：
```cpp
ptask->~Task(); //调用外在的析构函数
```
### 第五步：释放
你可以反复利用缓存并给它分配一个新的对象（重复步骤2，3，4）如果你不打算再次使用这个缓存，你可以象这样释放它：
```cpp
delete [] buff;
```
跳过任何步骤就可能导致运行时间的崩溃，内存泄露，以及其它的意想不到的情况。如果你确实需要使用placement new，请认真遵循以上的步骤。
  
-------
本文作者:Danny Kalev是具有14年经验的系统分析员和软件工程师，他主要专长于C++和面象对象程序设计。
