## 定义
字典树，英文名 trie。顾名思义，就是一个像字典一样的树。
## 引入
如下图：

![](https://github.com/ashenone0917/image/blob/main/trie1.png)

可以发现，这棵字典树用边来代表字母，而从根结点到树上某一结点的路径就代表了一个字符串。举个例子，1->4->8->12表示的就是字符串 caa。

trie 的结构非常好懂，我们用δ(u,s)表示结点的u的c字符指向的下一个结点，或着说是结点u代表的字符串后面添加一个字符c形成的字符串的结点。（c的取值范围和字符集大小有关，不一定是 0~26。）

有时需要标记插入进 trie 的是哪些字符串，每次插入完成时在这个字符串所代表的节点处打上标记即可。

## 实现
```cpp
// C++ Version
struct trie {
  int nex[100000][26], cnt;
  bool exist[100000];  // 该结点结尾的字符串是否存在

  void insert(char *s, int l) {  // 插入字符串
    int p = 0;
    for (int i = 0; i < l; i++) {
      int c = s[i] - 'a';
      if (!nex[p][c]) nex[p][c] = ++cnt;  // 如果没有，就添加结点
      p = nex[p][c];
    }
    exist[p] = 1;
  }

  bool find(char *s, int l) {  // 查找字符串
    int p = 0;
    for (int i = 0; i < l; i++) {
      int c = s[i] - 'a';
      if (!nex[p][c]) return 0;
      p = nex[p][c];
    }
    return exist[p];
  }
};
```

