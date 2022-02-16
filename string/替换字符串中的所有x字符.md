```cpp
template<class S>
inline void ReplaceAllInPlace(S* str,
	const S& from,
	const S& to) {
	size_t pos = 0;
	while ((pos = str->find(from.c_str(), pos, from.size())) != S::npos) {
		str->replace(pos, from.size(), to.c_str(), to.size());
		pos += to.size();
	}
}


int main() {
	std::string from = ":";
	std::string to = "";

	std::string str = "http://192.168.3:1.6:0";
  //将str中所有:替换为空
	ReplaceAllInPlace(&str, from, to);
}
```
