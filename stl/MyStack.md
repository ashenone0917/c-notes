```cpp
template <typename T>
class MyStack {
public:
	using Pointer = T* ;
	MyStack(size_t capacity) : capacity_(capacity) {
		stack_ = Pointer(::malloc(sizeof(T) * capacity));
	}

	void push(T&& value) {
		emplace(std::move(value));
	}

	void push(const T& value) {
		emplace(value);
	}

	template<class... _Valty>
	void emplace(_Valty&&... _Val) {
		if (current_ == capacity_) {
			capacity_ = capacity_ << 1;
			auto ptr = (Pointer)(::malloc(sizeof(T) * capacity_));
			memcpy(ptr, stack_, current_ * sizeof(T));
			::free(stack_);
			stack_ = ptr;
		}
		new (stack_ + current_) T(std::forward<_Valty>(_Val)...);
		++current_;
	}

	T pop() {
		if (current_ == 0) throw std::exception("stack is null");

		--current_;
		auto pointer = (Pointer)(stack_ + current_);
		T ret(*pointer);
		pointer->~T();
		return ret;
	}

	~MyStack() {
		for (int i = 0; i < current_; ++i) {
			auto pointer = (Pointer)(stack_ + i);
			pointer->~T();
		}
		::free(stack_);
	}

private:
	Pointer stack_ = nullptr;
	size_t capacity_ = 0;
	size_t current_ = 0;
};
```
