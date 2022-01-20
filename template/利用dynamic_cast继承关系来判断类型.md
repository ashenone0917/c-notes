```cpp
#include<iostream>
#include<typeinfo>

using namespace std;

class A {
   virtual void process() {}
   
};
class B :public A {
    virtual void size() {}
};

class C :public A {
    virtual void size() {}
};

int main() {
   A * a1 = new B;
   B * b1 = dynamic_cast<B*>(a1);
   if (b1 == nullptr) {
       cout << "b1 nullptr!!" << endl;
   }
    A * a2 = new B;
    C * b2 = dynamic_cast<C*>(a2);
    if (b2 == nullptr) {
       cout << "b2 nullptr!!" << endl;
    }
    B bb;
    A & a3 = bb;
    B & b3 = dynamic_cast<B&>(a3);
    try {
      A aa;
      A & a4 = aa;
      B & b4 = dynamic_cast<B&>(a4);    
    }
    catch (bad_cast& c) {
        cout << "bad cast!" << endl;
        
    }
  return 0;

}
```
