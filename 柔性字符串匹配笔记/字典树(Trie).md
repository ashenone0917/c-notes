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
//内存大概是下面这种格式
// 1 2
// 5
//
// 4    3
//
//          6  7 8 9
// 10
//
//
//
//
//
//
//                   14
//
//             13
//
//
//
//
//
//
//
//
//     12
// 11 
```
## 应用
### 检索字符串
字典树最基础的应用——查找一个字符串是否在“字典”中出现过。
给你n个名字串，然后进行m次点名，每次你需要回答“名字不存在”、“第一次点到这个名字”、“已经点过这个名字”之一。

1≤n≤10⁴，1≤m≤10⁵，所有字符串长度不超过50。
```cpp
#include <cstdio>

const int N = 500010; //50 * 10⁴(输入的字符数)

char s[60];
int n, m, ch[N][26], tag[N], tot = 1;

int main() {
  scanf("%d", &n);

  for (int i = 1; i <= n; ++i) {
    scanf("%s", s + 1);
    int u = 1;
    for (int j = 1; s[j]; ++j) {
      int c = s[j] - 'a';
      if (!ch[u][c])
        ch[u][c] =
            ++tot;  // 如果这个节点的子节点中没有这个字符，添加上并将该字符的节点号记录为++tot
      u = ch[u][c];  // 往更深一层搜索
    }
    tag[u] = 1;  // 最后一个字符为节点 u 的名字未被访问到记录为 1
  }

  scanf("%d", &m);

  while (m--) {
    scanf("%s", s + 1);
    int u = 1;
    for (int j = 1; s[j]; ++j) {
      int c = s[j] - 'a';
      u = ch[u][c];
      if (!u) break;  // 不存在对应字符的出边说明名字不存在
    }
    if (tag[u] == 1) {
      tag[u] = 2;  // 最后一个字符为节点 u 的名字已经被访问
      puts("OK");
    } else if (tag[u] == 2)  // 已经被访问，重复访问
      puts("REPEAT");
    else
      puts("WRONG");
  }

  return 0;
}
```

