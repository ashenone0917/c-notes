```cpp
bool string_end_with(const std::string& src, const std::string& pattern) {
	auto pattern_len = pattern.length();
	if (src.length() < pattern_len) return false;

	const auto& begin = src.end() - pattern_len;
	return std::equal(pattern.begin(), pattern.end(), begin);
}
```
