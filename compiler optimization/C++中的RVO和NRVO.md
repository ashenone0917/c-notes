# C++中的RVO和NRVO
RVO (return value optimization) 和NRVO (named return value optimization) 是C++在处理 “返回一个class object的函数” 时常用的优化技术，主要作用就是消除临时对象的构造和析构成本。目前我正在研究《深度探索C++对象模型》，对于这两种常见的编译器优化有了初步的了解。接下来以一个名叫Point3d的类和一个factory函数为例来总结一下。

Point3d类定义如下：

```cpp

class Point3d
{
public:
    Point3d(int x = 0, int y = 0, int z = 0): x(x), y(y), z(z)
    {
        cout << "constructor ";
        cout << this->x << " " << this->y << " " << this->z << endl;
    }
    Point3d(const Point3d &other): x(other.x), y(other.y), z(other.z)
    {
        cout << "copy constructor" << endl;
    }
    ~Point3d()
    {
        cout << "destructor" << endl;
    }
    Point3d& operator=(const Point3d &rhs)
    {
        if (this != &rhs)
        {
            this->x = rhs.x;
            this->y = rhs.y;
            this->z = rhs.z;
        }
        cout << "operator = " << endl;
        return *this;
    }
    int x;
    int y;
    int z;
};
```

我们定义了三个成员变量x, y, z，为了简单起见，我们将它们都定义为public访问属性。接下来，我们定义了该类的默认构造函数、拷贝构造函数、赋值运算符和析构函数，它们都会通过打印一些字符来追踪函数调用。为了简单起见，我们不定义移动构造函数和移动赋值运算符。

接下来我们定义一个函数factory，它产生并返回一个Point3d对象：
```cpp
Point3d factory()
{
    Point3d po(1, 2, 3);
    return po;
}
```
整体代码如下：
```cpp
#include <iostream>
using namespace std;
class Point3d
{
public:
    Point3d(int x = 0, int y = 0, int z = 0): x(x), y(y), z(z)
    {
        cout << "constructor ";
        cout << this->x << " " << this->y << " " << this->z << endl;
    }
    Point3d(const Point3d &other): x(other.x), y(other.y), z(other.z)
    {
        cout << "copy constructor" << endl;
    }
    ~Point3d()
    {
        cout << "destructor" << endl;
    }
    Point3d& operator=(const Point3d &rhs)
    {
        if (this != &rhs)
        {
            this->x = rhs.x;
            this->y = rhs.y;
            this->z = rhs.z;
        }
        cout << "operator = " << endl;
        return *this;
    }
    int x;
    int y;
    int z;
};
 
Point3d factory();
int main()
{
    Point3d p = factory();
    cout << p.x << " " << p.y << " " << p.z << endl;
    return 0;
}
 
Point3d factory()
{
    Point3d po(1, 2, 3);
    return po;
}
```
接下来我们讨论“不做任何返回值优化”、“只做RVO不做NRVO”和“不光做RVO也做NRVO”三种情况

## (0)不做任何返回值优化

gcc中有一个-fno-elide-constructors的命令，可以去掉任何返回值优化。我们在编译时加上这个命令，观察到程序的输出如下。我们加上注释来说明程序运行过程
```cpp
constructor 1 2 3     //构造出factory()中的局部对象po
copy constructor     //调用Point3d的拷贝构造函数，用po构造出一个临时对象，姑且称之为临时对象_temp
destructor                //析构factory()中的局部对象po
copy constructor     //调用Point3d的拷贝构造函数，用临时对象_temp构造出main()函数中的局部对象p
destructor                //析构临时对象_temp
1 2 3                         //输出x, y, z

destructor                //析构main()函数中的局部对象p
```
可以看到，不做任何返回值优化时我们会承担两次拷贝构造函数和两次析构函数调用的成本。

## (1)使用RVO而不用NRVO
VS在debug模式下使用RVO而不用NRVO。我们可以将原来的代码在VS中进行测试，观察到程序的输出如下：
```cpp
constructor 1 2 3     //构造出factory()中的局部对象po
copy constructor
destructor
1 2 3                         //输出x, y, z
destructor                //析构main()函数中的局部对象p
```
我们可以发现，使用了RVO之后我们减少了一次拷贝构造函数和一次析构函数的调用。

事实上，RVO的原理是，将“返回一个类对象的函数”的返回值当做该函数的参数处理。具体而言，在上面的例子中，factory()函数会被改写成如下的形式：
```cpp

//c++伪代码
void factory(Point3d &_result)
{
    Point3d po;       //不做初始化
    po.Point3d::Point3d(1, 2, 3);   //调用构造函数构造po
    _result.Point3d::Point3d(po);   //调用拷贝构造函数构造factory()函数参数
    po.Point3d::~Point3d();         //析构po
    return;               //没有返回值
}
```
而对应的函数调用则会被改写成如下的形式：
```cpp
Point3d p;       //不做初始化
factory(p);      //将p变为函数参数，即调用函数factory()来初始化p
```
这样我们就可以很清楚地发现，上文的输出结果里拷贝构造函数的调用是为了从factory()中的局部对象po构造出函数参数_result，而析构函数的调用则是为了析构po。换句话说，进行了RVO之后，我们的factory()函数只使用了一个叫做po的局部对象，接下来该函数利用这个po对象直接构造出了factory()函数外面，main()函数里面的对象p。
这样做当然比不使用任何返回值优化要好，因为它减少了一次拷贝构造函数的调用和一次析构函数的调用。然而我们伟大的工程师们依然不知足。能不能把这个局部对象po也给省略掉呢？换句话说，能不能让factory()函数直接构造出对象p呢？如果能的话，我们就可以再次减少一次拷贝构造函数(用来利用po构造出_result)和一次析构函数(用来析构po)的调用。
## (2)在只使用RVO不使用NRVO时再次优化拷贝构造函数和析构函数的调用

我们可以将factory()函数改写成如下的形式：
```cpp
Point3d factory()
{
	return Point3d(1, 2, 3);
}
```
运行程序，得到的输出如下：
```cpp
constructor 1 2 3
1 2 3
destructor
```
我们可以发现，和上面(1)中的输出相比，减少了一次拷贝构造函数和析构函数的调用。为什么呢？

实际上，在这时，factory()函数被改写成如下形式：
```cpp

//c++伪代码
void factory(Point3d &_result)
{
    _result.Point3d::Point3d(1, 2, 3);   //调用构造函数构造factory()函数参数
    return;               //没有返回值
}
```
main()中的factory()函数调用依然保持(1)中的形式不变
```cpp
Point3d p;       //不做初始化
factory(p);      //将p变为函数参数，即调用函数factory()来初始化p
```
这时，由于我们的factory函数返回的是一个匿名的Point3d对象，编译器就可以进行更进一步的优化，省略掉factory()中的局部对象的构造、析构和从这个局部对象到函数参数的拷贝，从而减少一次拷贝构造函数和一次析构函数的调用。这时，我们可以认为，factory()函数直接构造出了main()函数中的p对象。

## (3)既使用RVO又使用NRVO
(2)的不足之处在于程序员必须通过手动返回临时对象来优化代码。NRVO使得在程序员写出和(1)中相同代码的情况下也能起到(2)中的效果，即，让factory()函数直接构造出main()函数中的p对象。

这时，factory()函数的写法依然和(1)中而不是(2)中相同。
```cpp
Point3d factory()
{
    Point3d po(1, 2, 3);
    return po;
}
```
编译器会用_result直接替换po，也就是改写成如下代码：
```cpp
//c++伪代码
void factory(Point3d &_result)
{
    _result.Point3d::Point3d(1, 2, 3);   //调用构造函数构造factory()函数参数
    return;               //没有返回值
}
```
当然，如果程序员写出的是这样的代码：
```cpp

Point3d factory()
{
    Point3d po;
    po.x = 1;
    po.y = 2;
    po.z = 3;
    return po;
}
```
则会被改写为：
```cpp
//c++伪代码
void factory(Point3d &_result)
{
    _result.Point3d::Point3d();   //调用构造函数构造factory()函数参数
    _result.x = 1;
    _result.y = 2;
    _result.z = 3;
    return;               //没有返回值
}
```
(如果只使用RVO则会被改写成如下代码)
```cpp
//c++伪代码
void factory(Point3d &_result)
{
    Point3d po;
    po.Point3d::Point3d();   //调用构造函数构造factory()局部对象
    po.x = 1;
    po.y = 2;
    po.z = 3;
    _result.Point3d::Point3d(po); //调用拷贝构造函数构造factory()函数参数
    return;               //没有返回值
}
```
可以很明显地看出来：1、在RVO机制上加上NRVO机制的直接表现就是编译器直接用“用来替代函数返回值的参数”取代“该函数返回的那个局部对象”，在这里，表现为编译器直接使用_result取代了po对象。2、节约了一次拷贝构造函数和一次析构函数调用的成本。因为“该函数返回的那个局部对象”被“用来代替函数返回值的那个参数”所取代，所以我们无需构造出该局部对象，自然也无需析构它。

目前的常用c++编译器都支持NRVO，C++11也已经把“允许编译器进行NRVO”写入了标准。经过测试，gcc编译器在debug和release模式下均支持NRVO，VS在debug模式下不支持NRVO，仅支持RVO，而在release模式下也支持NRVO。
