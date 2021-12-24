```cpp
1. mkdir .build //创建目录
2. cmake "cmakelist.txt目录"，例如 cmake ..(指定上层目录的cmakelist.txt)，但是这样会默认搭建第一个编译器选项
3. 正常使用是这样 "cmake "Visual Studio 15 2017  Win64" .." 创建VS2017工程且指定上层(../)的cmakelist.txt
4. 或者-G "cmake -G "Visual Studio 15 2017  Win64" .." 创建VS2017工程且指定上层(../)的cmakelist.txt

```


```cpp
\\编译win32工程
1.cmake -G 可查看编译平台
2.cmake -A WIN32 "Visual Studio 16 2019" .. 编译win32工程在vs2019上
```

[参考](https://www.cnblogs.com/pandamohist/p/13644953.html)

