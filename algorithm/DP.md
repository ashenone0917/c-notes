动态规划（Dynamic Programming, DP）是一种算法设计技术，主要用于解决具有重叠子问题和最优子结构特性的问题。原理是将原始问题拆分为更小的子问题，解决这些子问题，然后将结果存储起来，以便之后再次遇到同样的子问题时直接使用，避免重复计算。动态规划既可以用于求解最优化问题，也可以用于计数问题。

### 动态规划的原理
重叠子问题：
问题的解决方案包含了多个相同的子问题而不是总是生成新的子问题。
例如，在计算斐波那契数列时，f(n) = f(n-1) + f(n-2)，两个子问题 f(n-1) 和 f(n-2) 又将分别需要计算 f(n-2) 和 f(n-3)。
最优子结构：
原问题的最优解包含了其子问题的最优解。
例如，在最短路径问题中，从点A到点C的最短路径必须包含从点A到点B的最短路径。
存储解决方案：
存储子问题的解决方案以避免重复计算。这通常使用一个数组或哈希表。
这称为记忆化，在自顶向下的实现中常见，或者在自底向上的方法中顺序填充表格。
构造解决方案：
如果问题求具体的解决方案（而不只是求解的值），通常需要从存储的信息中回溯构造解决方案。
### 例题与实现

下面这个例子展示了动态规划的典型应用：求解0-1背包问题。

> 0-1背包问题：给定一个固定大小、能够携带重量W的背包以及一组有重量和价值的物品，找出一个最佳的解决方案，以满载背包所能携带的物品的最大价值。
```cpp
#include <iostream>
#include <vector>
#include <algorithm>

int knapsack(const std::vector<int>& weights, const std::vector<int>& values, int W) {
    int N = weights.size();
    std::vector<std::vector<int>> dp(N + 1, std::vector<int>(W + 1, 0));

    for (int i = 1; i <= N; ++i) {
        for (int w = 1; w <= W; ++w) {
            if (weights[i - 1] <= w) {
                dp[i][w] = std::max(dp[i - 1][w], values[i - 1] + dp[i - 1][w - weights[i - 1]]);
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    return dp[N][W];
}

int main() {
    std::vector<int> weights = {2, 1, 3, 2}; // 物品的重量
    std::vector<int> values = {12, 10, 20, 15}; // 物品的价值
    int W = 5; // 背包的容量

    std::cout << "Maximum value in knapsack = " << knapsack(weights, values, W) << std::endl;
    
    return 0;
}
```
在这个例子中，`dp[i][w]` 存储了考虑前 `i` 件物品，在不超过重量 `w` 的情况下能得到的最大价值。外层循环遍历所有物品，内层循环遍历所有可能的重量。对每个物品，我们决定“不放入背包”和“放入背包”哪个能得到更大的价值，并存储结果。最后的 `dp[N][W]` 就是问题的解决方案。

动态规划问题的关键是定义递推关系，理解如何从已计算的子问题解中找到当前问题的解。然后通过代码实现递推更新过程，并存储中间结果，在最后取得最终问题的解。
