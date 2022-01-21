```cpp
template <typename T>
class removePointer {
public:
	using Type = T;
};

template <typename T>
class removePointer<T*> {
public:
	//用到了模板的递归
	using Type = typename removePointer<T>::Type;
};

template <typename T>
class removeConst {
public:
	using Type = T;
};

template <typename T>
class removeConst<const T> {
public:
	using Type = T;
};

int main() {
	
	int***** m = nullptr;
	removePointer<decltype(m)>::Type n;
	removeConst<const int>::Type k;
}
```
