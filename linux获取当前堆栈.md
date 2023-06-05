```cpp
/* 调用栈的最大深度 */
#define MAX_STACK_DEPTH 16

/* 打印调用栈函数 */
void dump_stack_user() {
    void *stack_trace[MAX_STACK_DEPTH] = {0};
    char **stack_strings = NULL;
    int stack_depth = 0;
    int i = 0;

    /* 获取栈中各层调用函数地址 */
    stack_depth = backtrace(stack_trace, MAX_STACK_DEPTH);

    /* 查找符号表将函数调用地址转换为函数名称 */
    stack_strings = (char **)backtrace_symbols(stack_trace, stack_depth);
    if (NULL == stack_strings) {
        printf(" Memory is not enough while dump Stack Trace! \r\n");
        return;
    }

    /* 打印调用栈 */
    printf(" Stack Trace: \r\n");
    for (i = 0; i < stack_depth; ++i) {
        printf(" [%d] %s \r\n", i, stack_strings[i]);
    }

    /* 获取函数名称时申请的内存需要自行释放 */
    free(stack_strings);
    stack_strings = NULL;

    return;
}
```
