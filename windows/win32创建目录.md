```cpp
bool CreateDirectoryRecursive(const std::wstring& directory) {
    if (directory.empty())
        return false;

    size_t pos = 0;
    std::wstring current_dir;
    auto colon_pos = directory.find_first_of(L":\\", pos);
    if (std::wstring::npos != colon_pos) {
        pos = colon_pos + 2;
    }

    while ((pos = directory.find_first_of(L"\\", pos)) != std::wstring::npos) {
        current_dir = directory.substr(0, pos);
        if (!CreateDirectoryW(current_dir.c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError()) {
            return false;
        }
        ++pos;
    }
    if (!CreateDirectoryW(directory.c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError()) {
        return false;
    }

    return true;
}
```
