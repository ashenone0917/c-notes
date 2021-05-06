```cpp
// BinarySearch.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
int my_binary_search(vector<string>& vec, string& val) {
    int mid,left = 0;
    int right = vec.size();
    while (left <= right) {
        mid = left + (right - left >> 1);
        if (vec[mid] < val) left = mid + 1;
        else if (val < vec[mid]) right = mid - 1;
        else {
            return mid;
        }
    }
    return -1;
}

struct Soft {
    string name;
    Soft(const char* name_) {
        name = name_;
    }
    int operator<(Soft& soft) {
        return name < soft.name;
    }
};
template<typename T>
int BinarySearch(std::vector<T>& vec, T& val) {
    int mid, left = 0;
    int right = vec.size();
    while (left <= right) {
        mid = left + (right - left >> 1);
        if (vec[mid] < val) left = mid + 1;
        else if (val < vec[mid]) right = mid - 1;
        else {
            return mid;
        }
    }
    return -1;
}


int main()
{
    vector<string> vecStr = { "123","456","abc","aaa","908","fsdf","z","BC","X"};
    std::sort(vecStr.begin(), vecStr.end(), 
        [](string& s1, string& s2) {
        return s1 < s2;
    });
    std::string val = "777";
    auto index = my_binary_search(vecStr, val);
    auto result = std::binary_search(vecStr.begin(), vecStr.end(), val);

    val = "aaa";
    index = my_binary_search(vecStr, val);
    result = std::binary_search(vecStr.begin(), vecStr.end(), val, [](const string& s1, const string& s2) {
        return s1 < s2;
    });
    
    vector<Soft> vecSoft = {
        "123","456","abc","aaa","908","fsdf","z","BC","X"
    };
    std::sort(vecSoft.begin(), vecSoft.end(),
        [](Soft& s1, Soft& s2) {
        return s1.name < s2.name;
    });
    Soft soft("908");
    index = BinarySearch<Soft>(vecSoft, soft);
    Soft soft2("aaa");
    index = BinarySearch<Soft>(vecSoft, soft2);
    Soft soft3("907");
    index = BinarySearch<Soft>(vecSoft, soft3);
}
```
