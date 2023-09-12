大家都知道，C++空类的内存大小为1字节，为了保证其对象拥有彼此独立的内存地址。非空类的大小与类中非静态成员变量和虚函数表的多少有关。
而值得注意的是，类中非静态成员变量的大小与编译器内存对齐的设置有关。
成员变量在类中的内存存储并不一定是连续的。它是按照编译器的设置，按照内存块来存储的，这个内存块大小的取值，就是内存对齐。

## 一、引入问题。
```cpp
#include<iostream>
using namespace std;
class test {
private :
    
    char c='1';//1byte 
    int i;//4byte
    short s=2;//2byte
};

int main(){
    cout << sizeof(test) << endl;
    return 0;
}
```
输出：12
```cpp
class test2 {
private:
    int i;//4byte
    char c = '1';//1byte 
    short s = 2;//2byte
};

int main(){
    cout << sizeof(test2) << endl;
    return 0;
}
```
输出：8\
我们可以看到。类test和test2的成员变量完全一样，只是定义顺序不一样，却造成了2个类占用内存大小不一样。而这就是编译器内存对齐的缘故。

## 二、规则
1. 第一个数据成员放在offset为0的地方，以后每个数据成员的对齐按照#pragma pack指定的数值和这个数据成员自身长度中，比较小的那个进行。
2. 在数据成员完成各自对齐之后，类(结构或联合)本身也要进行对齐，对齐将按照#pragma pack指定的数值和结构(或联合)最大数据成员长度中，比较小的那个进行。

很明显#pragma pack(n)作为一个预编译指令用来设置多少个字节对齐的。值得注意的是，n的缺省数值是按照编译器自身设置，一般为8，合法的数值分别是1、2、4、8、16。
即编译器只会按照1、2、4、8、16的方式分割内存。若n为其他值，是无效的。

## 三、问题分析
### （1）对于类test的内存空间是这样的：
![test memory](https://github.com/ashenone0917/image/blob/main/271354292862668.png "test的内存空间")
#### 内存分配过程：
1. char和编译器默认的内存缺省分割大小比较，char比较小，分配一个字节给它。
2. int和编译器默认的内存缺省分割大小比较，int比较小，占4字节。只能空3个字节，重新分配4个字节。
3. short和编译器默认的内存缺省分割大小比较，short比较小，占2个字节，分配2个字节给它。
4. 对齐结束类本身也要对齐，所以最后空余的2个字节也被test占用。

### （2）对于类test2的内存空间是这样的：
![test memory](https://github.com/ashenone0917/image/blob/main/testA2.png "test的内存空间")
1. int和编译器默认的内存缺省分割大小比较，int比较小，占4字节。分配4个字节给int。
2. char和编译器默认的内存缺省分割大小比较，char比较小，分配一个字节给它。
3. short和编译器默认的内存缺省分割大小比较，short比较小，此时前面的char分配完毕还余下3个字节，足够short的2个字节存储，所以short紧挨着。分配2个字节给short。
4. 对齐结束类本身也要对齐，所以最后空余的1个字节也被test占用。

### （3）使用#pragma pack(n)
```cpp
#include<iostream>
using namespace std;
#pragma pack(1)//设定为 1 字节对齐
class test {
private :
    
    char c='1';//1byte 
    int i;//4byte
    short s=2;//2byte
};

class test2 {
private:
    int i;//4byte
    char c = '1';//1byte 
    short s = 2;//2byte
};
int main(){
    cout << sizeof(test) << endl;
    cout << sizeof(test2) << endl;
    return 0;
}
```
输出：
![result](https://github.com/ashenone0917/image/blob/main/271439339424823.png "result")\
可以看到，当我们把编译器的内存分割大小设置为1后，类中所有的成员变量都紧密的连续分布。\

## 三、内存对齐的作用(以Windows 64位系统举例)
在Windows 64位系统上，通常建议按照8字节（64位）对齐数据，因为大多数64位处理器的自然字大小为8字节。这种对齐方式有助于充分利用处理器的性能，减少缓存未命中，至于为什么能减少缓存未命中，理由如下：   

在64位的CPU上，通常情况下，申请的地址都是8的倍数，即内存地址是按照8字节对齐的,这是因为大多数64位处理器使用的是8字节（64位）的寻址。对于64位CPU，通常情况下，CPU访问内存的地址也是按照8字节对齐的。这是因为64位处理器的寻址空间非常广泛，可以处理非常大的内存范围，因此通常按照8字节对齐以确保最佳性能。  

当CPU执行内存访问时，如果访问的数据不是按照8字节对齐，可能会引发额外的处理器周期来处理这种不对齐的访问，这会导致性能下降。因此，编写代码时应该尽量保持对数据的访问是按照8字节对齐的，这有助于避免性能问题。  

将数据按照8字节对齐与缓存行（Cache Line）有关，因为缓存行通常的大小是64字节(64位cpu的data bus)。缓存行是高速缓存中的最小单位，意味着处理器在从主存加载数据时会加载整个缓存行。如果访问的数据跨越多个缓存行，就会导致缓存行跨越（Cache Line False Sharing）的问题，这可能会降低性能。因此通过将数据按照8字节对齐，可以确保每个数据项都位于不同的缓存行中，从而最大限度地减少了缓存行跨越的可能性。这有助于避免多个CPU核心在访问不同数据时不必要地争用相同的缓存行，提高了程序的并发性能。  

举个缓存行跨越的例子:  
|0|1|2|3|4|5|6|7|8|，字段a没有以8字节对齐，在此内存占用了3-8位置，由于访问内存也是按照8字节对齐，所以此时想要获取a，需要访问两次内存，0-7，8-15，截取其中的3-7和7-8，结果就是缓存行跨越导致访问了两次内存，但是如果a按8字节对齐，那么a如果a的长度小于8字节，只需要一次访问便能获取。

此外还有cache miss和cache ping pong的问题看以下链接：
https://plantegg.github.io/2021/05/16/CPU_Cache_Line%E5%92%8C%E6%80%A7%E8%83%BD/


至此，C++内存对齐总结已经差不多了。想要更多了解C++对象内存分配推荐陈浩的2篇文章：\
[http://blog.csdn.net/haoel/article/details/3081328](http://blog.csdn.net/haoel/article/details/3081328 "1")\
[http://blog.csdn.net/haoel/article/details/1948051](http://blog.csdn.net/haoel/article/details/1948051 "2")
