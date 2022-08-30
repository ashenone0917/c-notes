```cpp
#include <vector>
#include <string>
#include <iterator>
#include <regex>
std::vector<std::string> s_split(const std::string& in, const std::string& delim) {
    std::regex re{ delim };
    // 调用 std::vector::vector (InputIterator first, InputIterator last,const allocator_type& alloc = allocator_type())
    // 构造函数,完成字符串分割
    return std::vector<std::string> {
        std::sregex_token_iterator(in.begin(), in.end(), re, -1),
            std::sregex_token_iterator()
    };
}

template<typename TString>
inline std::vector<TString> SplitString(TString& tstring_, typename TString::const_pointer delim) {
    if (tstring_.empty()) return std::vector<TString>();

    typename TString::size_type start = 0;
    typename TString::size_type end = 0;
    typename TString::size_type size = tstring_.size();
    std::vector<TString> retVal{};
    do
    {
        end = tstring_.find(delim, start);
        if (end == TString::npos) end = size;
        if (start != end) retVal.emplace_back(tstring_.substr(start, end - start));
        start = end + 1;
    } while (start < size);

    return retVal;
}

int main()
{
    std::string ts1= "1,2,3";
    auto vts1 = SplitString(ts1,",");
}
