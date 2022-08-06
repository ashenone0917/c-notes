```cpp
// TopK.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>

#define PARENT_NODE(n) ((n - 1) / 2)
#define LEFT_NODE(n) ((n * 2) + 1)
#define RIGHT_NODE(n) ((n * 2) + 2)

void swap(std::vector<int>& v,int a, int b) {
    int temp = v[a];
    v[a] = v[b];
    v[b] = temp;
}

void insertMinHeap(std::vector<int>& heap, int count, int value) {
    heap[count] = value;
    while (PARENT_NODE(count) >= 0) {
        auto pv = heap[PARENT_NODE(count)];
        if (pv > value) {
            swap(heap, PARENT_NODE(count), count);
            count = PARENT_NODE(count);
        }
        else break;
    }
}

void removeTopInMinHead(std::vector<int>& heap,int count) {
    if (count == 0) return;

    heap[0] = heap[count--];
    int current = 0;
    while (true) {
        int minPos = current;
        if (LEFT_NODE(current) <= count && heap[LEFT_NODE(current)] < heap[current]) minPos = LEFT_NODE(current);
        if (RIGHT_NODE(current) <= count && heap[RIGHT_NODE(current)] < heap[minPos]) minPos = RIGHT_NODE(current);

        if (minPos == current) break;

        swap(heap, minPos, current);
        current = minPos;
    }
}

std::vector<int> topK(std::vector<int>& data, int k) {
    std::vector<int> tmp;
    if (k == 0) return tmp;
    tmp.resize(k);
    int i = 0;
    for (auto v : data) {
        if (i < k) {
            insertMinHeap(tmp, i, v);
        }
        else {
            if (v > tmp[0]) {
                removeTopInMinHead(tmp, k - 1);
                insertMinHeap(tmp, k - 1, v);
            }
        }
        ++i;
    }
    return tmp;
}
int main() {
    std::vector<int> v = {7,1,4,2,65,87,1,0,123,4,6423,12,10 };
    auto tk = topK(v, 3);
    std::cout << "Hello World!\n";
}
``
