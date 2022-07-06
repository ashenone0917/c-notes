```cpp
void insertionSort(vector<int>& v) {
	auto len = v.size();
	if (len <= 1) return;

	for (int i = 1; i < len; ++i) {
		auto value = v[i];
		int j = i - 1;
		for (; j >= 0; --j) {
			if (v[j] > value) {
				v[j + 1] = v[j];
			}
			else break;
		}
		v[j + 1] = value;
	}
}

int main()
{
  vector<int> test = { 5,76,1,7,65,24,0,321,456,789,3213,0,10,432 };
  insertionSort(test);
  std::cout << "Hello World!\n";
}
```
