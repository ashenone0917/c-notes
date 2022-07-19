```cpp
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

	bool deleteNode(int value) {
		auto p = tree_;
		BinarySearchNode* pp = nullptr;
		while (p != nullptr && p->value_ != value) {
			pp = p;
			if (value > p->value_) p = p->right_;
			else p = p->left_;
		}

		if (p == nullptr) return false;

		if (p->left_ != nullptr && p->right_ != nullptr) {
			auto minP = p->right_;
			BinarySearchNode* minPP = p;
			while (minP->left_ != nullptr) {
				minPP = minP;
				minP = minP->left_;
			}

			p->value_ = minP->value_;
			p = minP;
			pp = minPP;
		}

		BinarySearchNode* child = nullptr;
		if (p->left_ != nullptr) child = p->left_;
		else if (p->right_ != nullptr) child = p->right_;

		if (pp == nullptr) {
			delete tree_;
			tree_ = nullptr;
		}
		else if (pp->left_ == p) {
			delete p;
			pp->left_ = child;
		}
		else if (pp->right_ == p) {
			delete p;
			pp->right_ = child;
		}

		return true;
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
	{
		BinarySearchTree bst;
		bst.insert(5);
		bst.insert(10);
		bst.insert(2);
		bst.insert(3);
		bst.insert(1);
		auto fr = bst.find(2);
		auto dr = bst.deleteNode(8);
		dr = bst.deleteNode(2);
		fr = bst.find(2);
		fr = bst.find(3);
	}
	
	std::cout << "Hello World!\n";
}
```
