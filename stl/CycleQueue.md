```cpp
#include <iostream>
#include <atomic>
using namespace std;

template <typename T>
class CycleQueue {
public:
	using Pointer = T* ;
	CycleQueue(int capacity) : capacity_(capacity + 1) {
		queue_ = Pointer(::malloc(sizeof(T) * (capacity_)));
	};

	template <class... _Valty>
	bool enqueue(_Valty&&... _Val) {
		if ((tail_ + 1) % capacity_ == head_) return false;

		new (queue_ + tail_) T(std::forward<_Valty>(_Val)...);
		tail_ = (tail_ + 1) % capacity_;
		return true;
	}

	T dequeue() {
		if(tail_ == head_) throw std::exception("queue is null");

		auto pointer = (Pointer)(queue_ + head_);
		T ret(*pointer);
		pointer->~T();
		head_ = (head_ + 1) % capacity_;
		return ret;
	}

	T front() {
		auto pointer = (Pointer)(queue_ + head_);
		T ret(*pointer);
		return ret;
	};

	~CycleQueue() {
		while (tail_ != head_) {
			auto pointer = (Pointer)(queue_ + head_);
			pointer->~T();
			head_ = (head_ + 1) % capacity_;
		}
		::free(queue_);
	}
private:
	int tail_ = 0;
	int head_ = 0;
	int capacity_;
	Pointer queue_;
};

std::atomic_int g_n = 0;
class TestC {
public:
	int n_;
	TestC() { n_ = g_n++; std::cout << "create TestC seq = " << n_<<std::endl; }
	~TestC() { std::cout << "destory TestC seq = " << n_ << std::endl; }
};
int main()
{
	{
		CycleQueue<TestC> cq(5);
		cq.enqueue(TestC());
		cq.enqueue(TestC());
		cq.enqueue(TestC());
		cq.enqueue(TestC());
		cq.enqueue(TestC());
		cq.enqueue(TestC());

		auto tc = cq.dequeue();
		tc = cq.dequeue();
		tc = cq.dequeue();
		tc = cq.dequeue();
		tc = cq.dequeue();
	}
	int m = 10;
	
    std::cout << "Hello World!\n";
}
```
