```cpp
void shellSort(vector<int>& v) {
	auto len = v.size();
	if (len <= 1) return;
	int gap = len >> 1;
	for (; gap >= 1; gap = gap >> 1) {
		for (int i = gap; i < len ; ++i ) {
			auto value = v[i];
			int j = i - gap;
			for (; j >= 0; j -= gap) {
				if (v[j] > value) v[j + gap] = v[j];
				else break;
			}
			v[j + gap] = value;
		}
	}
}

int main()
{
	vector<int> test = { 5,76,1,7,65,24,0,321,456,789,3213,0,10,432 };
	shellSort(test);
    std::cout << "Hello World!\n";
}
```
