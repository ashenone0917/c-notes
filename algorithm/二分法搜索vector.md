```cpp
// HuaweiTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <vector>
std::vector<int> arrayInt = {1,3,5,8,10,13,18,20 };
std::vector<int>::iterator find(int id) {
	int right = arrayInt.size() - 1;
	int mid = 0, left = 0;
	while (left <= right) {
		mid = left + ((right - left) >> 1);
		auto&& midId = arrayInt[mid];
		if (midId < id) left = mid + 1;
		else if (midId > id) right = mid - 1;
		else return arrayInt.begin() + mid;
	}
	return arrayInt.end();
}

int main()
{
	auto temp = find(20);
	if (temp != arrayInt.end()) {
		std::cout << *temp;
	}

	temp = find(10);
	if (temp != arrayInt.end()) {
		std::cout << *temp;
	}

	temp = find(4);
	if (temp != arrayInt.end()) {
		std::cout << *temp;
	}
}

```
