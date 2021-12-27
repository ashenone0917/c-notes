```cpp
#include <vector>
#include <string>

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

    return std::move(retVal);
}

int main()
{
    std::string ts1= "1,2,3";
    auto vts1 = SplitString(ts1,",");
}
