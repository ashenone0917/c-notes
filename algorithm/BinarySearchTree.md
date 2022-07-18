```cpp
// BinarySearchTree.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

struct BinarySearchNode {
    int value_ = 0;
    BinarySearchNode* left_ = nullptr;
    BinarySearchNode* right_ = nullptr;
    BinarySearchNode() = default;
    BinarySearchNode(int value) : value_(value) {};
};
class BinarySearchTree {
public:
    void insert(int value) {
        if (tree_ == nullptr) {
            tree_ = new BinarySearchNode(value);
            return;
        }

        auto p = tree_;
        while (p != nullptr) {
            if (value > p->value_) {
                if (p->right_ == nullptr) {
                    p->right_ = new BinarySearchNode(value);
                    return;
                }
                p = p->right_;
            }
            else if (value < p->value_) {
                if (p->left_ == nullptr) {
                    p->left_ = new BinarySearchNode(value);
                    return;
                }
                p = p->left_;
            }
            else return;
        }

        return;
    }

    BinarySearchNode* find(int value) {
        auto p = tree_;
        while (p != nullptr) {
            if (value > p->value_) p = p->right_;
            else if (value < p->value_) p = p->left_;
            else return p;
        }
        return nullptr;
    }

    void deleteNode(int value) {
        auto p = tree_;

    }

    BinarySearchTree() = default;
    ~BinarySearchTree() {
        freeTree(tree_);
        tree_ = nullptr;
    }
private:
    void freeTree(BinarySearchNode* node) {
        if (node == nullptr) return;
        freeTree(node->left_);
        freeTree(node->right_);
        delete node;
    }

private:
    BinarySearchNode* tree_ = nullptr;
};
int main()
{
    std::cout << "Hello World!\n";
}
```
