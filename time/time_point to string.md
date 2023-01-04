
```cpp
  #include <chrono>
	std::string timePointToString(const std::chrono::system_clock::time_point& tp) {
		std::time_t t = std::chrono::system_clock::to_time_t(tp);
		char buffer[1024] = { 0 };
		ctime_s(buffer, 1024, &t);
		return buffer;
	}
```
