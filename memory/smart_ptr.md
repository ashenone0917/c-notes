```cpp
 #include <memory>
#include <iostream>
auto unique_deleter = [](int* p) {
    std::cout << "unique_ptr delete:" << *p << "\n";
    delete p;
};
auto shared_deleter = [](int* p) {
    std::cout << "shared_ptr delete:" << *p << "\n";
    delete p;
};
auto shared_deleter_check_nullptr = [](int* p) {
    if (p != nullptr)
    {
        std::cout << "shared_ptr2 delete:" << *p << "\n";
        delete p;
    }
    else
    {
        std::cout << "shared_ptr2 is null\n";
    }
};
 
int main()
{
    {
        // unique_ptr == nullptr, 删除器没有被调用
        std::unique_ptr<int, decltype(unique_deleter)> pUniquePtr(nullptr, unique_deleter);
        // unique_ptr != nullptr, 删除器被调用
        std::unique_ptr<int, decltype(unique_deleter)> pUniquePtr2(new int(5), unique_deleter);
        // shared_ptr == nullptr, 删除器仍然被调用, 幸好对"nullptr"有检查
        std::shared_ptr<int> pSharedPtr(nullptr, shared_deleter_check_nullptr);
        // shared_ptr != nullptr, 删除器被调用
        std::shared_ptr<int> pSharedPtr2(new int(10), shared_deleter);
        // shared_ptr == nullptr, 删除器仍然被调用, 没有对"nullptr"的检查, 程序崩溃
        //std::shared_ptr<int> pSharedPtr(nullptr, shared_deleter);
    }
    getchar();
}
```
