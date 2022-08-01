```cpp
// HeapSort.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
//最大堆
class Heap {
public:
    Heap() {
        heap_malloc();
    }

    Heap(int capacity) : capacity_ (capacity) {
        heap_malloc();
    }

    void insert(int value) {
        if (count_ >= capacity_) heap_realloc();

        ++count_;
        data_[count_] = value;
        int i = count_;
        while ((i >> 1) > 0 && data_[i >> 1] < data_[i]) {
            swap(data_[i], data_[i / 2]);
            i = i >> 1;
        }
    }

    void remote_top() {
        if (count_ == 0) return;
        data_[1] = data_[count_--];
        heapify();
    }

private:
    void heapify() {
        int i = 1;
        while (true) {
            int maxPos = i;
            if (i * 2 <= count_ && data_[i] < data_[i * 2]) maxPos = i * 2;
            if (i * 2 + 1 <= count_ && data_[maxPos] < data_[i * 2 + 1]) maxPos = i * 2 + 1;

            if (maxPos == i) break;

            swap(data_[i], data_[maxPos]);
            i = maxPos;
        }
    }

    void heap_malloc() {
        data_ = new int[capacity_ + 1];
    }

    void heap_realloc() {
        capacity_ = ((capacity_ - 1) << 1) + 1;
        int* temp = new int[capacity_];
        memcpy_s(temp, capacity_, data_ + 1, count_);
        delete[] data_;
        data_ = temp;
    }

    void swap(int& a, int& b) {
        int tmp = a;
        a = b;
        b = tmp;
    }

    int count_ = 0;
    int capacity_ = 2;
    int* data_ = nullptr;
};

#define PARENT_NODE(n) ((n - 1) >> 1)
#define LEFT_CHILD(n) ((n >> 1) + 1)
#define RIGHT_CHILD(n) ((n >> 1) + 2)

void swap(int& a, int& b) {
    int tmp = a;
    a = b;
    b = tmp;
}

void heapify(std::vector<int>& v,int current,int count) {
    while (true) {
        int maxPos = current;
        if (LEFT_CHILD(current) <= count && v[current] < v[LEFT_CHILD(current)]) maxPos = LEFT_CHILD(current);
        if (RIGHT_CHILD(current) <= count && v[maxPos] < v[RIGHT_CHILD(current)]) maxPos = RIGHT_CHILD(current);

        if (maxPos == current) break;
        swap(v[current], v[maxPos]);
        current = maxPos;
    }
}
void heapSort(std::vector<int>& v) {
    auto count = v.size() - 1;
    for (int i = (count >> 1); i >= 0; --i) {
        heapify(v, i, count);
    }


}

int main()
{
    
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
```
