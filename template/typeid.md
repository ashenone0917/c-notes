利用typeid做运行时类型识别

```cpp
template<typename T>
bool isWString(T& t) {
    if (typeid(t) == typeid(std::wstring)) {
        std::cout << "is wstring" << std::endl;
        return true;
    }
    else {
        std::cout << "not wstring is " << typeid(t).name() << std::endl;
        return false;
    }
}

 std::wstring wstemp;
 std::string stemp;

 isWString(wstemp);
 isWString(stemp);
```

print:
```cpp
is wstring
not wstring is class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >
```
