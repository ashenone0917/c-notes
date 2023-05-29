```
.PHONY: all clean
MKDIR = mkdir
RM = rm
RMFLAGS = -fr
DIRS = objs exes
all: $(DIRS)
$(DIRS):
        $(MKDIR) $@
        @echo "$@"
clean:
        $(RM) $(RMFLAGS) $(DIRS)
        
//make后先执行all，all发现有依赖，解析依赖为objs exes
//依赖从左到右按顺序处理
//先处理objs，会匹配到$(DIRS)，objs exes
//然后执行 $(MKDIR) $@ @echo "$@"
//
```

```
.PHONY: clean
CC = gcc
RM = rm
EXE = simple
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS)) -》把.c 替换为.o
$(EXE): $(OBJS)//触发了xxx.o的依赖之后会匹配到下面的%.o: %.c去生成依赖
        $(CC) -o $@ $^
%.o: %.c   //Makefile中的%.o表示匹配任意以.o结尾的文件名，其中%表示一个通配符，可以匹配任意字符串，所以上面依赖的xxx.o会匹配到这里，然后会依赖xxx.c
        $(CC) -o $@ -c $^
clean:
        $(RM) $(EXE) $(OBJS)
```
