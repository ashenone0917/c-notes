```cpp
#include <iostream>
#include <Windows.h>
#include <set>
#define SUB_STRING_LEN 3
int main() {
	static auto compare = [=](const wchar_t* lsh, const wchar_t* rsh) {
		return  wcsncmp(lsh, rsh, SUB_STRING_LEN) < 0;
	};
    //std::wstring wt = L"adfdaklrhkeqwjh421k4mfdm,3kl213mqnfdmasnfasdfsadkfjwqeklrjkqwl";
    std::wstring wt = L"abcdefghijklmnopqrstuvwxyz";
    std::set<const wchar_t*,decltype(compare)> test(compare);
	auto str = wt.c_str();
	auto tempsize = wt.length();
	for (auto i = 0; i < wt.length() - 2; ++i) {
		test.insert(str);
		++str;
	}
	int m = 10;
}
```
