```cpp
#include <iostream>
#include <windows.h>
//把所有__stdcall去掉也能用
class Test {
public:
    virtual void __stdcall Func() = 0;
    virtual void __stdcall Func2() = 0;
};

class TestDerived : public Test {
public:
    virtual void __stdcall Func() override {
        std::cout << "TestDerived::Func() called" << std::endl;
    }

    virtual void __stdcall Func2() override {
        std::cout << "TestDerived::Func2() called" << std::endl;
    }
};

class Proxy {
public:
    void  Func() {
        std::cout << "Proxy::Func() called" << std::endl;
        auto this_ = (Test*)this;
        this_->Func2();
    }
};

template<typename FuncAddrType>
uintptr_t addressValue(FuncAddrType addr)
{
    return *reinterpret_cast<uintptr_t*>(&addr);
}

size_t WINAPI HookVtbl(void* pObject, size_t classIdx, size_t methodIdx, size_t newMethod)
{
    DWORD       oldProtect = 0;
    size_t** Vtable = NULL;
    size_t      oldMethod = NULL;

    Vtable = (size_t**)pObject;
    oldMethod = Vtable[classIdx][methodIdx];
    if (VirtualProtect(Vtable[classIdx] + methodIdx, sizeof(size_t*), PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        Vtable[classIdx][methodIdx] = newMethod;
        VirtualProtect(Vtable[classIdx] + methodIdx, sizeof(size_t*), oldProtect, &oldProtect);
    }
    return oldMethod;
}

int main() {
    Test* test = new TestDerived;
    uintptr_t* vtablePtr = *(uintptr_t**)&test;
    uintptr_t hookFuncAddr = addressValue(&Proxy::Func);
    HookVtbl(test, 0, 0, (size_t)hookFuncAddr);
    test->Func();
    return 0;
}
```
