```cpp
#ifndef _SMART_HANDLE_H_
#define _SMART_HANDLE_H_
#include <Windows.h>
template<typename Closer, HANDLE invalid_handle>
class SmartHandle {
public:
	SmartHandle(const SmartHandle&) = delete;
	SmartHandle& operator=(const SmartHandle&) = delete;

	SmartHandle(HANDLE handle = invalid_handle) : handle_(handle) {
	}

	SmartHandle(SmartHandle&& other)
		: handle_(other.handle_) {
		other.handle_ = invalid_handle;
	}

	SmartHandle& operator=(SmartHandle&& other) noexcept {
		if (&other != this) {
			Close();
			handle_ = other.handle_;
			other.handle_ = invalid_handle;
		}
		return *this;
	}

	bool operator==(HANDLE handle) {
		return handle_ == handle;
	}

	~SmartHandle() {
		Close();
	}

	operator HANDLE() const {
		return handle_;
	}


	bool IsValid() {
		return handle_ != invalid_handle;
	}

private:
	VOID Close() {
		if (handle_ != invalid_handle) {
			Closer()(handle_);
			handle_ = invalid_handle;
		}
	}

private:
	HANDLE handle_ = invalid_handle;
};

struct SmartHandleCloseHandle {
	void operator()(HANDLE handle) {
		CloseHandle(handle);
	}
};
struct FindFileCloseHandle {
	void operator()(HANDLE handle) {
		FindClose(handle);
	}
};

typedef SmartHandle<SmartHandleCloseHandle, (HANDLE)NULL> EventHandle;
typedef SmartHandle<SmartHandleCloseHandle, INVALID_HANDLE_VALUE> NamedPipeHandle;
typedef SmartHandle<SmartHandleCloseHandle, INVALID_HANDLE_VALUE> FileHandle;
typedef SmartHandle<FindFileCloseHandle, INVALID_HANDLE_VALUE> FindFileHandle;

#endif // _SMART_HANDLE_H_

```
