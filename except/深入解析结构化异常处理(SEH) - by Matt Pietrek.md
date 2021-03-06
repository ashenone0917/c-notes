尽管以前写过一篇SEH相关的文章《 关于SEH的简单总结 》， 但那真的只是皮毛，一直对Windows异常处理的原理似懂非懂， 看了下面的文章 ，一切都豁然开朗.

1997年文章，Windows技术的根一直没变： http://www.microsoft.com/msj/0197/exception/exception.aspx

**Matt Pietrek 著**

**董岩 译**

在Win32操作系统提供的所有功能中，使用最广泛而又没有公开的恐怕要数结构化异常处理（ Structured Exception Handling ，SEH） 了。当你考虑Win32结构化异常处理时，也许会想到__try、__finally和__except等术语。可能你在任何一本讲解Win32的好书上都能找到关于SEH较为详细的描述，甚至Win32 SDK文档也对使用__try、__finally和__except进行结构化异常处理进行了相当完整的描述。

既然已经有了这些文档，那为什么我还说SEH并未公开呢？本质上来说，Win32结构化异常处理是操作系统提供的服务。你可能找到的所有关于SEH方面的文档都只是描述了某个特别的编译器的运行时库对操作系统实现的封装。关键字__try、__finally或者__except并没有什么神奇的。Microsoft的操作系统和编译器开发小组定义了这些关键字和它们的作用。其它C++编译器厂商完全按照它们的语义来就可以了。当编译器的SEH支持层把原始的操作系统SEH的复杂性封装起来的时候，它同时也把原始的操作系统SEH的细节隐藏了起来。

我曾经接到大量来自想自己实现编译器层面SEH的人发来的电子邮件，他们苦于找不到关于操作系统SEH实现方面的任何文档。按说，我应该能够告诉他们 Visual C++或Borland C++的运行时库源代码就是他们想要的。但是不知出于什么原因，编译器层面的SEH看起来好像是个大秘密。无论是Microsoft还是Borland都没有提供他们的SEH支持层最底层的源代码。（现在Microsoft仍然没有提供这些源代码，它提供的是编译过的目标文件，而Borland则提供了相应的源代码。）

 在本文中，我会剥掉结构化异常处理外面的包装直至其最基本的概念。在此过程中，我会把操作系统提供的支持与编译器通过代码生成和运行时库提供的支持分开来说。当我挖掘到关键的操作系统例程时，我使用的是运行于Intel处理器上的Windows NT 4.0。但是我这里讲的大部分内容同样也适用于其它处理器。我会避免涉及到真实的C++异常处理，它使用的是catch()而不是__except。从内部来讲，真实的C++异常处理的实现与我这里要讲的非常相似。但是真实的C++异常处理有一些其它的复杂问题，它会混淆我这里要讲的一些概念。
 
  在挖掘组成Win32 SEH的晦涩的.H和.INC文件的过程中，我发现最好的信息来源之一是IBM OS/2头文件（特别是BSEXCPT.H）。如果你涉足这方面已经有一段时间了，就不会感到太奇怪。这里描述的SEH机制是早在Microsoft还工作在OS/2上时就已经定义好的。由于这个原因，你会发现Win32下的SEH和OS/2下的SEH极其相似。（现在我们可能已经没有机会体验这一点 了，OS/2已经永远成为历史了。）
  
## 一、浅析SEH
如果我把SEH的所有细节一股脑儿全倒给你，你可能无法接受。因此我先从一小部分开始，然后层层深入。如果你以前从未接触过结构化异常处理，那正好，因为你头脑中没有一些自己设想的概念。如果你以前接触过SEH，最好把头脑中有关__try、GetExceptionCode和 EXCEPTION_EXECUTE_HANDLER之类的词统统忘掉，假设它对你来说是全新的。深呼吸。准备好了吗？让我们开始吧！

设想我告诉过你，当一个线程出现错误时，操作系统给你一个机会被告知这个错误。说得更明白一些就是，**当一个线程出现错误时，操作系统调用用户定义的一个回调函数**。这个回调函数可以做它想做的一切。例如它可以修复错误，或者它也可以播放一段音乐。无论回调函数做什么，它最后都要返回一个值来告诉系统下一步做什么。（这不是十分准确，但就此刻来说非常接近。）

当你的某一部分代码出错时，系统再回调你的其它代码，那么这个回调函数看起来是什么样子呢？换句话说，你想知道关于异常什么类型的信息呢？实际上这并不重要，因为Win32已经替你做了决定。 异常的回调函数的样子 如下：

```cpp
EXCEPTION_DISPOSITION
__cdecl _except_handler( struct _EXCEPTION_RECORD *ExceptionRecord,
                        void * EstablisherFrame,
                        struct _CONTEXT *ContextRecord,
                        void * DispatcherContext);
```                     

这个原型来自标准的Win32头文件EXCPT.H，乍看起来有些费解。但如果你仔细看，它并不是很难理解。首先，忽略掉返回值的类型（EXCEPTION_DISPOSITION）。你得到的基本信息就是它是一个叫作 _except_handler 并且带有四个参数的函数。

这个函数的第一个参数是一个指向 EXCEPTION_RECORD 结构的指针。这个结构在WINNT.H中定义，如下所示：

```cpp
typedef struct _EXCEPTION_RECORD {
   DWORD ExceptionCode;
   DWORD ExceptionFlags;
   struct _EXCEPTION_RECORD *ExceptionRecord;
   PVOID ExceptionAddress;
   DWORD NumberParameters;
   DWORD ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD;
```

这个结构中的 ExcepitonCode成员是赋予异常的代码。通过在WINNT.H中搜索以“STATUS_”开头的＃define定义，你可以得到一个异常代码列表。例如所有人都非常熟悉的STATUS_ACCESS_VIOLATION的代码是0xC0000005。一个更全面的异常代码列表可以在 Windows NT DDK的NTSTATUS.H中找到。此结构的第四个成员是异常发生的地址。其它成员暂时可以忽略。

_except_handler 函数的第二个参数是一个指向establisher帧结构的指针。它是SEH中一个至关重要的参数，但是现在你可以忽略它。

_except_handler 回调函数的第三个参数是一个指向 CONTEXT 结 构的指针。此结构在WINNT.H中定义，它代表某个特定线程的寄存器值。图1显示了CONTEXT结构的成员。当用于SEH时，CONTEXT结构表示异常发生时寄存器的值。顺便说一下，这个CONTEXT结构就是GetThreadContext和SetThreadContext这两个API中使用 的那个CONTEXT结构。

```cpp
typedef struct _CONTEXT
{
    DWORD ContextFlags;
    DWORD Dr0;
    DWORD Dr1;
    DWORD Dr2;
    DWORD Dr3;
    DWORD Dr6;
    DWORD Dr7;
    FLOATING_SAVE_AREA FloatSave;
    DWORD SegGs;
    DWORD SegFs;
    DWORD SegEs;
    DWORD SegDs;
    DWORD Edi;
    DWORD Esi;
    DWORD Ebx;
    DWORD Edx;
    DWORD Ecx;
    DWORD Eax;
    DWORD Ebp;
    DWORD Eip;
    DWORD SegCs;
    DWORD EFlags;
    DWORD Esp;
    DWORD SegSs;
} CONTEXT;
```

_except_handler 回调函数的第四个参数被称为DispatcherContext。它暂时也可以被忽略。

到现在为止，你头脑中已经有了一个当异常发生时会被操作系统调用的回调函数的模型了。这个回调函数带四个参数，其中三个指向其它结构。在这些结构中，一些域比较重要，其它的就不那么重要。这里的关键是_exept_handler回调函数接收到操作系统传递过来的许多有价值的信息，例如异常的类型和发生的地址。使用这些信息，异常回调函数就能决定下一步做什么。

对我来说，现在就写一个能够显示_except_handler作用的样例程序是再诱人不过的了。但是我们还缺少一些关键信息。特别是，当错误发生时操作系统是怎么知道到哪里去调用这个回调函数的呢？答案是还有一个称为EXCEPTION_REGISTRATION的结构。通篇你都会看到这个结构，所以不要跳过这一部分。我唯一能找到的EXCEPTION_REGISTRATION结构的正式定义是在Visual C++运行时库源代码中的EXSUP.INC文件中：

```cpp
<pre name="code" class="cpp">_EXCEPTION_REGISTRATION struc
    prev                dd       ?
    handler             dd       ?
_EXCEPTION_REGISTRATION ends
```

这个结构在WINNT.H的NT_TIB结构的定义中被称为_EXCEPITON_REGISTARTION_RECORD。唉，没有一个地方能够找到 _EXCEPTION_REGISTRATION_RECORD的定义，所以我不得不使用EXSUP.INC中这个汇编语言的结构定义。这是我前面所说 SEH未公开的一个证据。（读者可以使用内核调试器，如KD或SoftICE并加载调试符号来查看这个结构的定义。

下图是在KD中的结果：

![](https://github.com/ashenone0917/image/blob/main/o_st11.JPG)

下图是在SoftICE中的结果： 

![](https://github.com/ashenone0917/image/blob/main/o_st2.JPG)

译者注）

无论正在干什么，现在让我们回到手头的问题上来。当异常发生时，操作系统是如何知道到哪里去调用回调函数的呢？实际 上，EXCEPTION_REGISTARTION结构由两个域组成，第一个你现在可以忽略。第二个域handler，包含一个指向 _except_handler回调函数的指针。这让你离答案更近一点，但现在的问题是，操作系统到哪里去找 EXCEPTION_REGISTATRION结构呢？

要回答这个问题，记住**结构化异常处理是基于线程的**这一点是非常有用的。也就是说，每个线程有它自己的异常处理回调函数。在1996年五月的Under The Hood专栏中，我介绍了一个关键的Win32数据结构——**线程信息块（Thread Information/Environment Block，TIB或TEB）**。这个结构的某些域在Windows NT、Windows 95、Win32s和OS/2上是相同的。**TIB的第一个DWORD是一个指向线程的EXCEPTION_REGISTARTION结构的指针。在基于Intel处理器的Win32平台上，FS寄存器总是指向当前的TIB。因此在FS:[0]处你可以找到一个指向EXCEPTION_REGISTARTION结构的指针。**

 到现在为止，我们已经有了足够的认识。当异常发生时，系统查找出错线程的TIB，获取一个指向EXCEPTION_REGISTRATION结构的指针。在这个结构中有一个指向_except_handler回调函数的指针。现在操作系统已经知道了足够的信息去调用_except_handler函数，如图 2所示。
 
 ![1111](https://github.com/ashenone0917/image/blob/main/20161016165459218.png)
 
 **_except_handler函数**
 
  把这些小块知识拼凑起来，我写了一个小程序来演示上面这个对操作系统层面的结构化异常处理的简化描述，如图3的MYSEH.CPP所示。它只有两个函数。main函数使用了三个内联汇编块。第一个内联汇编块通过两个PUSH指令（“PUSH handler”和“PUSH FS:[0]”）在堆栈上创建了一个EXCEPTION_REGISTRATION结构。PUSH FS:[0]这条指令保存了先前的FS:[0]中的值作为这个结构的一部分，但这在此刻并不重要。重要的是现在堆栈上有一个8字节的 EXCEPTION_REGISTRATION结构。紧接着的下一条指令（MOV FS:[0],ESP）使线程信息块中的第一个DWORD指向了新的EXCEPTION_REGISTRATION结构。（注意堆栈操作）
  
  ```cpp
  //==================================================
// MYSEH - Matt Pietrek 1997
// Microsoft Systems Journal, January 1997
// FILE: MYSEH.CPP
// 用命令行CL MYSEH.CPP编译
//==================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
DWORD scratch;
EXCEPTION_DISPOSITION
__cdecl
_except_handler( struct _EXCEPTION_RECORD *ExceptionRecord,
                void * EstablisherFrame,
                struct _CONTEXT *ContextRecord,
                void * DispatcherContext )
{
    unsigned i;
    // 指明是我们让流程转到我们的异常处理程序的
    printf( "Hello from an exception handler\n" );
    // 改变CONTEXT结构中EAX的值，以便它指向可以成功进写操作的位置
    ContextRecord->Eax = (DWORD)&scratch;
    // 告诉操作系统重新执行出错的指令
return ExceptionContinueExecution;
}
int main()
{
    DWORD handler = (DWORD)_except_handler;
    __asm
    { 
        // 创建EXCEPTION_REGISTRATION结构：
        push handler // handler函数的地址
        push FS:[0] // 前一个handler函数的地址
        mov FS:[0],ESP // 安装新的EXECEPTION_REGISTRATION结构
    }
    __asm
    {
        mov eax,0     // 将EAX清零
        mov [eax], 1 // 写EAX指向的内存从而故意引发一个错误
    }
    printf( "After writing!\n" );
    __asm
    { 
        // 移去我们的EXECEPTION_REGISTRATION结构
        mov eax,[ESP]    // 获取前一个结构
        mov FS:[0], EAX // 安装前一个结构
        add esp, 8       // 将我们的EXECEPTION_REGISTRATION弹出堆栈
    }
    return 0;
}
```
如果你想知道我为什么把EXCEPTION_REGISTRATION结构创建在堆栈上而不是使用全局变量，我有一个很好的理由可以解释它。实际上，当你使用编译器的__try/__except语法结构时，编译器自己也把EXCEPTION_REGISTRATION结构创建在堆栈上。我只是简单地向你展示了如果使用__try/__except时编译器做法的简化版。

 回到main函数，第二个__asm块通过先把EAX寄存器清零（MOV EAX,0）然后把此寄存器的值作为内存地址让下一条指令（MOV [EAX],1）向此地址写入数据而故意引发一个错误。最后的__asm块移除这个简单的异常处理程序：它首先恢复了FS:[0]中先前的内容，然后把 EXCEPTION_REGISTRATION结构弹出堆栈（ADD ESP,8）。
 
 现在假若你运行MYSEH.EXE，就会看到整个过程。当MOV [EAX],1这条指令执行时，它引发一个访问违规。系统在FS:[0]处的TIB中查找，然后发现了一个指向 EXCEPTION_REGISTRATION结构的指针。在MYSEH.CPP中，在这个结构中有一个指向_except_handler函数的指针。 系统然后把所需的四个参数（我在前面已经说过）压入堆栈，接着调用_except_handler函数。
 
 一旦进入_except_handler，这段代码首先通过一个printf语句表明“哈!是我让它转到这里的！”。接着，_except_handler 修复了引发错误的问题——即EAX寄存器指向了一个不能写的内存地址（地址0）。修复方法就是改变CONTEXT结构中的EAX的值使它指向一个允许写的 位置。在这个简单的程序中，我专门为此设置了一个DWORD变量（scratch）。_except_handler函数最后的动作是返回 ExceptionContinueExecution这个值，它在EXCPT.H文件中定义。

当操作系统看到返回值为ExceptionContinueExecution时，它将其理解为你已经修复了问题，而引起错误的那条指令应该被重新执行。由于我的_except_handler函数已经让EAX寄存器指向一个合法的内存，MOV [EAX],1指令再次执行，这次main函数一切正常。看，这也并不复杂，不是吗？

## 二、移向更深处
有了这个最简单的情景之后，让我们回去填补那些空白。虽然这个异常回调机制很好，但它并不是一个完美的解决方案。对于稍微复杂一些的应用程序来说，仅用一个函数就能处理程序中任何地方都可能发生的异常是相当困难的。一个更实用的方案应该是有多个异常处理例程，每个例程针对程序中的一部分。实际上，操作系统提供的正是这个功能。

还记得系统用来查找异常回调函数的EXCEPTION_REGISTRATION结构吗？这个结构的第一个成员，称为prev，前面我们暂时把它忽略了。它实际上是一个指向另外一个EXCEPTION_REGISTRATION结构的指针。这第二个EXCEPTION_REGISTRATION结构可以有一个完全不同的处理函数。它的prev域可以指向第三个EXCEPTION_REGISTRATION结构，依次类推。 **简单地说，就是有一个EXCEPTION_REGISTRATION结构链表。线程信息块的第一个DWORD（在基于Intel CPU的机器上是FS:[0]）指向这个链表的头部。**

操作系统要这个 EXCEPTION_REGISTRATION 结构链表做什么呢？原来，当异常发生时，系统遍历这个链表以查找一个（其异常处理程序）同意处理这个异常的EXCEPTION_REGISTRATION结构。在 MYSEH.CPP中，异常处理程序通过返回ExceptionContinueExecution表示它同意处理这个异常。异常回调函数也可以拒绝处理 这个异常。在这种情况下，系统移向链表的下一个EXCEPTION_REGISTRATION结构并询问它的异常回调函数，看它是否同意处理这个异常。图 4显示了这个过程。一旦系统找到一个处理这个异常的回调函数，它就停止遍历链表。

![](https://github.com/ashenone0917/image/blob/main/o_pietrek4.jpg)

**图4　查找一个处理异常的EXCEPTION_REGISTRATION结构**

为了使代码尽量简单，我使用了编译器层面的异常处理。main函数只设置了一个 __try/__except块。在__try块内部调用了HomeGrownFrame函数。这个函数与前面的MYSEH程序非常相似。它也是在堆栈上创建一个EXCEPTION_REGISTRATION结构，并且让FS:[0]指向此结构。在建立了新的异常处理程序之后，这个函数通过向一个NULL 指针所指向的内存处写入数据而故意引发一个错误：

```cpp
*(PDWORD)0 = 0;
```
这个异常处理回调函数，同样被称为_except_handler，却与前面的那个截然不同。它首先打印出ExceptionRecord结构中的异常代码和标志，这个结构的地址是作为一个指针参数被这个函数接收的。打印出异常标志的原因一会儿就清楚了。因为_except_handler函数并没有打算修复出错的代码，因此它返回ExceptionContinueSearch。这导致操作系统继续在EXCEPTION_REGISTRATION结构链表中搜索下一个 EXCEPTION_REGISTRATION结构。接下来安装的异常回调函数是针对main函数中的__try/__except块的。__except块简单地打印出“Caught the exception in main()”。此时我们只是简单地忽略这个异常来表明我们已经处理了它。

```cpp
//=================================================
// MYSEH2 - Matt Pietrek 1997
// Microsoft Systems Journal, January 1997
// FILE: MYSEH2.CPP
// 使用命令行CL MYSEH2.CPP编译
//=================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
EXCEPTION_DISPOSITION
__cdecl _except_handler(
              struct _EXCEPTION_RECORD *ExceptionRecord,
              void * EstablisherFrame,
              struct _CONTEXT *ContextRecord,
               void * DispatcherContext )
{
printf( "Home Grown handler: Exception Code: %08X Exception Flags %X",
ExceptionRecord->ExceptionCode, ExceptionRecord->ExceptionFlags );
if ( ExceptionRecord->ExceptionFlags & 1 )
printf( " EH_NONCONTINUABLE" );
if ( ExceptionRecord->ExceptionFlags & 2 )
printf( " EH_UNWINDING" );
if ( ExceptionRecord->ExceptionFlags & 4 )
printf( " EH_EXIT_UNWIND" );
if ( ExceptionRecord->ExceptionFlags & 8 ) // 注意这个标志
printf( " EH_STACK_INVALID" );
if ( ExceptionRecord->ExceptionFlags & 0x10 )   // 注意这个标志
printf( " EH_NESTED_CALL" );
printf( "\n" );
// 我们不想处理这个异常，让其它函数处理吧
return ExceptionContinueSearch;
}
void HomeGrownFrame( void )
{
DWORD handler = (DWORD)_except_handler;
__asm
{
   // 创建EXCEPTION_REGISTRATION结构：
   push handler       // handler函数的地址
   push FS:[0]        // 前一个handler函数的地址
   mov FS:[0],ESP     // 安装新的EXECEPTION_REGISTRATION结构
}
*(PDWORD)0 = 0; // 写入地址0，从而引发一个错误
printf( "I should never get here!\n" );
__asm
{
   // 移去我们的EXECEPTION_REGISTRATION结构
   mov eax,[ESP]     // 获取前一个结构
   mov FS:[0], EAX // 安装前一个结构
   add esp, 8        // 把我们EXECEPTION_REGISTRATION结构弹出堆栈
}
}
int main()
{
__try
{
    HomeGrownFrame();
}
__except( EXCEPTION_EXECUTE_HANDLER )
{
    printf( "Caught the exception in main()\n" );
}
return 0;
}
```

这里的关键是执行流程。 **当一个异常处理程序拒绝处理某个异常时，它实际上也就拒绝决定流程最终将从何处恢复。只有处理某个异常的异常处理程序才能决定待所有异常处理代码执行完毕之后流程将从何处恢复。** 这个规则的意义非常重大，虽然现在还不明显。

 当使用结构化异常处理时，如果一个函数有一个异常处理程序但它却不处理某个异常，这个函数就有可能非正常退出。例如在MYSEH2中 HomeGrownFrame函数就不处理异常。由于在链表中后面的某个异常处理程序（这里是main函数中的）处理了这个异常，因此出错指令后面的 printf就永远不会执行。从某种程度上说，使用结构化异常处理与使用setjmp和longjmp运行时库函数有些类似。
 
 如果你运行MYSEH2，会发现其输出有些奇怪。看起来好像调用了两次_except_handler函数。根据你现有的知识，第一次调用当然可以完全理解。但是为什么会有第二次呢？
 
 ```cpp
 Home Grown handler: Exception Code: C0000005 Exception Flags 0
Home Grown handler: Exception Code: C0000027 Exception Flags 2 EH_UNWINDING
Caught the Exception in main()
```

比较一下以“Home Grown Handler”开头的两行，就会看出它们之间有明显的区别。第一次异常标志是0，而第二次是2。这把我们带入到了展开（Unwinding）的世界中。实际上，当一个异常处理回调函数拒绝处理某个异常时，它会被再一次调用。但是这次回调并不是立即发生的。这有点复杂。我需要把异常发生时的情形好好梳理一下。

**当异常发生时，系统遍历EXCEPTION_REGISTRATION结构链表，直到它找到一个处理这个异常的处理程序。一旦找到，系统就再次遍历这个链表，直到处理这个异常的结点为止。在这第二次遍历中，系统将再次调用每个异常处理函数。关键的区别是，在第二次调用中，异常标志被设置为2。这个值被定义为EH_UNWINDING。**（EH_UNWINDING的定义在Visual C++ 运行时库源代码文件EXCEPT.INC中，但Win32 SDK中并没有与之等价的定义。）
EH_UNWINDING 表示什么意思呢？原来，当一个异常处理回调函数被第二次调用时（带EH_UNWINDING标志），操作系统给这个函数一个最后清理的机会。什么样的清理呢？一个绝好的例子是C++类的析构函数。当一个函数的异常处理程序拒绝处理某个异常时，通常执行流程并不会正常地从那个函数退出。现在，想像一个定义了 一个C++类的实例作为局部变量的函数。C++规范规定析构函数必须被调用。这带EH_UNWINDING标志的第二次回调就给这个函数一个机会去做一些类似于调用析构函数和__finally块之类的清理工作。

在异常已经被处理完毕，并且所有前面的异常帧都已经被展开之后，流程从处理异常的那个回调函数决定的地方开始继续执行。一定要记住，仅仅把指令指针设置到所需的代码处就开始执行是不行的。流程恢复执行处的代码的堆栈指针和栈帧指针（在Intel CPU上是ESP和EBP）也必须被恢复成它们在处理这个异常的函数的栈帧上的值。因此，这个处理异常的回调函数必须负责把堆栈指针和栈帧指针恢复成它们在包含处理这个异常的SEH代码的函数的堆栈上的值。

通常，展开操作导致堆栈上处理异常的帧以下的堆栈区域上的所有内容都被移除了，就好像我们从来没有调用过这些函数一样。展开的另外一个效果就是 EXCEPTION_REGISTRATION结构链表上处理异常的那个结构之前的所有EXCEPTION_REGISTRATION结构都被移除了。这很好理解，因为这些EXCEPTION_REGISTRATION结构通常都被创建在堆栈上。在异常被处理后，堆栈指针和栈帧指针在内存中比那些从 EXCEPTION_REGISTRATION结构链表上移除的EXCEPTION_REGISTRATION结构高。图6显示了我说的情况。

![](https://github.com/ashenone0917/image/blob/main/o_pietrek6.jpg)

**图6 从异常展开**

迄今为止，我实际上一直在假设操作系统总是能在EXCEPTION_REGISTRATION结构链表中找到一个异常处理程序。如果找不到怎么办呢？实际上，这几乎不可能发生。因为操作系统暗中已经为每个线程都提供了一个默认的异常处理程序。这个默认的异常处理程序总是链表的最后一个结点，并且它总是选择处理异常。它进行的操作与其它正常的异常处理回调函数有些不同，下面我会说明。

让我们来看一下系统是在什么时候插入了这个默认的、最后一个异常处理程序。很明显它需要在线程执行的早期，在任何用户代码开始执行之前。图7是我为 BaseProcessStart 函数写的伪代码，它是Windows NT KERNEL32.DLL的一个内部例程。这个函数带一个参数——线程入口点函数的地址。**BaseProcessStart**运行在新进程的环境中，并且它调用这个进程的第一个线程的入口点函数。

**BaseProcessStart伪代码**
```cpp
BaseProcessStart( PVOID lpfnEntryPoint )
{
DWORD retValue;
DWORD currentESP;
DWORD exceptionCode;
currentESP = ESP;
__try
{
NtSetInformationThread( GetCurrentThread(),
ThreadQuerySetWin32StartAddress,
&lpfnEntryPoint,
sizeof(lpfnEntryPoint) );
retValue = lpfnEntryPoint();
ExitThread( retValue );
}
__except(   //过滤器表达式代码
        exceptionCode = GetExceptionInformation(),
        UnhandledExceptionFilter( GetExceptionInformation() ) )
{
ESP = currentESP;
if ( !_BaseRunningInServerProcess ) // 普通进程
    ExitProcess( exceptionCode );
else // 服务
    ExitThread( exceptionCode );
}
}
```

 在上面的伪代码中，注意对lpfnEntryPoint的调用被一个__try和__except块封装着。就是这个__try块安装了默认的、异常处理程 序链表上的最后一个异常处理程序。所有后来注册的异常处理程序都被安装在链表中这个结点的前面。如果lpfnEntryPoint函数返回，那么表明线程 一直运行到完成并且没有引发异常。这时BaseProcessStart调用ExitThread使线程退出。
 
 如果线程引发了一个异常但是没有异常处理程序来处理它时怎么办呢？这时，执行流程转到__except关键字后面的括号中。在BaseProcessStart中，这段代码调用 UnhandledExceptionFilter 这个API，后面我会讲到它。现在对于我们来说，重要的是**UnhandledExceptionFilter**这个API包含了默认的异常处理程序。'
 
 如果UnhandledExceptionFilter返回EXCEPTION_EXECUTE_HANDLER，这时BaseProcessStart中的__except块开始执行。而__except块所做的只是调用ExitProcess函数去终止当前进程。稍微想一下你就会理解了。常识告诉我们， 如果一个进程引发了一个错误而没有异常处理程序去处理它，这个进程就会被系统终止。你在伪代码中看到的正是这些。
 
 对于上面所说的我还有一点要补充。如果引发错误的线程是作为服务来运行的，并且是基于线程的服务，那么__except块并不调用ExitProcess，相反，它调用ExitThread。不能仅仅因为一个服务出错就终止整个服务进程。
 
  UnhandledExceptionFilter 中的默认异常处理程序都做了什么呢？当我在一个技术讲座上问起这个问题时，响应者寥寥无几。几乎没有人知道当未处理异常发生时，到底操作系统的默认行为是什么。简单地演示一下这个默认的行为也许会让很多人豁然开朗。我运行一个故意引发错误的程序，其结果如下（见图8）。
  
  ![](https://github.com/ashenone0917/image/blob/main/o_pietrek8.jpg)
  
  **图8 未处理异常对话框**
  
  表面上看，UnhandledExceptionFilter显示了一个对话框告诉你发生了一个错误。这时，你被给予了一个机会或者终止出错进程，或者调试它。但是幕后发生了许多事情，我会在文章最后详细讲述它。
  
  正如我让你看到的那样，当异常发生时，用户写的代码可以（并且通常是这样）获得机会执行。同样，在展开操作期间，用户写的代码也可以执行。这个用户写的代码可能也有错误，并且可能引发另一个异常。由于这个原因，异常处理回调函数也可以返回另外两个值： **ExceptionNestedException** 和 **ExceptionCollidedUnwind** 。很明显，它们很重要。但这是非常复杂的问题，我并不打算在这里涉及它们。要想理解其中的一些基本问题太困难了。
  
  ## 三、编译器层面的SEH
  虽然我在前面偶尔也使用了__try和__except，但迄今为止几乎我写的所有内容都是关于操作系统方面对SEH的实现。然而看一下我那两个使用操作系统的原始SEH的小程序别扭的样子，编译器对这个功能进行封装实在是非常有必要的。现在让我们来看一下Visual C++是如何在操作系统对SEH功能实现的基础上来创建它自己的结构化异常处理支持的。
  
  在我们继续下去之前，记住其它编译器可以使用原始的系统SEH来做一些完全不同的事情这一点是非常重要的。并没有什么规定编译器必须实现Win32 SDK文档中描述的__try/__except模型。例如Visual Basic 5.0在它的运行时代码中使用了结构化异常处理，但是那里的数据结构和算法与我这里要讲的完全不同。
  
  如果你把Win32 SDK文档中关于结构化异常处理方面的内容从头到尾读一遍，一定会遇到下面所谓的 **“基于帧”的异常处理程序模型** ：
  
  ```cpp
  __try {
    // 这里是被保护的代码
}
__except (过滤器表达式) { 
   // 这里是异常处理程序代码
}
```

 简单地说，在一个函数中，一个__try块中的所有代码就通过创建在这个函数的堆栈帧上的一个EXCEPTION_REGISTRATION结构来保护。在函数的入口处，这个新的EXCEPTION_REGISTRATION结构被放在异常处理程序链表的头部。在__try块结束后，相应的 EXCEPTION_REGISTRATION结构从这个链表的头部被移除。正如我前面所说，异常处理程序链表的头部被保存在FS:[0]处。因此，如果你在调试器中单步跟踪时看到类似下面的指令时
 
 ```cpp
 MOV DWORD PTR FS:[00000000],ESP
 ```
 
 或者
 
 ```cpp
 MOV DWORD PTR FS:[00000000],ECX
 ```
 
 就能非常确定这段代码正在进入或退出一个__try/__except块。
 
 既然一个__try块相当于堆栈上的一个EXCEPTION_REGISTRATION结构，那么EXCEPTION_REGISTRATION结构中的回调函数相当于什么呢？使用Win32的术语来说，异常处理回调函数相当于 **过滤器表达式（filter-expression）** 代码。实际上，过滤器表达式就是__except关键字后面的小括号中的代码。就是这个过滤器表达式代码决定了后面的大括号中的代码是否执行。
 
 由于过滤器表达式代码是你自己写的，你当然可以决定在你的代码中的某个地方是否处理某个特定的异常。它可以简单的只是一句“EXCEPTION_EXECUTE_HANDLER”，也可以先调用一个把 p 计算到 20,000,000 位 的函数，然后再返回一个值来告诉操作系统下一步做什么。随你的便。关键是你的过滤器表达式代码必须是我前面讲的有效的异常处理回调函数。
 
 我刚才讲的虽然相当简单，但那只不过是隔着有色玻璃看世界罢了。实际它是非常复杂的。首先，你的过滤器表达式代码并不是被操作系统直接调用的。事实上，各个 EXCEPTION_REGISTRATION 结构的handler域都指向了同一个函数。这个函数在Visual C++的运行时库中，它被称为 **__except_handler3** 。正是这个__except_handler3调用了你的过滤器表达式代码，我一会儿再接着说它。
 
 对我前面的简单描述需要修正的另一个地方是，并不是每次进入或退出一个__try块时就创建或撤销一个EXCEPTION_REGISTRATION结构。 相反，在使用SEH的任何函数中只创建一个EXCEPTION_REGISTRATION结构。换句话说，你可以在一个函数中使用多个__try/__except块，但是在堆栈上只创建一个EXCEPTION_REGISTRATION结构。同样，你可以在一个函数中嵌套使用__try块，但Visual C++仍旧只是创建一个EXCEPTION_REGISTRATION结构。
 
 如果整个EXE或DLL只需要单个的异常处理程序（__except_handler3），同时，如果单个的EXCEPTION_REGISTRATION 结构就能处理多个__try块的话，很明显，这里面还有很多东西我们不知道。这个技巧是通过一个通常情况下看不到的表中的数据来完成的。由于本文的目的就是要深入探索结构化异常处理，那就让我们来看一看这些数据结构吧。
 
 ## 四、扩展的异常处理帧
 
  Visual C++ 的 SEH实现并没有使用原始的EXCEPTION_REGISTRATION结构。它在这个结构的末尾添加了一些附加数据。这些附加数据正是允许单个函数 （__except_handler3）处理所有异常并将执行流程传递到相应的过滤器表达式和__except块的关键。我在Visual C++运行时库源代码中的EXSUP.INC文件中找到了有关Visual C++扩展的EXCEPTION_REGISTRATION结构格式的线索。在这个文件中，你会看到以下定义（已经被注释掉了）：
  
  ```cpp
  ;struct _EXCEPTION_REGISTRATION{
;   struct _EXCEPTION_REGISTRATION *prev;
;   void (*handler)(    PEXCEPTION_RECORD,
;                   PEXCEPTION_REGISTRATION,
;                   PCONTEXT,
;                  PEXCEPTION_RECORD);
; struct scopetable_entry *scopetable;
; int trylevel;
; int _ebp;
; PEXCEPTION_POINTERS xpointers;
;};
  ```
   在前面你已经见过前两个域：prev和handler。它们组成了基本的EXCEPTION_REGISTRATION结构。后面三个域：scopetable（作用域表）、trylevel和_ebp是新增加的。scopetable域指向一个scopetable_entry结构 数组，而trylevel域实际上是这个数组的索引。最后一个域_ebp，是EXCEPTION_REGISTRATION结构创建之前栈帧指针（EBP）的值。
   
   _ebp 域成为扩展的EXCEPTION_REGISTRATION结构的一部分并非偶然。它是通过PUSH EBP这条指令被包含进这个结构中的，而大多数函数开头都是这条指令（ **通常编译器并不为使用FPO优化的函数生成标准的堆栈帧，这样其第一条指令可能不是PUSH EBP。但是如果使用了SEH的话，那么无论你是否使用了FPO优化，编译器一定生成标准的堆栈帧** ）。这条指令可以使EXCEPTION_REGISTRATION结构中所有其它的域都可以用一个相对于栈帧指针（EBP）的负偏移来访问。例如 trylevel域在[EBP-04]处，scopetable指针在[EBP-08]处，等等。（也就是说，这个结构是从[EBP-10H]处开始 的。）
   
   紧跟着扩展的EXCEPTION_REGISTRATION结构下面，Visual C++压入了另外两个值。紧跟着（即[EBP-14H]处）的一个DWORD，是为一个指向**EXCEPTION_POINTERS**结构（一个标准的Win32 结构）的指针所保留的空间。这个指针就是你调用GetExceptionInformation这个API时返回的指针。尽管SDK文档暗示**GetExceptionInformation**是一个标准的Win32 API，但事实上它是一个编译器内联函数。当你调用这个函数时，Visual C++生成以下代码：
   
   ```cpp
   MOV EAX,DWORD PTR [EBP-14]
   ```
   
   GetExceptionInformation 是一个编译器内联函数，与它相关的 GetExceptionCode 函数也是如此。此函数实际上只是返回GetExceptionInformation返回的数据结构（EXCEPTION_POINTERS）中的一个结构 （EXCEPTION_RECORD）中的一个域（ExceptionCode）的值。当Visual C++为**GetExceptionCode**函数生成下面的指令时，它到底是想干什么？我把这个问题留给读者。（现在就能理解为什么SDK文档提醒我们要注意这两个函数的使用范围了。）
   
   ```cpp
   MOV EAX,DWORD PTR [EBP-14] ; // 执行完毕，EAX指向EXCEPTION_POINTERS结构
MOV EAX,DWORD PTR [EAX]     ;// 执行完毕，EAX指向EXCEPTION_RECORD结构
MOV EAX,DWORD PTR [EAX]     ;// 执行完毕，EAX中是ExceptionCode的值
```

现在回到扩展的EXCEPTION_REGISTRATION结构上来。在这个结构开始前的8个字节处（即[EBP-18H]处），Visual C++保留了一个DWORD来保存所有 prolog 代码执行完毕之后的堆栈指针（ESP）的值（实际生成的指令为MOV DWORD PTR [EBP-18H], ESP）。这个DWORD中保存的值是函数执行时ESP寄存器的正常值（除了在准备调用其它函数时把参数压入堆栈这个过程会改变ESP寄存器的值并在函数返回时恢复它的值外，函数在执行过程中一般不改变ESP寄存器的值）。

看起来好像我一下子给你灌输了太多的信息，这点我承认。在继续下去之前，让我们先暂停，来回顾一下Visual C++为使用结构化异常处理的函数生成的标准异常堆栈帧，它看起来像下面这个样子：

```cpp
EBP-00 _ebp
EBP-04 trylevel
EBP-08 scopetable数组指针
EBP-0C handler函数地址
EBP-10指向前一个EXCEPTION_REGISTRATION结构
EBP-14 GetExceptionInformation
EBP-18 栈帧中的标准ESP
```

在操作系统看来，只存在组成原始EXCEPTION_REGISTRATION结构的两个域：即 **[EBP-10h]处的prev指针和[EBP-0Ch]处的handler函数指针** 。栈帧中的其它所有内容是针对于Visual C++的。把这个Visual C++生成的标准异常堆栈帧记到脑子里之后，让我们来看一下真正实现编译器层面SEH的这个Visual C++运行时库例程——__except_handler3。

**__except_handler3和scopetable**

 我真的很希望让你看一看Visual C++运行时库源代码，让你自己好好研究一下__except_handler3函数，但是我办不到。因为Microsoft并没有提供。在这里你就将就着看一下我为__except_handler3函数写的伪代码吧（如下所示）。
 
 ```cpp
 int __except_handler3(
struct _EXCEPTION_RECORD * pExceptionRecord,
struct EXCEPTION_REGISTRATION * pRegistrationFrame,
struct _CONTEXT *pContextRecord,
void * pDispatcherContext )
{
LONG filterFuncRet;
LONG trylevel;
EXCEPTION_POINTERS exceptPtrs;
PSCOPETABLE pScopeTable;
CLD // 将方向标志复位（不测试任何条件！）
// 如果没有设置EXCEPTION_UNWINDING标志或EXCEPTION_EXIT_UNWIND标志
// 表明这是第一次调用这个处理程序（也就是说，并非处于异常展开阶段）
if ( ! (pExceptionRecord->ExceptionFlags
        & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND)) )
{
    // 在堆栈上创建一个EXCEPTION_POINTERS结构
    exceptPtrs.ExceptionRecord = pExceptionRecord;
    exceptPtrs.ContextRecord = pContextRecord;
// 把前面定义的EXCEPTION_POINTERS结构的地址放在比
// establisher栈帧低4个字节的位置上。参考前面我讲
// 的编译器为GetExceptionInformation生成的汇编代码*(PDWORD)((PBYTE)pRegistrationFrame - 4) = &exceptPtrs;
// 获取初始的“trylevel”值
trylevel = pRegistrationFrame->trylevel;
// 获取指向scopetable数组的指针 
scopeTable = pRegistrationFrame->scopetable;
search_for_handler:
if ( pRegistrationFrame->trylevel != TRYLEVEL_NONE )
{
    if ( pRegistrationFrame->scopetable[trylevel].lpfnFilter )
    {
        PUSH EBP // 保存这个栈帧指针
// ！！！非常重要！！！切换回原来的EBP。正是这个操作才使得
// 栈帧上的所有局部变量能够在异常发生后仍然保持它的值不变。
EBP = &pRegistrationFrame->_ebp;
// 调用过滤器函数
filterFuncRet = scopetable[trylevel].lpfnFilter();
POP EBP // 恢复异常处理程序的栈帧指针
if ( filterFuncRet != EXCEPTION_CONTINUE_SEARCH )
{
if ( filterFuncRet < 0 ) // EXCEPTION_CONTINUE_EXECUTION
       return ExceptionContinueExecution;
// 如果能够执行到这里，说明返回值为EXCEPTION_EXECUTE_HANDLER
scopetable = pRegistrationFrame->scopetable;
// 让操作系统清理已经注册的栈帧，这会使本函数被递归调用
__global_unwind2( pRegistrationFrame );
// 一旦执行到这里，除最后一个栈帧外，所有的栈帧已经
// 被清理完毕，流程要从最后一个栈帧继续执行
EBP = &pRegistrationFrame->_ebp;
__local_unwind2( pRegistrationFrame, trylevel );
// NLG = "non-local-goto" (setjmp/longjmp stuff)
__NLG_Notify( 1 ); // EAX = scopetable->lpfnHandler
// 把当前的trylevel设置成当找到一个异常处理程序时
// SCOPETABLE中当前正在被使用的那一个元素的内容
pRegistrationFrame->trylevel = scopetable->previousTryLevel;
// 调用__except {}块，这个调用并不会返回
pRegistrationFrame->scopetable[trylevel].lpfnHandler();
}
}
scopeTable = pRegistrationFrame->scopetable;
trylevel = scopeTable->previousTryLevel;
goto search_for_handler;
}
else // trylevel == TRYLEVEL_NONE
{
    return ExceptionContinueSearch;
}
}
else // 设置了EXCEPTION_UNWINDING标志或EXCEPTION_EXIT_UNWIND标志
{
   PUSH EBP // 保存EBP
   EBP = &pRegistrationFrame->_ebp; // 为调用__local_unwind2设置EBP
   __local_unwind2( pRegistrationFrame, TRYLEVEL_NONE )
   POP EBP // 恢复EBP
   return ExceptionContinueSearch;
}
}
```

虽然__except_handler3的代码看起来很多，但是记住一点：它只是一个我在文章开头讲过的异常处理回调函数。它同MYSEH.EXE和 MYSEH2.EXE中的异常回调函数都带有同样的四个参数。__except_handler3大体上可以由第一个if语句分为两部分。这是由于这个函数可以在两种情况下被调用，一次是正常调用，另一次是在展开阶段。其中大部分是在非展开阶段的回调。

__except_handler3 一开始就在堆栈上创建了一个EXCEPTION_POINTERS结构，并用它的两个参数来对这个结构进行初始化。我在伪代码中把这个结构称为 exceptPrts，它的地址被放在[EBP-14h]处。你回忆一下前面我讲的编译器为GetExceptionInformation和 GetExceptionCode函数生成的汇编代码就会意识到，这实际上初始化了这两个函数使用的指针。

接着，__except_handler3从EXCEPTION_REGISTRATION帧中获取当前的trylevel（在[EBP-04h]处）。trylevel变量实际是scopetable数组的索引，而正是这个数组才使得一个函数中的多个__try块和嵌套的__try块能够仅使用一个 EXCEPTION_REGISTRATION结构。每个scopetable元素结构如下：

```cpp
typedef struct _SCOPETABLE
{
   DWORD previousTryLevel;
   DWORD lpfnFilter;
   DWORD lpfnHandler;
} SCOPETABLE, *PSCOPETABLE;
```

SCOPETABLE 结构中的第二个成员和第三个成员比较容易理解。它们分别是过滤器表达式代码的地址和相应的__except块的地址。但是prviousTryLevel成员有点复杂。总之一句话，它用于嵌套的__try块。**这里的关键是 函数中的每个__try块都有一个相应的SCOPETABLE结构** 。

正如我前面所说，当前的trylevel指定了要使用的scopetable数组的哪一个元素，最终也就是指定了过滤器表达式和__except块的地址。 现在想像一下两个__try块嵌套的情形。如果内层__try块的过滤器表达式不处理某个异常，那外层__try块的过滤器表达式就必须处理它。那现在要问，__except_handler3是如何知道SCOPETABLE数组的哪个元素相应于外层的__try块的呢？答案是：外层__try块的索引由 SCOPETABLE结构的previousTryLevel域给出。利用这种机制，你可以嵌套任意层的__try块。previousTryLevel 域就好像是一个函数中所有可能的异常处理程序构成的线性链表中的结点一样。如果trylevel的值为0xFFFFFFFF（实际上就是-1，这个值在 EXSUP.INC中被定义为 TRYLEVEL_NONE ），标志着这个链表结束。

回到__except_handler3的代码中。在获取了当前的trylevel之后，它就调用相应的SCOPETABLE结构中的过滤器表达式代码。如果过滤器表达式返回 EXCEPTION_CONTINUE_SEARCH ，__exception_handler3 移向SCOPETABLE数组中的下一个元素，这个元素的索引由previousTryLevel域给出。如果遍历完整个线性链表（还记得吗？这个链表是由于在一个函数内部嵌套使用__try块而形成的）都没有找到处理这个异常的代码，__except_handler3返回DISPOSITION_CONTINUE_SEARCH （原文如此，但根据_except_handler函数的定义，这个返回值应该为 ExceptionContinueSearch 。实际上这两个常量的值是一样的。我在伪代码中已经将其改正过来了），这导致系统移向下一个EXCEPTION_REGISTRATION帧（这个链表是由于函数嵌套调用而形成的）。

如果过滤器表达式返回 **EXCEPTION_EXECUTE_HANDLER** ， 这意味着异常应该由相应的__except块处理。它同时也意味着所有前面的EXCEPTION_REGISTRATION帧都应该从链表中移除，并且相应的__except块都应该被执行。第一个任务通过调用__global_unwind2来完成的，后面我会讲到这个函数。跳过这中间的一些清理代码， 流程离开__except_handler3转向__except块。令人奇怪的是，流程并不从__except块中返回，虽然是 __except_handler3使用CALL指令调用了它。

当前的trylevel值是如何被设置的呢？它实际上是由编译器隐含处理的。编译器非常机灵地修改这个扩展的EXCEPTION_REGISTRATION 结构中的trylevel域的值（实际上是生成修改这个域的值的代码）。如果你检查编译器为使用SEH的函数生成的汇编代码，就会在不同的地方都看到修改这个位于[EBP-04h]处的trylevel域的值的代码。

__except_handler3 是如何做到既通过CALL指令调用__except块而又不让执行流程返回呢？由于CALL指令要向堆栈中压入了一个返回地址，你可以想象这有可能破坏堆栈。如果你检查一下编译器为__except块生成的代码，你会发现它做的第一件事就是将EXCEPTION_REGISTRATION结构下面8个字节 处（即[EBP-18H]处）的一个DWORD值加载到ESP寄存器中（实际代码为MOV ESP,DWORD PTR [EBP-18H]）,这个值是在函数的prolog代码中被保存在这个位置的（实际代码为MOV DWORD PTR [EBP-18H],ESP）。

## 五、ShowSEHFrames程序

如果你现在觉得已经被EXCEPTION_REGISTRATION、scopetable、trylevel、过滤器表达式以及展开等等之类的词搞得晕头转向的话，那和我最初的感觉一样。但是编译器层面的结构化异常处理方面的知识并不适合一点一点的学。除非你从整体上理解它，否则有很多内容单独看并没有什么意义。当面对大堆的理论时，我最自然的做法就是写一些应用我学到的理论方面的程序。如果它能够按照预料的那样工作，我就知道我的理解（通常）是正确的。

图10是ShowSEHFrame.EXE的源代码。它使用__try/__except块设置了好几个Visual C++ SEH帧。然后它显示每一个帧以及Visual C++为每个帧创建的scopetable的相关信息。这个程序本身并不生成也不依赖任何异常。相反，我使用了多个__try块以强制Visual C++生成多个EXCEPTION_REGISTRATION帧以及相应的scopetable。

**图10　ShowSEHFrames.CPP**

```cpp
//=========================================================
// ShowSEHFrames - Matt Pietrek 1997
// Microsoft Systems Journal, February 1997
// FILE: ShowSEHFrames.CPP
// 使用命令行CL ShowSehFrames.CPP进行编译//=========================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#pragma hdrstop
//-------------------------------------------------------------------
// 本程序仅适用于Visual C++，它使用的数据结构是特定于Visual C++的
//-------------------------------------------------------------------
#ifndef _MSC_VER
#error Visual C++ Required (Visual C++ specific information is displayed)
#endif
//-------------------------------------------------------------------
// 结构定义
//-------------------------------------------------------------------
// 操作系统定义的基本异常帧
struct EXCEPTION_REGISTRATION
{
     EXCEPTION_REGISTRATION* prev;
     FARPROC handler;
};
// Visual C++扩展异常帧指向的数据结构
struct scopetable_entry
{
    DWORD previousTryLevel;
    FARPROC lpfnFilter;
    FARPROC lpfnHandler;
};
// Visual C++使用的扩展异常帧
struct VC_EXCEPTION_REGISTRATION : EXCEPTION_REGISTRATION
{
     scopetable_entry * scopetable;
     int trylevel;
     int _ebp;
};
//----------------------------------------------------------------
// 原型声明
//----------------------------------------------------------------
// __except_handler3是Visual C++运行时库函数，我们想打印出它的地址
// 但是它的原型并没有出现在任何头文件中，所以我们需要自己声明它。
extern "C" int _except_handler3(PEXCEPTION_RECORD,
                                EXCEPTION_REGISTRATION *,
                                PCONTEXT,
                                PEXCEPTION_RECORD);
//-------------------------------------------------------------
// 代码
//-------------------------------------------------------------
//
// 显示一个异常帧及其相应的scopetable的信息
//
void ShowSEHFrame( VC_EXCEPTION_REGISTRATION * pVCExcRec )
{
    printf( "Frame: %08X Handler: %08X Prev: %08X Scopetable: %08X\n",
            pVCExcRec, pVCExcRec->handler, pVCExcRec->prev,
            pVCExcRec->scopetable );
scopetable_entry * pScopeTableEntry = pVCExcRec->scopetable;
for ( unsigned i = 0; i <= pVCExcRec->trylevel; i++ )
{
    printf( " scopetable[%u] PrevTryLevel: %08X "
            "filter: %08X __except: %08X\n", i,
            pScopeTableEntry->previousTryLevel,
            pScopeTableEntry->lpfnFilter,
            pScopeTableEntry->lpfnHandler );
pScopeTableEntry++;
}
printf( "\n" );
}
//
// 遍历异常帧的链表，按顺序显示它们的信息
//
void WalkSEHFrames( void )
{
   VC_EXCEPTION_REGISTRATION * pVCExcRec;
// 打印出__except_handler3函数的位置
printf( "_except_handler3 is at address: %08X\n", _except_handler3 );
printf( "\n" );
// 从FS:[0]处获取指向链表头的指针
__asm mov eax, FS:[0]
__asm mov [pVCExcRec], EAX
// 遍历异常帧的链表。0xFFFFFFFF标志着链表的结尾
while ( 0xFFFFFFFF != (unsigned)pVCExcRec )
{
   ShowSEHFrame( pVCExcRec );
   pVCExcRec = (VC_EXCEPTION_REGISTRATION *)(pVCExcRec->prev);
}
}
void Function1( void )
{
   // 嵌套3层__try块以便强制为scopetable数组产生3个元素
   __try
   {
         __try
         {
             __try
             {
                 WalkSEHFrames(); // 现在显示所有的异常帧的信息
             } __except( EXCEPTION_CONTINUE_SEARCH )
              {}
        } __except( EXCEPTION_CONTINUE_SEARCH )
          {}
    } __except( EXCEPTION_CONTINUE_SEARCH )
      {}
}
int main()
{
int i;
// 使用两个__try块（并不嵌套），这导致为scopetable数组生成两个元素
__try
{
    i = 0x1234;
} __except( EXCEPTION_CONTINUE_SEARCH )
{
      i = 0x4321;
}
__try
{
    Function1(); // 调用一个设置更多异常帧的函数
} __except( EXCEPTION_EXECUTE_HANDLER )
{
       // 应该永远不会执行到这里，因为我们并没有打算产生任何异常
       printf( "Caught Exception in main\n" );
}
return 0;
}
```

ShowSEHFrames 程序中比较重要的函数是WalkSEHFrames和ShowSEHFrame。WalkSEHFrames函数首选打印出 __except_handler3的地址，打印它的原因很快就清楚了。接着，它从FS:[0]处获取异常链表的头指针，然后遍历该链表。此链表中每个结 点都是一个VC_EXCEPTION_REGISTRATION类型的结构，它是我自己定义的，用于描述Visual C++的异常处理帧。对于这个链表中的每个结点，WalkSEHFrames都把指向这个结点的指针传递给ShowSEHFrame函数。

ShowSEHFrame 函数一开始就打印出异常处理帧的地址、异常处理回调函数的地址、前一个异常处理帧的地址以及scopetable的地址。接着，对于每个 scopetable数组中的元素，它都打印出其priviousTryLevel、过滤器表达式的地址以及相应的__except块的地址。我是如何知道scopetable数组中有多少个元素的呢？其实我并不知道。但是我假定 VC_EXCEPTION_REGISTRATION 结构中的当前trylevel域的值比scopetable数组中的元素总数少1。

图11是ShowSEHFrames的运行结果。首先检查以“Frame:”开头的每一行，你会发现它们显示的异常处理帧在堆栈上的地址呈递增趋势，并且在前三个帧中，它们的异常处理程序的地址是一样的（都是004012A8）。再看输出的开始部分，你会发现这个004012A8不是别的，它正是 Visual C++运行时库函数__except_handler3的地址。这证明了我前面所说的单个回调函数处理所有异常这一点。

![](https://github.com/ashenone0917/image/blob/main/o_pietrk11.jpg)

**图11　ShowSEHFrames运行结果**

你可能想知道为什么明明ShowSEHFrames程序只有两个函数使用SEH，但是却有三个异常处理帧使用__except_handler3作为它们的异常回调函数。实际上第三个帧来自Visual C++运行时库。Visual C++运行时库源代码中的CRT0.C文件清楚地表明了对main或WinMain的调用也被一个__try/__except块封装着。这个__try 块的过滤器表达式代码可以在WINXFLTR.C文件中找到。

回到ShowSEHFrames程序，注意到最后一个帧的异常处理程序的地址是77F3AB6C，这与其它三个不同。仔细观察一下，你会发现这个地址在 KERNEL32.DLL中。这个特别的帧就是由KERNEL32.DLL中的BaseProcessStart函数安装的，这在前面我已经说过。

## 六、展开

在挖掘展开（Unwinding）的实现代码之前让我们先来搞清楚它的意思。我在前面已经讲过所有可能的异常处理程序是如何被组织在一个由线程信息块的第一个DWORD（FS:[0]）所指向的链表中的。由于针对某个特定异常的处理程序可能不在这个链表的开头，因此就需要从链表中依次移除实际处理异常的那个 异常处理程序之前的所有异常处理程序。

正如你在Visual C++的__except_handler3函数中看到的那样，展开是由 __global_unwind2 这个运行时库（RTL）函数来完成的。这个函数只是对 RtlUnwind 这个未公开的API进行了非常简单的封装。（现在这个API已经被公开了，但给出的信息极其简单，详细信息可以参考最新的Platform SDK文档。）

```cpp
__global_unwind2(void * pRegistFrame)
{
    _RtlUnwind( pRegistFrame, &__ret_label, 0, 0 );
    __ret_label:
}
```

虽然从技术上讲RtlUnwind是一个KERNEL32函数，但它只是转发到了NTDLL.DLL中的同名函数上。图12是我为此函数写的伪代码。

**图12 RtlUnwind函数的伪代码**

```cpp
void _RtlUnwind( PEXCEPTION_REGISTRATION pRegistrationFrame,
                PVOID returnAddr, // 并未使用！（至少是在i386机器上）
                PEXCEPTION_RECORD pExcptRec,
                DWORD _eax_value)
{
   DWORD stackUserBase;
   DWORD stackUserTop;
   PEXCEPTION_RECORD pExcptRec;
   EXCEPTION_RECORD exceptRec;
   CONTEXT context;
// 从FS:[4]和FS:[8]处获取堆栈的界限
RtlpGetStackLimits( &stackUserBase, &stackUserTop );
   if ( 0 == pExcptRec ) // 正常情况
   {
       pExcptRec = &excptRec;
      pExcptRec->ExceptionFlags = 0;
      pExcptRec->ExceptionCode = STATUS_UNWIND;
      pExcptRec->ExceptionRecord = 0;
      pExcptRec->ExceptionAddress = [ebp+4]; // RtlpGetReturnAddress()—获取返回地址
      pExcptRec->ExceptionInformation[0] = 0;
   }
if ( pRegistrationFrame )
       pExcptRec->ExceptionFlags |= EXCEPTION_UNWINDING;
    else             // 这两个标志合起来被定义为EXCEPTION_UNWIND_CONTEXT
      pExcptRec->ExceptionFlags|=(EXCEPTION_UNWINDING|EXCEPTION_EXIT_UNWIND);
context.ContextFlags =( CONTEXT_i486 | CONTEXT_CONTROL |
CONTEXT_INTEGER | CONTEXT_SEGMENTS);
RtlpCaptureContext( &context );
context.Esp += 0x10;
context.Eax = _eax_value;
PEXCEPTION_REGISTRATION pExcptRegHead;
pExcptRegHead = RtlpGetRegistrationHead(); // 返回FS:[0]的值
// 开始遍历EXCEPTION_REGISTRATION结构链表
while ( -1 != pExcptRegHead )
{
   EXCEPTION_RECORD excptRec2;
if ( pExcptRegHead == pRegistrationFrame )
{
   NtContinue( &context, 0 );
}
else
{
   // 如果存在某个异常帧在堆栈上的位置比异常链表的头部还低
   // 说明一定出现了错误
   if ( pRegistrationFrame && (pRegistrationFrame <= pExcptRegHead) )
   {
      // 生成一个异常
      excptRec2.ExceptionRecord = pExcptRec;
      excptRec2.NumberParameters = 0;
      excptRec2.ExceptionCode = STATUS_INVALID_UNWIND_TARGET;
      excptRec2.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
      RtlRaiseException( &exceptRec2 );
    }
}
PVOID pStack = pExcptRegHead + 8; // 8 = sizeof(EXCEPTION_REGISTRATION)
// 确保pExcptRegHead在堆栈范围内，并且是4的倍数
if ( (stackUserBase <= pExcptRegHead )
      && (stackUserTop >= pStack )
      && (0 == (pExcptRegHead & 3)) )
{
   DWORD pNewRegistHead;
   DWORD retValue;
retValue = RtlpExecutehandlerForUnwind(pExcptRec, pExcptRegHead, &context,
&pNewRegistHead, pExceptRegHead->handler );
if ( retValue != DISPOSITION_CONTINUE_SEARCH )
{
   if ( retValue != DISPOSITION_COLLIDED_UNWIND )
   {
      excptRec2.ExceptionRecord = pExcptRec;
      excptRec2.NumberParameters = 0;
      excptRec2.ExceptionCode = STATUS_INVALID_DISPOSITION;
      excptRec2.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
      RtlRaiseException( &excptRec2 );
   }
   else
      pExcptRegHead = pNewRegistHead;
}
PEXCEPTION_REGISTRATION pCurrExcptReg = pExcptRegHead;
pExcptRegHead = pExcptRegHead->prev;
RtlpUnlinkHandler( pCurrExcptReg );
}
else // 堆栈已经被破坏！生成一个异常
{
   excptRec2.ExceptionRecord = pExcptRec;
   excptRec2.NumberParameters = 0;
   excptRec2.ExceptionCode = STATUS_BAD_STACK;
   excptRec2.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
   RtlRaiseException( &excptRec2 );
}
}
// 如果执行到这里，说明已经到了EXCEPTION_REGISTRATION
     // 结构链表的末尾，正常情况下不应该发生这种情况。
     //（因为正常情况下异常应该被处理，这样就不会到链表末尾）
     if ( -1 == pRegistrationFrame )
         NtContinue( &context, 0 );
     else
         NtRaiseException( pExcptRec, &context, 0 );
}
```

RtlUnwind 函数的伪代码到这里就结束了，以下是它调用的几个函数的伪代码：

```cpp
PEXCEPTION_REGISTRATION RtlpGetRegistrationHead( void )
{
   return FS:[0];
}
RtlpUnlinkHandler( PEXCEPTION_REGISTRATION pRegistrationFrame )
{
   FS:[0] = pRegistrationFrame->prev;
}
void RtlpCaptureContext( CONTEXT * pContext )
{
   pContext->Eax = 0;
   pContext->Ecx = 0;
   pContext->Edx = 0;
   pContext->Ebx = 0;
   pContext->Esi = 0;
   pContext->Edi = 0;
   pContext->SegCs = CS;
   pContext->SegDs = DS;
   pContext->SegEs = ES;
   pContext->SegFs = FS;
   pContext->SegGs = GS;
   pContext->SegSs = SS;
   pContext->EFlags = flags; // 它对应的汇编代码为__asm{ PUSHFD / pop [xxxxxxxx] }
   pContext->Eip = 此函数的调用者的调用者的返回地址    // 读者看一下这个函数的
   pContext->Ebp = 此函数的调用者的调用者的EBP        // 汇编代码就会清楚这一点
   pContext->Esp = pContext->Ebp + 8;
}
```

虽然RtlUnwind函数的规模看起来很大，但是如果你按一定方法把它分开，其实并不难理解。它首先从FS:[4]和FS:[8]处获取当前线程堆栈的界限。它们对于后面要进行的合法性检查非常重要，以确保所有将要被展开的异常帧都在堆栈范围内。

RtlUnwind 接着在堆栈上创建了一个空的EXCEPTION_RECORD结构并把STATUS_UNWIND赋给它的ExceptionCode域，同时把 EXCEPTION_UNWINDING标志赋给它的ExceptionFlags域。指向这个结构的指针作为其中一个参数被传递给每个异常回调函数。然后，这个函数调用RtlCaptureContext函数来创建一个空的CONTEXT结构，这个结构也变成了在展开阶段调用每个异常回调函数时传递给它们的一个参数。

RtlUnwind 函数的其余部分遍历EXCEPTION_REGISTRATION结构链表。对于其中的每个帧，它都调用 RtlpExecuteHandlerForUnwind函数，后面我会讲到这个函数。正是这个函数带EXCEPTION_UNWINDING标志调用了异常处理回调函数。每次回调之后，它调用RtlpUnlinkHandler移除相应的异常帧。

RtlUnwind 函数的第一个参数是一个帧的地址，当它遍历到这个帧时就停止展开异常帧。上面所说的这些代码之间还有一些安全性检查代码，它们用来确保不出问题。如果出现任何问题，RtlUnwind就引发一个异常，指示出了什么问题，并且这个异常带有EXCEPTION_NONCONTINUABLE标志。当一个进程被设 置了这个标志时，它就不允许再运行，必须终止。

## 七、未处理异常

在文章的前面，我并没有全面描述UnhandledExceptionFilter这个API。通常情况下你并不直接调用它（尽管你可以这么做）。大多数情况下它都是由KERNEL32中进行默认异常处理的过滤器表达式代码调用。前面BaseProcessStart函数的伪代码已经表明了这一点。

图13是我为UnhandledExceptionFilter函数写的伪代码。这个API有点奇怪（至少在我看来是这样）。如果异常的类型是 EXCEPTION_ACCESS_VIOLATION，它就调用_BasepCheckForReadOnlyResource。虽然我没有提供这个函数的伪代码，但可以简要描述一下。如果是因为要对EXE或DLL的资源节（.rsrc）进行写操作而导致的异常，_BasepCurrentTopLevelFilter就改变出错页面正常的只读属性，以便允许进行写操作。如果是这种特殊的情况，UnhandledExceptionFilter返回EXCEPTION_CONTINUE_EXECUTION，使系统重新执行出错指令。

**图13 UnHandledExceptionFilter函数的伪代码**

```cpp
UnhandledExceptionFilter( STRUCT _EXCEPTION_POINTERS *pExceptionPtrs )
{
PEXCEPTION_RECORD pExcptRec;
DWORD currentESP;
DWORD retValue;
DWORD DEBUGPORT;
DWORD dwTemp2;
DWORD dwUseJustInTimeDebugger;
CHAR szDbgCmdFmt[256]; // 从AeDebug这个注册表键值返回的字符串
CHAR szDbgCmdLine[256]; // 实际的调试器命令行参数（已填入进程ID和事件ID）
STARTUPINFO startupinfo;
PROCESS_INFORMATION pi;
HARDERR_STRUCT harderr; // ???
BOOL fAeDebugAuto;
TIB * pTib; // 线程信息块
 
pExcptRec = pExceptionPtrs->ExceptionRecord;
if ( (pExcptRec->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
      && (pExcptRec->ExceptionInformation[0]) )
{
retValue=BasepCheckForReadOnlyResource(pExcptRec->ExceptionInformation[1]);
if ( EXCEPTION_CONTINUE_EXECUTION == retValue )
return EXCEPTION_CONTINUE_EXECUTION;
}
// 查看这个进程是否运行于调试器下
retValue = NtQueryInformationProcess(GetCurrentProcess(), ProcessDebugPort,
                                     &debugPort, sizeof(debugPort), 0 );
if ( (retValue >= 0) && debugPort ) // 通知调试器
return EXCEPTION_CONTINUE_SEARCH;
// 用户调用SetUnhandledExceptionFilter了吗？
// 如果调用了，那现在就调用他安装的异常处理程序
if ( _BasepCurrentTopLevelFilter )
{
   retValue = _BasepCurrentTopLevelFilter( pExceptionPtrs );
   if ( EXCEPTION_EXECUTE_HANDLER == retValue )
      return EXCEPTION_EXECUTE_HANDLER;
   if ( EXCEPTION_CONTINUE_EXECUTION == retValue )
      return EXCEPTION_CONTINUE_EXECUTION;
// 只有返回值为EXCEPTION_CONTINUE_SEARCH时才会继续执行下去
}
// 调用过SetErrorMode(SEM_NOGPFAULTERRORBOX)吗？
{
   harderr.elem0 = pExcptRec->ExceptionCode;
   harderr.elem1 = pExcptRec->ExceptionAddress;
if ( EXCEPTION_IN_PAGE_ERROR == pExcptRec->ExceptionCode )
harderr.elem2 = pExcptRec->ExceptionInformation[2];
else
harderr.elem2 = pExcptRec->ExceptionInformation[0];
dwTemp2 = 1;
fAeDebugAuto = FALSE;
harderr.elem3 = pExcptRec->ExceptionInformation[1];
pTib = FS:[18h];
DWORD someVal = pTib->pProcess->0xC;
if ( pTib->threadID != someVal )
{
   __try
{
     char szDbgCmdFmt[256];
     retValue = GetProfileStringA( "AeDebug", "Debugger", 0,
                        szDbgCmdFmt, sizeof(szDbgCmdFmt)-1 );
if ( retValue )
dwTemp2 = 2;
char szAuto[8];
retValue = GetProfileStringA( "AeDebug", "Auto", "0",
                    szAuto, sizeof(szAuto)-1 );
if ( retValue )
if ( 0 == strcmp( szAuto, "1" ) )
   if ( 2 == dwTemp2 )
      fAeDebugAuto = TRUE;
}
__except( EXCEPTION_EXECUTE_HANDLER )
{
   ESP = currentESP;
   dwTemp2 = 1;
   fAeDebugAuto = FALSE;
}
}
if ( FALSE == fAeDebugAuto )
{
   retValue=NtRaiseHardError(STATUS_UNHANDLED_EXCEPTION | 0x10000000,
               4, 0, &harderr,_BasepAlreadyHadHardError ? 1 : dwTemp2,
               &dwUseJustInTimeDebugger );
}
else
{
   dwUseJustInTimeDebugger = 3;
   retValue = 0;
}
if (retValue >= 0 && (dwUseJustInTimeDebugger == 3)
&& (!_BasepAlreadyHadHardError)&&(!_BaseRunningInServerProcess))
{
_BasepAlreadyHadHardError = 1;
SECURITY_ATTRIBUTES secAttr = { sizeof(secAttr), 0, TRUE };
HANDLE hEvent = CreateEventA( &secAttr, TRUE, 0, 0 );
memset( &startupinfo, 0, sizeof(startupinfo) );
sprintf(szDbgCmdLine, szDbgCmdFmt, GetCurrentProcessId(), hEvent);
startupinfo.cb = sizeof(startupinfo);
startupinfo.lpDesktop = "Winsta0\Default"
CsrIdentifyAlertableThread(); // ???
retValue = CreateProcessA( 0,           // 应用程序名称
szDbgCmdLine, // 命令行
0, 0,          // 进程和线程安全属性
1,             // bInheritHandles
0, 0,          // 创建标志、环境
0,             // 当前目录
&statupinfo, // STARTUPINFO
&pi);          // PROCESS_INFORMATION
if ( retValue && hEvent )
{
   NtWaitForSingleObject( hEvent, 1, 0 );
   return EXCEPTION_CONTINUE_SEARCH;
}
}
if ( _BasepAlreadyHadHardError )
NtTerminateProcess(GetCurrentProcess(), pExcptRec->ExceptionCode);
}
return EXCEPTION_EXECUTE_HANDLER;
}
LPTOP_LEVEL_EXCEPTION_FILTER
SetUnhandledExceptionFilter(
          LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter )
{
   // _BasepCurrentTopLevelFilter是KERNEL32.DLL中的一个全局变量
   LPTOP_LEVEL_EXCEPTION_FILTER previous= _BasepCurrentTopLevelFilter;
// 设置为新值
_BasepCurrentTopLevelFilter = lpTopLevelExceptionFilter;
return previous; // 返回以前的值
}
```

UnhandledExceptionFilter 接下来的任务是确定进程是否运行于Win32调试器下。也就是进程的创建标志中是否带有标志 **DEBUG_PROCESS** 或 **DEBUG_ONLY_THIS_PROCESS** 。 它使用NtQueryInformationProcess函数来确定进程是否正在被调试，我在本月的Under the Hood专栏中讲解了这个函数。如果正在被调试，UnhandledExceptionFilter就返回 EXCEPTION_CONTINUE_SEARCH，这告诉系统去唤醒调试器并告诉它在被调试程序（debuggee）中产生了一个异常。

UnhandledExceptionFilter 接下来调用用户安装的未处理异常过滤器（如果存在的话）。通常情况下，用户并没有安装回调函数，但是用户可以调用 SetUnhandledExceptionFilter这个API来安装。上面我也提供了这个API的伪代码。这个函数只是简单地用用户安装的回调函数的地址来替换一个全局变量，并返回替换前的值。

有了初步的准备之后，UnhandledExceptionFilter就开始做它的主要工作：用一个时髦的应用程序错误对话框来通知你犯了低级的编程错误。有两种方法可以避免出现这个对话框。第一种方法是调用SetErrorMode函数并指定SEM_NOGPFAULTERRORBOX标志。另一种方法是将AeDebug子键下的Auto的值设为1。此时UnhandledExceptionFilter跳过应用程序错误对话框直接启动AeDebug子键下的Debugger的值所指定的调试器。如果你熟悉“即时调试（Just In Time Debugging，JIT）”的话，这就是操作系统支持它的地方。接下来我会详细讲。

大多数情况下，上面的两个条件都为假。这样UnhandledExceptionFilter就调用NTDLL.DLL中的 NtRaiseHardError函数。正是这个函数产生了应用程序错误对话框。这个对话框等待你单击“确定”按钮来终止进程，或者单击“取消”按钮来调试它。（单击“取消”按钮而不是“确定”按钮来加载调试器好像有点颠倒了，可能这只是我个人的感觉吧。）

如果你单击“确定”，UnhandledExceptionFilter就返回EXCEPTION_EXECUTE_HANDLER。调用UnhandledExceptionFilter 的进程通常通过终止自身来作为响应（正像你在BaseProcessStart的伪代码中看到的那样）。这就产生了一个有趣的问题——大多数人都认为是系统终止了产生未处理异常的进程，而实际上更准确的说法应该是，系统进行了一些设置使得产生未处理异常的进程将自身终止掉了。

UnhandledExceptionFilter 执行时真正有意思的部分是当你单击应用程序错误对话框中的“取消”按钮，此时系统将调试器附加（attach）到出错进程上。这段代码首先调用 CreateEvent来创建一个事件内核对象，调试器成功附加到出错进程之后会将此事件对象变成有信号状态。这个事件句柄以及出错进程的ID都被传到sprintf函数，由它将其格式化成一个命令行，用来启动调试器。一切就绪之后，UnhandledExceptionFilter就调用 CreateProcess来启动调试器。如果CreateProcess成功，它就调用NtWaitForSingleObject来等待前面创建的那个事件对象。此时这个调用被阻塞，直到调试器进程将此事件变成有信号状态，以表明它已经成功附加到出错进程上。 UnhandledExceptionFilter函数中还有一些其它的代码，我在这里只讲重要的。

## 八、进入地狱

如果你已经走了这么远，不把整个过程讲完对你有点不公平。我已经讲了当异常发生时操作系统是如何调用用户定义的回调函数的。我也讲了这些回调的内部情况，以及编译器是如何使用它们来实现__try和__except的。我甚至还讲了当某个异常没有被处理时所发生的情况以及系统所做的扫尾工作。剩下的就只有异常回调过程最初是从哪里开始的这个问题了。好吧，让我们深入系统内部来看一下结构化异常处理的开始阶段吧。

图14是我为KiUserExceptionDispatcher函数和一些相关函数写的伪代码。这个函数在NTDLL.DLL中，它是异常处理执行的起点。为了绝对准确起见，我必须指出：刚才说的并不是绝对准确。例如在Intel平台上，一个异常导致CPU将控制权转到ring 0（0特权级，即内核模式）的一个处理程序上。这个处理程序由中断描述符表（Interrupt Descriptor Table，IDT）中的一个元素定义，它是专门用来处理相应异常的。我跳过所有的内核模式代码，假设当异常发生时CPU直接将控制权转到了 KiUserExceptionDispatcher函数。

**图14 KiUserExceptionDispatcher的伪代码**

```cpp
KiUserExceptionDispatcher( PEXCEPTION_RECORD pExcptRec, CONTEXT * pContext )
{
   DWORD retValue;
// 注意：如果异常被处理，那么RtlDispatchException函数就不会返回
if ( RtlDispatchException( pExceptRec, pContext ) )
   retValue = NtContinue( pContext, 0 );
else
   retValue = NtRaiseException( pExceptRec, pContext, 0 );
EXCEPTION_RECORD excptRec2;
excptRec2.ExceptionCode = retValue;
excptRec2.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
excptRec2.ExceptionRecord = pExcptRec;
excptRec2.NumberParameters = 0;
RtlRaiseException( &excptRec2 );
}
int RtlDispatchException( PEXCEPTION_RECORD pExcptRec, CONTEXT * pContext )
{
   DWORD stackUserBase;
   DWORD stackUserTop;
   PEXCEPTION_REGISTRATION pRegistrationFrame;
   DWORD hLog;
// 从FS:[4]和FS:[8]处获取堆栈的界限
RtlpGetStackLimits( &stackUserBase, &stackUserTop );
pRegistrationFrame = RtlpGetRegistrationHead();
while ( -1 != pRegistrationFrame )
{
   PVOID justPastRegistrationFrame = &pRegistrationFrame + 8;
   if ( stackUserBase > justPastRegistrationFrame )
   {
      pExcptRec->ExceptionFlags |= EH_STACK_INVALID;
      return DISPOSITION_DISMISS; // 0
   }
if ( stackUsertop < justPastRegistrationFrame )
{
   pExcptRec->ExceptionFlags |= EH_STACK_INVALID;
   return DISPOSITION_DISMISS; // 0
}
if ( pRegistrationFrame & 3 ) // 确保堆栈按DWORD对齐
{
   pExcptRec->ExceptionFlags |= EH_STACK_INVALID;
   return DISPOSITION_DISMISS; // 0
}
if ( someProcessFlag )
{
   hLog = RtlpLogExceptionHandler( pExcptRec, pContext, 0,
                                 pRegistrationFrame, 0x10 );
}
DWORD retValue, dispatcherContext;
retValue= RtlpExecuteHandlerForException(pExcptRec, pRegistrationFrame,
pContext, &dispatcherContext,
pRegistrationFrame->handler );
if ( someProcessFlag )
RtlpLogLastExceptionDisposition( hLog, retValue );
if ( 0 == pRegistrationFrame )
{
   pExcptRec->ExceptionFlags &= ~EH_NESTED_CALL; // 关闭标志
}
EXCEPTION_RECORD excptRec2;
DWORD yetAnotherValue = 0;
if ( DISPOSITION_DISMISS == retValue )
{
   if ( pExcptRec->ExceptionFlags & EH_NONCONTINUABLE )
   {
     excptRec2.ExceptionRecord = pExcptRec;
      excptRec2.ExceptionNumber = STATUS_NONCONTINUABLE_EXCEPTION;
      excptRec2.ExceptionFlags    = EH_NONCONTINUABLE;
      excptRec2.NumberParameters = 0;
      RtlRaiseException( &excptRec2 );
   }
   else
      return DISPOSITION_CONTINUE_SEARCH;
}
else if ( DISPOSITION_CONTINUE_SEARCH == retValue )
{}
else if ( DISPOSITION_NESTED_EXCEPTION == retValue )
{
   pExcptRec->ExceptionFlags |= EH_EXIT_UNWIND;
   if ( dispatcherContext > yetAnotherValue )
       yetAnotherValue = dispatcherContext;
}
else // DISPOSITION_COLLIDED_UNWIND
{
   excptRec2.ExceptionRecord = pExcptRec;
   excptRec2.ExceptionNumber = STATUS_INVALID_DISPOSITION;
   excptRec2.ExceptionFlags    = EH_NONCONTINUABLE;
   excptRec2.NumberParameters = 0;
   RtlRaiseException( &excptRec2 );
}
pRegistrationFrame = pRegistrationFrame->prev; // 转到前一个帧
}
return DISPOSITION_DISMISS;
}
_RtlpExecuteHandlerForException: // 处理异常（第一次）
MOV EDX,XXXXXXXX
JMP ExecuteHandler
RtlpExecutehandlerForUnwind: // 处理展开（第二次）
MOV EDX,XXXXXXXX
int ExecuteHandler( PEXCEPTION_RECORD pExcptRec,
PEXCEPTION_REGISTRATION pExcptReg,
CONTEXT * pContext,
PVOID pDispatcherContext,
FARPROC handler ) // 实际上是指向_except_handler()的指针
{
   // 安装一个EXCEPTION_REGISTRATION帧，EDX指向相应的handler代码
   PUSH EDX
   PUSH FS:[0]
   MOV FS:[0],ESP
   // 调用异常处理回调函数
EAX = handler( pExcptRec, pExcptReg, pContext, pDispatcherContext );
// 移除EXCEPTION_REGISTRATION帧
MOV ESP,DWORD PTR FS:[00000000]
POP DWORD PTR FS:[00000000]
return EAX;
}
_RtlpExecuteHandlerForException使用的异常处理程序：
{
   // 如果设置了展开标志，返回DISPOSITION_CONTINUE_SEARCH
   // 否则，给pDispatcherContext赋值并返回DISPOSITION_NESTED_EXCEPTION
   return pExcptRec->ExceptionFlags & EXCEPTION_UNWIND_CONTEXT ?
            DISPOSITION_CONTINUE_SEARC : ( *pDispatcherContext = 
                                           pRegistrationFrame->scopetable,
                                           DISPOSITION_NESTED_EXCEPTION );
}
_RtlpExecuteHandlerForUnwind使用的异常处理程序：
{
   // 如果设置了展开标志，返回DISPOSITION_CONTINUE_SEARCH
    // 否则，给pDispatcherContext赋值并返回DISPOSITION_COLLIDED_UNWIND
return pExcptRec->ExceptionFlags & EXCEPTION_UNWIND_CONTEXT ?
            DISPOSITION_CONTINUE_SEARCH : ( *pDispatcherContext = 
                                           pRegistrationFrame->scopetable,
                                            DISPOSITION_COLLIDED_UNWIND );
}
```

KiUserExceptionDispatcher 的核心是对RtlDispatchException的调用。这拉开了搜索已注册的异常处理程序的序幕。如果某个处理程序处理这个异常并继续执行，那么对 RtlDispatchException的调用就不会返回。如果它返回了，只有两种可能：或者调用了NtContinue以便让进程继续执行，或者产生了新的异常。如果是这样，那异常就不能再继续处理了，必须终止进程。

现在把目光对准 **RtlDispatchException** 函数的代码，这就是我通篇提到的遍历异常帧的代码。这个函数获取一个指向EXCEPTION_REGISTRATION结构链表的指针，然后遍历此链表以寻找一个异常处理程序。由于堆栈可能已经被破坏了，所以这个例程非常谨慎。在调用每个EXCEPTION_REGISTRATION结构中指定的异常处理程序之前，它确保这个结构是按DWORD对齐的，并且是在线程的堆栈之中，同时在堆栈中比前一个EXCEPTION_REGISTRATION结构高。

RtlDispatchException 并不直接调用EXCEPTION_REGISTRATION结构中指定的异常处理程序。相反，它调用 RtlpExecuteHandlerForException来完成这个工作。根据RtlpExecuteHandlerForException的执行情况，RtlDispatchException或者继续遍历异常帧，或者引发另一个异常。这第二次的异常表明异常处理程序内部出现了错误，这样就不能继续执行下去了。

RtlpExecuteHandlerForException 的代码与RtlpExecuteHandlerForUnwind的代码极其相似。你可能会回忆起来在前面讨论展开时我提到过它。这两个“函数”都只是简单地给EDX寄存器加载一个不同的值然后就调用ExecuteHandler函数。也就是说，RtlpExecuteHandlerForException和RtlpExecuteHandlerForUnwind都是ExecuteHanlder这个公共函数的前端。

ExecuteHandler 查找EXCEPTION_REGISTRATION结构的handler域的值并调用它。令人奇怪的是，对异常处理回调函数的调用本身也被一个结构化异常处 理程序封装着。在SEH自身中使用SEH看起来有点奇怪，但你思索一会儿就会理解其中的含义。如果在异常回调过程中引发了另外一个异常，操作系统需要知道这个情况。根据异常发生在最初的回调阶段还是展开回调阶段，ExecuteHandler或者返回 **DISPOSITION_NESTED_EXCEPTION** ，或者返回 **DISPOSITION_COLLIDED_UNWIND** 。这两者都是“红色警报！现在把一切都关掉！”类型的代码。

如果你像我一样，那不仅理解所有与SEH有关的函数非常困难，而且记住它们之间的调用关系也非常困难。为了帮助我自己记忆，我画了一个调用关系图（图15）。

现在要问：在调用ExecuteHandler之前设置EDX寄存器的值有什么用呢？这非常简单。如果ExecuteHandler在调用用户安装的异常处 理程序的过程中出现了什么错误，它就把EDX指向的代码作为原始的异常处理程序。它把EDX寄存器的值压入堆栈作为原始的 EXCEPTION_REGISTRATION结构的handler域。这基本上与我在MYSEH和MYSEH2中对原始的结构化异常处理的使用情况一 样。

![](https://github.com/ashenone0917/image/blob/main/o_pietrk15.jpg)

**图15 在SEH中是谁调用了谁**

## 九、结论

结构化异常处理是Win32一个非常好的特性。多亏有了像Visual C++之类的编译器的支持层对它的封装，一般的程序员才能付出比较小的学习代价就能利用SEH所提供的便利。但是在操作系统层面上，事情远比Win32文档说的复杂。

不幸的是，由于人人都认为系统层面的SEH是一个非常困难的问题，因此至今这方面的资料都不多。在本文中，我已经向你指出了系统层面的SEH就是围绕着简单的回调在打转。如果你理解了回调的本质，在此基础上分层理解，系统层面的结构化异常处理也不是那么难掌握。

## 附录：关于prolog和epilog

美国英语中的“prolog”实际上就是“prologue”。从这个词的意思“序幕、序言”就能大致猜出它的作用。一个函数的prolog代码主要是为这个函数的执行做一些准备工作，例如设置堆栈帧、设置局部变量所使用的堆栈空间以及保存相关的寄存器等。标准的prolog代码开头一般为以下三条指令：

```cpp
PUSH        EBP
MOV     EBP, ESP
SUB     ESP, XXX
```

上面的三条指令为使用EBP寄存器来访问函数的参数（正偏移）和局部变量（负偏移）做好了准备。例如按照__stdcall调用约定，调用者 （caller）将被调函数（callee）的参数从右向左压入堆栈，然后用CALL指令调用这个函数。CALL指令将返回地址压入堆栈，然后流程就转到 了被调函数的prolog代码。此时[ESP]中是返回地址，[ESP+4]中是函数的第一个参数。本来可以就这样使用ESP寄存器来访问参数，但由于PUSH和POP指令会隐含修改ESP寄存器的值，这样同一个参数在不同时刻可能需要通过不同的指令形式来访问（例如，如果现在向堆栈中压入一个值的话， 那访问第一个参数就需要使用[ESP+8]了）。为了解决这个问题，所以使用EBP寄存器。EBP寄存器被称为栈帧（frame）指针，它正是用于此目的。当上述prolog指令中的前两条指令执行后，就可以使用EBP来访问参数了，并且在整个函数中都不会改变此寄存器的值。在前面的例子中， [EBP+8]处就是第一个参数的值，[EBP+0Ch]处是第二个参数的值，依次类推。

大多数C/C++编译器都有“栈帧指针省略（ Frame-Pointer Omission ）” 这个选项（在Microsoft C/C++编译器中为/Oy，VS2010工程属性中就有这个配置项），它导致函数使用ESP来访问参数，从而可以空闲出一个寄存器（EBP）用于其它目的，并且由于不需要设置堆栈帧，从而会稍微提高运行速度。但是在某些情况下必须使用堆栈帧。作者在前面也提到过，Microsoft已经在其MSDN文档中指明：结构化异常处理是 **基于帧** 的异常处理。也就是说，它必须使用堆栈帧。当你查看编译器为使用SEH的函数生成的汇编代码时就会清楚这一点。无论你是否使用/Oy选项，它都设置堆栈帧。

可能有的读者在调试应用程序时偶然进入到了系统DLL（例如NTDLL.DLL）中，但是却意外地发现许多函数的prolog代码的第一条指令并不是上面所说的“PUSH EBP”，而是一条“垃圾”指令——“MOV EDI, EDI”（这条指令占两个字节）。Microsoft C/C++编译器被称为优化编译器，它怎么可能生成这么一条除了占用空间之外别无它用的指令呢？实际上，如果你比较细心的话，会发现以这条指令开头的函数 的前面有5条NOP指令（它们一共占5个字节），如下图所示。

![](https://github.com/ashenone0917/image/blob/main/o_st3.JPG)

考虑一下使用JMP指令进行近跳转和远跳转分别需要几个字节？他们正好分别是2个字节和5个字节！这难道是巧合？熟悉API拦截的读者可能已经猜到了，它们是供拦截API时使用的。实际上，这是Microsoft对系统打“热补丁”（Hot Patching）时拦截API用的。在打“热补丁“时，修补程序在5条NOP指令处写入一个远跳转指令，以跳转到被修补过的代码处。而“MOV EDI, EDI”处用一个近跳转指令覆盖，它跳转到5个NOP指令所在的位置。使用“MOV EDI, EDI”而不是直接使用两个NOP指令是出于性能考虑。

第三条指令用于为局部变量保留空间，其中的XXX就是需要保留的字节数。不使用局部变量的函数没有这条指令。另外，如果局部变量比较少的话——例如2个，为了性能考虑，编译器往往会使用类似于两条“PUSH ECX”这样的指令来为局部变量保留空间。这三条指令后面一般还有几条PUSH指令用于保存函数使用的寄存器（一般是EBX、ESI和EDI）。

与prolog代码相对的就是epilog代码。与prolog类似，从它的意思“尾声、结尾”也能猜出它的作用。它主要做一些清理工作。标准的epilog代码如下：

```cpp
MOV     ESP, EBP
POP     EBP
RET     XXX
```

 这三条指令前面可能还有几条POP指令用于恢复在prolog代码中保存的寄存器（如果存在的话）。有了前面的分析，epilog代码不言自明。需要说明的一点是，最后的RET指令用于返回调用者，并从堆栈中弹出无用信息，XXX指定了弹出的字节数。它一般用于将参数弹出堆栈。因此从这个值就可以知道函数的参数个数（每个参数均为4字节）。
 
 为了简化这种操作，Intel引入了ENTER和LEAVE指令。其中ENTER相当于前面所说的prolog代码的前两条指令，而LEAVE相当于上面的epilog代码的前两条指令。但由于实现上ENTER指令比前面所说的两条指令执行速度慢，因此编译器都不使用这条指令。这样，你实际看到的情况就是：prolog代码就是前面所说的那样，但epilog代码使用了LEAVE指令。
 
 >https://blog.csdn.net/chenlycly/article/details/52575260
