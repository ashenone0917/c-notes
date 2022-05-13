# windbg看函数参数(以kernelbase!CreateProcessW为例,cmd调用使用ipconfig命令)
## x64
### 定位到函数头的情况
在x64的情况下，前4个参数都是在rcx,rdx,r8,r9(参数在这四个寄存器中，不在内存中，但仍然会占着4个位置)，如下图：
![](https://github.com/ashenone0917/image/blob/main/Snipaste_2022-05-13_19-32-42.png)  
此时rsp还没有被移动到rpb，所以看所有参数使用rsp  
```
dq rsp
```
![](https://github.com/ashenone0917/image/blob/main/Snipaste_2022-05-13_19-46-10.png)  
从上面可以看出，前四个参数虽然没有在内存中，但仍然占位。参数的位置按照图片可以类推  

### 定位到非函数头的情况
前四个参数同上
但是此时应使用rbp(通常情况，但是在本函数使用r11)
```
dq rbp
```
使用r11的原因如下：  
![](https://github.com/ashenone0917/image/blob/main/Snipaste_2022-05-13_19-49-57.png)

## x86
要区分fastcall , thiscall其他没啥好说的
