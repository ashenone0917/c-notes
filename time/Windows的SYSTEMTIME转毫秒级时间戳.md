```cpp
#include <atltime.h>
long long int SYSTEMTIME2UnixMs(SYSTEMTIME & st) {

		//SYSTEMTIME转为毫秒级时间戳
		CTime t(st);
		time_t tt = t.GetTime();
		return tt * 1000 + st.wMilliseconds;
 }
 ```
