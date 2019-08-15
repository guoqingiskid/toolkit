#pragma once
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <initializer_list>
#undef _DATA_TEST

/** 最基本的排序二叉树 */
template<typename T>
class BaseBinaryTree
{
public:
	template<typename U>
	struct Node
	{
		U _val;
		Node *_parent;
		Node *_left;
		Node *_right;
		size_t _height;

		Node(const U &val, size_t height, Node *parent, Node *left, Node *right) : _val(val), _height(height), _parent(parent), _left(left), _right(right) {}
		Node() :_val(U()), _left(nullptr), _parent(nullptr), _right(nullptr),_height(0) {}
		~Node() 
		{
			std::cout << "Node: " << _val << " decontruction"<< std::endl;
			delete _left;
			delete _right;
		}
	};

	BaseBinaryTree() :_root(nullptr) 
	{
		/***/
#ifdef  _DATA_TEST
		std::vector<int> vec{ 10,5,6,7,9,13,4 };
		_root = new Node<T>(vec[0], nullptr, nullptr, nullptr);
		auto leftPtr = _root->_left = new Node<T>(vec[1], _root, nullptr, nullptr);
		auto rightPtr = _root->_right = new Node<T>(vec[2], _root, nullptr, nullptr);
		leftPtr->_left = new Node<T>(vec[3], leftPtr, nullptr, nullptr);
		leftPtr->_right = new Node<T>(vec[4], leftPtr, nullptr, nullptr);
		rightPtr->_left = new Node<T>(vec[5], rightPtr, nullptr, nullptr);
		rightPtr->_right = new Node<T>(vec[6], rightPtr, nullptr, nullptr);
#endif //  _DATA_TEST
	}

	BaseBinaryTree(const std::initializer_list<T> &args) :BaseBinaryTree()
	{
		for (auto ptr = args.begin(); ptr != args.end(); ++ptr)
		{
			insert(*ptr);
		}
	}

	~BaseBinaryTree()
	{
		delete _root;
	}

public:
	void preOrder()
	{
		_preOrder(_root);
	}

	void inOrder()
	{
		_inOrder(_root);
		std::cout << std::endl;
	}

	void postOrder()
	{
		_postOrder(_root);
	}

	/** 深度优先遍历 */
	void depthFirst()
	{
		std::deque<Node<T> *> que;   //利用一个队列来保存节点的下层子节点
		if (!_root)
			return;
		que.push_back(_root);
		while (!que.empty())
		{
			Node<T> *node = que.front();
			que.pop_front();
			std::cout << node->_val << "\t";
			if(node->_left)  que.push_back(node->_left);
			if(node->_right) que.push_back(node->_right);
		}
	}

	virtual void insert(const T &val)
	{
		++_node_size;
		_insert(val, _root);
	}

	/** 排序树的查找元素操作是天然的二叉搜索 */
	bool findNode(const T &val)
	{
		return _find(val, _root) == nullptr ? false : true;
	}

	/** 排序树的删除操作：情况比较多，稍有些复杂 */
	virtual void erase(const T &val)
	{
		_erase(val);
	}

private:

	/** 树前序遍历 */
	void _preOrder(Node<T> *node)
	{
		if (!node)
			return;
			
		/** 简单打印出来，可以使用policy来处理 */
		std::cout << node->_val << "\t";
		_preOrder(node->_left);
		_preOrder(node->_right);
	}

	/** 中序遍历 */
	void _inOrder(Node<T> *node)
	{
		if (!node)
			return;

		/** 简单打印出来 */
		_inOrder(node->_left);
		std::cout << node->_val << "\t";
		_inOrder(node->_right);
	}

	/** 后序遍历 */
	void _postOrder(Node<T> *node)
	{
		if (!node)
			return;

		/** 简单打印出来 */
		_postOrder(node->_left);
		_postOrder(node->_right);
		std::cout << node->_val << "\t";
	}
	
	/** 排序二叉树的插入元素操作非常简单，较小数往树左边走，较大树往右边走 */
	void _insert(const T &val, Node<T> *ptr)
	{
		if (!_root)
		{
			_root = new Node<T>(val, 0, nullptr, nullptr, nullptr);
			return;
		}

		if (ptr)
		{
			if (ptr->_val > val)
			{
				if (!ptr->_left)
					ptr->_left = new Node<T>(val, ptr->_height + 1, ptr, nullptr, nullptr);
				else
					_insert(val, ptr->_left);
			}
			else
				if (!ptr->_right)
					ptr->_right = new Node<T>(val, ptr->_height + 1, ptr, nullptr, nullptr);
				else
					_insert(val, ptr->_right);
		}
	}

	void _erase(const T &val)
	{
		Node<T> *ptr = _find(val, _root);
		if (ptr)
		{
			/** 第一种情况：删除的节点是叶子节点 */
			if (ptr->_left == nullptr && ptr->_right == nullptr)
			{
				if (ptr->_parent->_left == ptr)
				{
					ptr->_parent->_left = nullptr;
				}
				else
				{
					ptr->_parent->_right = nullptr;
				}
			}
			else if (ptr->_left && ptr->_right == nullptr)			/** 第二种情况 */
			{
				if (ptr->_parent->_left == ptr)
				{
					ptr->_parent->_left = ptr->_left;
				}
				else
				{
					ptr->_parent->_right = ptr->_left;					
				}
				ptr->_left->_parent = ptr->_parent;
			}
			else if (ptr->_right && ptr->_left == nullptr)			/** 第二种情况 */
			{
				if (ptr->_parent->_left == ptr)
				{
					ptr->_parent->_left = ptr->_right;					
				}
				else
				{
					ptr->_parent->_right = ptr->_right;
				}
				ptr->_right->_parent = ptr->_parent;
			}
			else													/** 第三种情况：直接在左子树上找最大值节点来替换，然后内部再做调整 */
			{
				Node<T> *maxValNode = findMaxVal(ptr->_left);
				if (maxValNode == ptr->_left)
				{
					if (ptr->_parent->_left == ptr)
					{
						ptr->_parent->_left = maxValNode;
					}
					else
					{
						ptr->_parent->_right = maxValNode;
					}
					maxValNode->_right = ptr->_right;
					maxValNode->_parent = ptr->_parent;
				}
				else
				{
					/** 该节点不可能还有右节点，因此仅需将左子节点调整即可 */
					if (maxValNode->_left)
					{
						maxValNode->_parent->_right = maxValNode->_left;
					}

					if (ptr->_parent->_left == ptr)
					{
						ptr->_parent->_left = maxValNode;
					}
					else
					{
						ptr->_parent->_right = maxValNode;
					}
					maxValNode->_right = ptr->_right;
					maxValNode->_left = ptr->_left;
					maxValNode->_parent = ptr->_parent;
				}
			}

			ptr->_left = nullptr;
			ptr->_right = nullptr;
			delete ptr;
		}
	}

protected:
		Node<T> *_find(const T &val, Node<T> *ptr)
		{
			if (!ptr)
				return nullptr;

			if (ptr->_val == val)
				return ptr;
			else if (ptr->_val > val)
				return _find(val, ptr->_left);
			else
				return _find(val, ptr->_right);
		}

		Node<T> *findMaxVal(Node<T> *node)
		{
			if (node->_right)
			{
				return findMaxVal(node->_right);
			}
			else
				return node;
		}
protected:
	Node<T> *_root;
	size_t _node_size;
};

/** AVL树 */
template<typename T>
class AVLTree : public BaseBinaryTree<T>
{
public:

	AVLTree() {}
	AVLTree(const std::initializer_list<T> &args) : AVLTree<T>()
	{
		for (auto ptr = args.begin(); ptr != args.end(); ++ptr)
		{
			insert(*ptr);
		}
	}

	void insert(const T &val)override
	{
		_insert(val, _root);
	}

	void erase(const T &val)override
	{
		_erase(val);
	}

	size_t height()const
	{
		return height(_root);
	}

private:
	void _insert(const T &val, Node<T> *ptr)
	{
		if (!_root)
		{
			_root = new Node<T>(val, 1, nullptr, nullptr, nullptr);
			return;
		}

		if (ptr)
		{
			if (ptr->_val > val)
			{
				if (!ptr->_left)
				{
					ptr->_left = new Node<T>(val, 1, ptr, nullptr, nullptr);
					change_height(ptr->_left, ptr->_left->_height);

					/** 检测平衡，并解决 */
					check_and_deal(ptr->_left);
				}					
				else
					_insert(val, ptr->_left);
			}
			else
				if (!ptr->_right)
				{
					ptr->_right = new Node<T>(val, 1, ptr, nullptr, nullptr);
					change_height(ptr->_right, ptr->_right->_height);

					/** 检测平衡，并解决 */
					check_and_deal(ptr->_right);
				}					
				else
					_insert(val, ptr->_right);
		}
	}

	void _erase(const T &val)
	{
		Node<T> *ptr = _find(val, _root);
		if (ptr)
		{
			/** 第一种情况：删除的节点是叶子节点 */
			if (ptr->_left == nullptr && ptr->_right == nullptr)
			{
				if (ptr->_parent->_left == ptr)
				{
					ptr->_parent->_left = nullptr;
				}
				else
				{
					ptr->_parent->_right = nullptr;
				}
				ptr->_height = 0;
				change_height(ptr, ptr->_height);
			}
			else if (ptr->_left && ptr->_right == nullptr)			/** 第二种情况 */
			{
				if (ptr->_parent->_left == ptr)
				{
					ptr->_parent->_left = ptr->_left;
				}
				else
				{
					ptr->_parent->_right = ptr->_left;
				}
				ptr->_left->_parent = ptr->_parent;
				ptr->_height = 0;
				change_height(ptr, ptr->_height);
			}
			else if (ptr->_right && ptr->_left == nullptr)			/** 第二种情况 */
			{
				if (ptr->_parent->_left == ptr)
				{
					ptr->_parent->_left = ptr->_right;
				}
				else
				{
					ptr->_parent->_right = ptr->_right;
				}
				ptr->_right->_parent = ptr->_parent;
				ptr->_height = 0;
				change_height(ptr, ptr->_height);
			}
			else													/** 第三种情况：直接在左子树上找最大值节点来替换，然后内部再做调整 */
			{
				Node<T> *maxValNode = findMaxVal(ptr->_left);
				if (maxValNode == ptr->_left)
				{
					if (ptr->_parent->_left == ptr)
					{
						ptr->_parent->_left = maxValNode;
					}
					else
					{
						ptr->_parent->_right = maxValNode;
					}
					maxValNode->_right = ptr->_right;
					maxValNode->_parent = ptr->_parent;
					ptr->_height = 0;
					change_height(ptr, ptr->_height);
				}
				else
				{
					/** 该节点不可能还有右节点，因此仅需将左子节点调整即可 */
					if (maxValNode->_left)
					{
						maxValNode->_parent->_right = maxValNode->_left;
					}

					if (ptr->_parent->_left == ptr)
					{
						ptr->_parent->_left = maxValNode;
					}
					else
					{
						ptr->_parent->_right = maxValNode;
					}
					maxValNode->_right = ptr->_right;
					maxValNode->_left = ptr->_left;
					maxValNode->_parent = ptr->_parent;
					ptr->_height = 0;
					change_height(ptr, ptr->_height);
				}
			}

			/** 检测平衡，并解决 */
			if(ptr->_left) check_and_deal(ptr->_left);
			else if(ptr->_right) check_and_deal(ptr->_right);
			else check_and_deal(ptr);

			ptr->_left = nullptr;
			ptr->_right = nullptr;
			delete ptr;
		}
	}

	size_t height(Node<T> *ptr)const
	{
		if (ptr)
			return ptr->_height;
		else
			return 0;
	}

	void change_height(Node<T> *ptr, size_t height)
	{
		if (ptr->_parent && ptr->_parent->_height == height + 1)
			return;

		int l_tmp = 0, r_tmp = 0;
		while (ptr && ptr->_parent)
		{
			l_tmp = ptr->_parent->_left ? ptr->_parent->_left->_height : 0;
			r_tmp = ptr->_parent->_right ? ptr->_parent->_right->_height : 0;
			int max_val = std::max(l_tmp, r_tmp);
			ptr->_parent->_height = max_val + 1;
			ptr = ptr->_parent;
		}
	}

	void check_and_deal(Node<T> *ptr)
	{
		if (!ptr)
			return;

		/** 回溯检测 */
		if (height(ptr->_left) - height(ptr->_right) == 2)
		{
			/** LL,LR */
			if (height(ptr->_left->_left) > height(ptr->_left->_right))
				LLRotation(ptr);
			else
				LRRotation(ptr);
		}
		else if (height(ptr->_right) - height(ptr->_left) == 2)
		{
			/** RR,RL */
			if (height(ptr->_right->_left) > height(ptr->_right->_right))
				RLRotation(ptr);
			else
				RRRotation(ptr);
		}
		else
		{
			/** 没检测到，继续检测 */
			ptr = ptr->_parent;
			check_and_deal(ptr);
		}
	}

	/** 单旋转：LL，RR */
	void LLRotation(Node<T> *k2)
	{
		Node<T> *k1 = k2->_left;
		k2->_left = k1->_right;
		if(k1->_right)
			k1->_right->_parent = k2;
		k1->_right = k2;
		k1->_parent = k2->_parent;
		if (k2->_parent && k2->_parent->_left == k2)
			k2->_parent->_left = k1;
		else if (k2->_parent)
			k2->_parent->_right = k1;
		else
			_root = k1;
		k2->_parent = k1;
		k2->_height = std::max(height(k2->_left), height(k2->_right)) + 1;
		k1->_height = std::max(height(k1->_left), height(k1->_right)) + 1;	
		change_height(k1, k1->_height);
	}

	void RRRotation(Node<T> *k2)
	{
		Node<T> *k1 = k2->_right;
		k2->_right = k1->_left;
		if(k1->_left)
			k1->_left->_parent = k2;
		k1->_left = k2;
		k1->_parent = k2->_parent;
		if (k2->_parent && k2->_parent->_left == k2)
			k2->_parent->_left = k1;
		else if (k2->_parent)
			k2->_parent->_right = k1;
		else
			_root = k1;
		k2->_parent = k1;
		k2->_height = std::max(height(k2->_left), height(k2->_right)) + 1;
		k1->_height = std::max(height(k1->_left), height(k1->_right)) + 1;
		change_height(k1, k1->_height);
	}

	/** 双旋转： LR，RL */
	void LRRotation(Node<T> *k3)
	{
		RRRotation(k3->_left);
		LLRotation(k3);
	}

	void RLRotation(Node<T> *k3)
	{
		LLRotation(k3->_right);
		RRRotation(k3);
	}
};


/** 简单B树：2-3树，删除操作比较复杂暂时未实现 */
template<typename T>
class TwoThreeTree
{
public:

	template<typename U>
	struct TTNode
	{
		T _fst_val;
		T _sed_val;
		bool _is_full;
		TTNode<U> *_left;
		TTNode<U> *_mid;
		TTNode<U> *_right;
		TTNode<U> *_parent;

		TTNode(const T &val, TTNode<U> *left, TTNode<U> *mid, TTNode<U> *right, TTNode<U> *parent) :_fst_val(val), 
			_left(left), _mid(mid), _right(right),_parent(parent),_is_full(false) {}

		~TTNode()
		{
			std::cout << "deconstruction node: " << this->_fst_val << "\n";
			delete _left;
			delete _mid;
			delete _right;
		}

		bool full() { return _is_full; }
		void set_full(bool b) { _is_full = b; }
	};

	using Node = TTNode<T>;
public:
	TwoThreeTree():_root(nullptr)
	{

	}

	TwoThreeTree(const std::initializer_list<T> &args) :TwoThreeTree()
	{
		for (auto ptr = args.begin(); ptr != args.end(); ++ptr)
		{
			insert(*ptr);
		}
	}

	~TwoThreeTree()
	{
		delete _root;
	}

	/** 添加元素是唯一让2-3树高度增加的方式，需要考虑到节点分裂 */
	void insert(const T &val)
	{
		_insert(_root, val);
	}

	void inOrder()
	{
		_inOrder(_root);
		std::cout << std::endl;
	}

private:
	bool _insert(Node *ptr, const T &val)
	{
		if (!_root)
		{
			_root = new Node(val, nullptr, nullptr, nullptr, nullptr);
			return true;
		}

		if (ptr)
		{
			if (ptr->_fst_val > val)
			{
				if (!_insert(ptr->_left, val))
				{
					split_node(ptr, val);
					return true;
				}
			}	
			else if (!ptr->full())
			{
				if (!_insert(ptr->_right, val))
				{
					split_node(ptr, val);
					return true;
				}
			}
			else if (ptr->full())
			{
				if (ptr->_sed_val > val)
				{
					if (!_insert(ptr->_mid, val))
					{
						split_node(ptr, val);
						return true;
					}
				}
				else
				{
					if (!_insert(ptr->_right, val))
					{
						split_node(ptr, val);
						return true;
					}
				}
			}
		}
		else
		{
			/** 找到插入点 */
			return false;
		}
	}

	void split_node(Node *ptr, const T &val)
	{
		/** 递归分裂节点 */
		if (!ptr)
		{
			/** 回溯到根节点上 */
			Node *new_node = new Node(val, nullptr, nullptr, nullptr, nullptr);
			_splite(new_node, _root);
			_root = new_node;
			return;
		}

		if (!ptr->full())
		{
			int min_val = std::min(ptr->_fst_val, val);
			int max_val = std::max(ptr->_fst_val, val);
			ptr->_fst_val = min_val;
			ptr->_sed_val = max_val;
			ptr->set_full(true);
			
			/** 如果该节点具有子节点，那么会打破的2-3树的第一个特性：当前节点如果是满的，那么必须具有三个子节点 */
			if (!check_attribute_1(ptr))
			{
				/** 调整 */
				if (!checkout_attribute_2_1(ptr))
				{
					ptr->_mid = new Node(ptr->_left->_sed_val, nullptr, nullptr, nullptr, ptr);
					ptr->_left->set_full(false);
				}
				else if (!checkout_attribute_2_2(ptr))
				{
					ptr->_mid = new Node(ptr->_right->_fst_val, nullptr, nullptr, nullptr, ptr);
					ptr->_right->set_full(false);
				}
			}
			return;
		}

		int mid_val = val;
		if (val < ptr->_fst_val)
		{
			mid_val = ptr->_fst_val;
			ptr->_fst_val = val;
		}
		else if (val < ptr->_sed_val)
		{
			mid_val = val;
		}
		else
		{
			mid_val = ptr->_sed_val;
			ptr->_sed_val = val;
		}
		split_node(ptr->_parent, mid_val);
	}

	bool check_attribute_1(Node *ptr)
	{
		if (ptr->full() && ptr->_left && ptr->_right && !ptr->_mid)   //不满足特性
		{
			/** 调整 */
			return false;
		}
		else
			return true;
	}

	void _splite(Node *new_node, Node *old_node)
	{
		Node *left_node = new Node(old_node->_fst_val, nullptr, nullptr, nullptr, new_node);
		Node *right_node = new Node(old_node->_sed_val, nullptr, nullptr, nullptr, new_node);
		new_node->_left = left_node;
		new_node->_right = right_node;

		//right_node->set_full(false);

		/** 由于把左子节点分裂出去了，还需要递归继续处理该被分裂的左子节点的子节点 */
		if (!checkout_attribute_2_1(old_node))
		{
			right_node->_left = old_node->_mid;
			right_node->_right = old_node->_right;
			old_node->_mid->_parent = right_node;
			_splite(left_node, old_node->_left);		
		}
		else if(!checkout_attribute_2_2(old_node))
		{
			left_node->_right = old_node->_mid;
			left_node->_left = old_node->_left;
			old_node->_mid->_parent = left_node;
			_splite(right_node, old_node->_right);	
		}

		old_node->_left = old_node->_mid = old_node->_right = nullptr;
		delete old_node;
		
	}

	/** 检查是否满足排序特性 */
	bool checkout_attribute_2_1(Node *ptr)
	{
		return !(ptr->_left && ptr->_left->full() && ptr->_left->_sed_val > ptr->_fst_val);
	}

	bool checkout_attribute_2_2(Node *ptr)
	{
		return !(ptr->_right && ptr->_right->full() && ptr->_right->_fst_val < ptr->_sed_val);
	}

	void _inOrder(Node *ptr)
	{
		if (!ptr)
			return;

		_inOrder(ptr->_left);
		std::cout << ptr->_fst_val << " ";
		_inOrder(ptr->_mid);
		if(ptr->full())
			std::cout << ptr->_sed_val << " ";
		_inOrder(ptr->_right);
	}

public:
	Node *_root;
};


/******** main.cpp *********/
#include "tree.h"
int main()
{
	/** Test1 */
	/*AVLTree<int> avl{ 10,3,17,2,8,12,18,1,4,9,5};
	avl.insert(19);
	avl.inOrder();
	avl.erase(5);
	std::cout << std::endl;
	avl.inOrder();
	avl.erase(4);
	std::cout << std::endl;
	avl.inOrder();*/

	/** Test2 */
	TwoThreeTree<int> myTree{ 5,6,3,2,7,4 };
	typename TwoThreeTree<int>::Node *ptr = myTree._root;
	myTree.inOrder();
	
	getchar();
	return 1;
}


