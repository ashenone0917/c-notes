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


```
.PHONY: all clean
MKDIR = mkdir
RM = rm
RMFLAGS = -fr
CC = gcc
DIR_OBJS = objs
DIR_EXES = exes
DIR_DEPS = deps
DIRS = $(DIR_OBJS) $(DIR_EXES) $(DIR_DEPS)
EXE = complicated
EXE := $(addprefix $(DIR_EXES)/, $(EXE))
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
OBJS := $(addprefix $(DIR_OBJS)/, $(OBJS))
DEPS = $(SRCS:.c=.dep)
DEPS := $(addprefix $(DIR_DEPS)/, $(DEPS))
all: $(DIRS) $(DEPS) $(EXE)
-include $(DEPS)
$(DIRS):
        $(MKDIR) $@
$(EXE): $(DIR_EXES) $(OBJS)
        $(CC) -o $@ $(filter %.o, $^)
$(DIR_OBJS)/%.o: $(DIR_OBJS) %.c
        $(CC) -o $@ -c $(filter %.c, $^)$(DIR_DEPS)/%.dep: $(DIR_DEPS) %.c
$(DIR_DEPS)/%.dep: $(DIR_DEPS) %.c
        @echo "Making $@ ..."
        @set -e; \
        $(RM) $(RMFLAGS) $@.tmp ; \
        $(CC) -E -MM $(filter %.c, $^) > $@.tmp ; \
        sed 's,\(.*\)\.o[ :]*,objs/\1.o: ,g' < $@.tmp > $@ ; \
        $(RM) $(RMFLAGS) $@.tmp
clean:
        $(RM) $(RMFLAGS) $(DIRS)
//这个makefile会无限递归,输出为

mkdir deps
Making deps/main.dep ...
Making deps/foo.dep ...
Making deps/main.dep ...
Making deps/main.dep ...
Making deps/foo.dep ...
Making deps/main.dep ...
Making deps/foo.dep ...
Making deps/main.dep ...
Making deps/foo.dep ...
Making deps/main.dep ...
...
//递归的原因是顺序问题，
//修改为以下则不会无限递归
.PHONY: all clean
MKDIR = mkdir
RM = rm
RMFLAGS = -fr
CC = gcc

DIR_OBJS = objs
DIR_EXES = exes
DIR_DEPS = deps
DIRS = $(DIR_OBJS) $(DIR_EXES) $(DIR_DEPS)

EXE = complicated
EXE := $(addprefix $(DIR_EXES)/, $(EXE))
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
OBJS := $(addprefix $(DIR_OBJS)/, $(OBJS))
DEPS = $(SRCS:.c=.dep)
DEPS := $(addprefix $(DIR_DEPS)/, $(DEPS))

all: $(EXE)

-include $(DEPS)

$(DIRS):
	$(MKDIR) $@
	@echo "$@"

$(EXE): $(DIR_EXES) $(OBJS)
	$(CC) -o $@ $(filter %.o, $^)

$(DIR_OBJS)/%.o: %.c | $(DIR_OBJS) // "| $(DIR_OBJS)"表示依赖关系中的顺序关系，即在生成obj文件之前需要先创建$(DIR_OBJS)目录
	$(CC) -o $@ -c $(filter %.c, $^)

$(DIR_DEPS)/%.dep: %.c | $(DIR_DEPS)// "| $(DIR_DEPS)"表示依赖关系中的顺序关系，即在生成依赖文件之前需要先创建$(DIR_DEPS)目录
	@echo "Making $@ ..."
	@set -e; \
	$(RM) $(RMFLAGS) $@.tmp ; \
	$(CC) -E -MM  $^ > $@.tmp ; \
	sed 's,\(.*\)\.o[ :]*,objs/\1.o: ,g' < $@.tmp > $@ ; \
	$(RM) $(RMFLAGS) $@.tmp

clean:
	$(RM) $(RMFLAGS) $(DIRS)

```
