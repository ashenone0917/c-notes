```cpp
// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

template<typename T,
    typename = std::enable_if_t<std::is_constructible_v<std::string,std::decay_t<T>>>>
void setName(T&& name) {
    std::string t(std::forward<T>(name));
    std::cout << t <<std::endl;
}

int main()
{
    setName("123");
    setName(std::string("456"));
    std::string s = "789";
    setName(s);
    
    std::cout << "Hello World!\n";
}
```
