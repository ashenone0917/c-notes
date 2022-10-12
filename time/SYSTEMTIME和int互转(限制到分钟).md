```cpp
#define YEAR_START 1990
typedef unsigned int MINUTE; //以分钟表示的时间类型
MINUTE ConvertSystemTimeToMinute(SYSTEMTIME sysTime)
{
	MINUTE time32 = 0;
	if (sysTime.wYear < YEAR_START) return 0;
	time32 = sysTime.wMinute;
	time32 |= (sysTime.wHour << 5);
	time32 |= (sysTime.wDay << 10);
	time32 |= (((DWORD)sysTime.wMonth) << 15);
	time32 |= ((DWORD)(sysTime.wYear - YEAR_START) << 19);
	return time32;
}

void ConvertMinuteToSystemTime(SYSTEMTIME *sysTime, IN MINUTE time32)
{
	sysTime->wMinute = (time32 & 0x1f); time32 >>= 5;
	sysTime->wHour = time32 & 0x1f; time32 >>= 5;
	sysTime->wDay = time32 & 0x1f; time32 >>= 5;
	sysTime->wMonth = time32 & 0xf; time32 >>= 4;
	sysTime->wYear = time32 + YEAR_START;
}

int main() {
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	auto m = ConvertSystemTimeToMinute(systime);
	SYSTEMTIME systime2;
	ConvertMinuteToSystemTime(&systime2, m);
	auto size = sizeof(fileEntry);//24
    std::cout << "Hello World!\n";
}
```
