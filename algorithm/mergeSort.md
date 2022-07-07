```cpp
void mergeSort(vector<int>& v,int start,int end) {
	if (start >= end) return;

	auto mid = (start + end) >> 1;
	auto s = start;
	auto e = end;
	mergeSort(v, s, mid);
	mergeSort(v, mid + 1, e);
	
	auto p = mid + 1;
	std::vector<int> tmp;
	tmp.reserve(end - start + 1);
	for (; s <= mid && p<= e;) {
		if (v[s] > v[p]) tmp.push_back(v[p++]);
		else tmp.push_back(v[s++]);
	}

	if (s > mid) for (; p <= end; ++p) tmp.push_back(v[p]);
	else for (; s <= mid; ++s) tmp.push_back(v[s]);
	for (int i = 0; i < tmp.size(); ++i) {
		v[start + i] = tmp[i];
	}
}

void mergeSortDummy(vector<int>& v, int start, int end) {
	if (start >= end) return;

	auto mid = (start + end) >> 1;
	auto s = start;
	auto e = end;
	mergeSort(v, s, mid);
	mergeSort(v, mid + 1, e);

	auto p = mid + 1;
	std::vector<int> tmp;
	tmp.reserve(end - start + 1);
	int dummyLeft = 0;
	int dummyRight = 0;
	for (; s <= mid || p <= e;) {
		if (s > mid) dummyLeft = INT_MAX;
		else dummyLeft = v[s];

		if (p > e) dummyRight = INT_MAX;
		else dummyRight = v[p];

		if(dummyLeft > dummyRight) tmp.push_back(v[p++]);
		else tmp.push_back(v[s++]);
	}

	for (int i = 0; i < tmp.size(); ++i) {
		v[start + i] = tmp[i];
	}
}

int main()
{
	vector<int> test = { 5,76,1,7,65,24,0,321,456,789,3213,0,10,432,9};
	mergeSort(test,0,test.size() - 1);
    std::cout << "Hello World!\n";
}
```
