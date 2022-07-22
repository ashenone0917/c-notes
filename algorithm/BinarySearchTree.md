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
			else {
				if (p->left_ == nullptr) {
					p->left_ = new BinarySearchNode(value);
					return;
				}
				p = p->left_;
			}
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

	BinarySearchNode* getMin() {
		return getMin(tree_);
	}

	BinarySearchNode* getMax() {
		return getMax(tree_);
	}

	BinarySearchNode* getPostNode(BinarySearchNode* node) {
		if (node == nullptr) return nullptr;

		if (node->right_ != nullptr) return getMin(node->right_);

		auto p = tree_;
		BinarySearchNode* successor = nullptr;
		while (p != nullptr) {
			if (node->value_ < p->value_) {
				successor = p;
				p = p->left_;
			}
			else {
				p = p->right_;
			}
		}
	}

	BinarySearchNode* getPostNode(int value) {
		auto p = tree_;
		BinarySearchNode* successor = nullptr;
		while (p != nullptr) {
			if (value < p->value_) {
				successor = p;
				p = p->left_;
			}
			else {
				p = p->right_;
			}
		}

		return successor;
	}

	BinarySearchNode* getPreNode(int value) {
		auto p = tree_;
		BinarySearchNode* successor = nullptr;
		while (p != nullptr) {
			if (p->value_ < value) {
				successor = p;
				p = p->right_;
			}
			else p = p->left_;
		}
		return successor;
	}

	//中位数的前驱节点，其实就是小于((等于)如果允许重复的话)此节点的最大节点
	BinarySearchNode* getPreNode(BinarySearchNode* node) {
		if (node == nullptr) return nullptr;

		//如果存在左子树，则找到左子树最大的节点
		if (node->left_) return getMax(node->left_);

		auto p = tree_;
		BinarySearchNode* successor = nullptr;
		while (p != nullptr) {
			if (node->value_ > p->value_) {
				//如果找到比node小的数，先暂存，然后往节点的右子树找，找到一个更大的比node更小的数
				successor = p;
				p = p->right_;
			}
			else {
				//如果节点大于等于node，则去找他的左子树，因为他的左子树才比node小
				p = p->left_;
			}
		}
	}

	void printOrder() {
		printOrder(tree_);
	}

	BinarySearchTree() = default;
	~BinarySearchTree() {
		freeTree(tree_);
		tree_ = nullptr;
	}
private:
	void printOrder(BinarySearchNode* node) {
		if (node == nullptr) return;
		printOrder(node->left_);
		std::cout << node->value_ << std::endl;
		printOrder(node->right_);
	}

	void freeTree(BinarySearchNode* node) {
		if (node == nullptr) return;
		freeTree(node->left_);
		freeTree(node->right_);
		delete node;
	}

	BinarySearchNode* getMax(BinarySearchNode* node) {
		if (node == nullptr) return nullptr;

		auto p = node;
		while (p->right_ != nullptr) p = p->right_;
		return p;
	}

	BinarySearchNode* getMin(BinarySearchNode* node) {
		if (node == nullptr) return nullptr;

		auto p = node;
		while (p->left_ != nullptr) p = p->left_;
		return p;
	}

private:
	BinarySearchNode* tree_ = nullptr;
};
int main()
{
	{
		BinarySearchTree bst;
		bst.insert(5);
		bst.insert(5);
		bst.insert(6);
		bst.insert(10);
		bst.insert(10);
		bst.insert(2);
		bst.insert(2);
		bst.insert(3);
		bst.insert(1);
		bst.printOrder();
		auto pr = bst.getPostNode(5);
		pr = bst.getPostNode(10);
		auto fr = bst.find(2);
		auto dr = bst.deleteNode(8);
		dr = bst.deleteNode(2);
		fr = bst.find(2);
		fr = bst.find(3);
		bst.printOrder();
	}

	std::cout << "Hello World!\n";
}
```
