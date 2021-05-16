```cpp
// virtual.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
using namespace std;
class Bird {
public:
    void eat() {
        cout << "base eat!!" << endl;
    }//无需重新实现，大家都一样

    virtual void sleep() {
        cout << "base sleep!!" << endl;
    }//可使用默认的，可重写
    virtual void fly() = 0;//必须实现
};

//纯虚函数也可以实现，让继承者调用，看上去像是默认的一样
void Bird::fly() {
    cout << "base fly" << endl;
}

class A : public Bird {
public:
    void fly() {
        Bird::fly();
    }
};

class B : public Bird {
public:
    void fly() {
        cout << "B fly" << endl;
    }
};

int main()
{
    A a;
    a.fly();
    a.eat();
    a.sleep();
    B b;
    b.fly();
    b.eat();
    b.sleep();
}
```
