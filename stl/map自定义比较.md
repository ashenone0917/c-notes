```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <ppltasks.h>
#include <map>
#include <Windows.h>
using namespace std;
using namespace Concurrency;
struct test_info {
    BYTE name_length_;
    WCHAR name_[0];
};

int main() {

    static auto compare = [](test_info* lsh, test_info* rsh) {
        //内部不能用不等于，返回结果必须得有强弱顺序
        if (lsh->name_length_ != rsh->name_length_) return lsh->name_length_ < rsh->name_length_;
        return  wcsncmp(lsh->name_, rsh->name_, lsh->name_length_) < 0;
    };

    static std::map<test_info*, uint32_t, decltype(compare)> hashMapName(compare);
    test_info* test_info1 = (test_info*)::malloc(1 + 3);
    test_info* test_info2 = (test_info*)::malloc(1 + 4);
    test_info* test_info3 = (test_info*)::malloc(1 + 3);
    test_info* test_info4 = (test_info*)::malloc(1 + 3);
    test_info1->name_length_ = 3;
    test_info2->name_length_ = 4;
    test_info3->name_length_ = 3;
    test_info4->name_length_ = 3;
    wmemcpy(test_info1->name_,L"123",3);
    wmemcpy(test_info2->name_,L"1234",4);
    wmemcpy(test_info3->name_,L"123",3);
    wmemcpy(test_info4->name_,L"789",3);
    auto resulttt = hashMapName.try_emplace(test_info1, 1);
    if (!resulttt.second) resulttt.first->second++;
    auto resulttt2 = hashMapName.try_emplace(test_info2, 1);
    if (!resulttt2.second) resulttt.first->second++;
    auto resulttt3 = hashMapName.try_emplace(test_info3, 1);
    if (!resulttt3.second) resulttt.first->second++;
    auto resulttt4 = hashMapName.try_emplace(test_info4, 1);
    if (!resulttt4.second) resulttt.first->second++;
}
```
