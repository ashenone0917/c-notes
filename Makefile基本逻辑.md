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
