```
显示所有线程
~*
显示当前活动线程.
~.
显示引起异常的线程
~#
显示第二个线程
~2
选择第二个线程
~2 s
显示所有线程的栈：
~* kb
!runaway
.attach pid
.detach
bl/bc/bd/be(支持*,1-n格式）
bp/bu/ba/bm
bp address
bp model!func
bu model!func
bm model!fu*c
ba w4/r4/e4/i4 address//内存断点 I/O
bp /p eprocess address/func//进程断点
bp /t ethread address/func//线程断点
ba /p /t
dt命令
dt [nt!]_PEB [-r]
dt nt!_P*
dt [nt!]_PEB address （device,driverobject为例）
dt [nt!]_PEB Ldr 7ffdf000
dt [nt!]_EPROCESS
x nt!zw*显示匹配符号
db 以一个字节显示值和ASSCII字符
dw 显示2字节的值
dd 显示4字节的值
dq 显示8字节的值
dp 显示指针长度的值（32等同于dd,64等同于dq )
dD 显示double实数(8字节)的值
df 显示float实数(4字节)的值
字符：
da 显示asscii值
du 显示unicode值
ds 显示ANSI_STRING值
dS显示UNICODE_STRING的值
混合显示
dW 显示2字节和asscii的值
dc 显示4字节和asscii值

dpp/dpu/dpa/dps
x1表示要显示的指针长度
d:4字节；q:8字节;p：32位4字节，64位8字节
x2表示指针的值显示的形式：
p:DWORD或者QWORD
a:ASSCII
u:UNICODE
eb/ed/eD/ef/ep/eq/ew ea/eu/(非null结尾的字符)
eza/ezu(null结尾的字符)
eb address value ea address "string" e adress value
机器码：
ebshort jump e9 near jump ea far jump
je/jz74 jne/jnz75
nop90
Int 3->cc
Ret ->c3
```
