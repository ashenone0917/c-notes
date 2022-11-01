```cpp
#include <Windows.h>
#include <iostream>
#define YEAR_START 1990
#define IS_ODD(number) (number & 0x1)
struct MINUTE {
	union {
		volatile unsigned int value_ = 0;
		struct {
			volatile unsigned int minute_ : 6;
			volatile unsigned int hour_ : 5;
			volatile unsigned int day_ : 3;
			volatile unsigned int month_ : 5;
			volatile unsigned int year_ : 8;
			volatile unsigned int reserve_ : 21;
		}v_;
	}ut_;

	SYSTEMTIME ToSystemtime() {
		SYSTEMTIME system_time;
		//system_time.wMinute = IS_ODD(ut_.v_.minute_) ? ut_.v_.minute_ << 1 : (ut_.v_.minute_ << 1) + 1; //为奇数则>>1前必是偶数，为偶数则>>1前必是奇数，需要更省内存可以这么做
		//上面那样还得判断0，感觉有点浪费时间
		system_time.wMinute = ut_.v_.minute_;
		system_time.wHour = ut_.v_.hour_;
		system_time.wDay = ut_.v_.day_;
		system_time.wMonth = ut_.v_.month_;
		system_time.wYear = ut_.v_.year_ + YEAR_START;
		return system_time;
	}
	
	MINUTE() = default;
	MINUTE(const MINUTE& rsh) {
		this->ut_.value_ = rsh.ut_.value_;
	}
};


MINUTE ConvertSystemTimeToMinute(SYSTEMTIME sysTime)
{
	MINUTE time32;
	if (sysTime.wYear < YEAR_START) return time32;
	//time32.ut_.v_.minute_ = sysTime.wMinute >> 1;配合上面注释的代码使用 
	time32.ut_.v_.minute_ = sysTime.wMinute;
	time32.ut_.v_.hour_ = sysTime.wHour;
	time32.ut_.v_.day_ = sysTime.wDay;
	time32.ut_.v_.month_ = sysTime.wMonth;
	time32.ut_.v_.year_ = sysTime.wYear - YEAR_START;
	return time32;
}

int main() {
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	auto m = ConvertSystemTimeToMinute(systime);
	auto s2 = m.ToSystemtime();
	
	std::cout << "hello world" << std::endl;
}
```
