```cpp
int partition(vector<int>& v, int left, int right) {
	auto pivot = v[right];
	while (left < right) {
		while (left < right && v[left] < pivot) ++left;
		if(left < right) v[right] = v[left];

		while (left < right && v[right] >= pivot) --right;
		if (left < right) v[left] = v[right];
	}

	v[left] = pivot;

	return left;
}

int partition2(vector<int>& v, int left, int right) {
	auto r = right;
	auto pivot = v[right];
	while (left < right) {
		while (left < right && v[left] <= pivot) ++left;
		while (left < right && v[right] >= pivot) --right;
		if (left < right) {
			auto temp = v[left];
			v[left] = v[right];
			v[right] = temp;
		}
	}
	
	auto temp = v[left];
	v[left] = v[r];
	v[r] = temp;

	return left;
}

void quickSort(vector<int>& v, int left, int right) {
	if (left >= right) return;

	auto q = partition2(v, left, right);
	quickSort(v, left, q - 1);
	quickSort(v, q + 1, right);
}

int main()
{
	vector<int> test = { 1,9,3,2,8,5};
	quickSort(test,0,test.size() - 1);
    std::cout << "Hello World!\n";
}
```
