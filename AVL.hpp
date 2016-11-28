/*
 * AVL.hpp
 *
 *  Created on: 2016-11-22
 *      Author: Lev Pechersky
 */
#ifndef AVL_HPP_
#define AVL_HPP_

/* Auxiliary functions, unrelated to AVL tree class.
 * Separate namespace to avoid redefinition. */
namespace aux {

static int abs(int x) {
	return x < 0 ? -x : x;
}

static int max(int x, int y) {
	return x < y ? y : x;
}

template<typename T>
static void swap(T& a, T& b) {
	T tmp(a);
	a = b;
	b = tmp;
}
}

/* AVL binary search tree.
 * Supports for ranged loops traversal. In-order used, i.e. items will be
 * sorted in ascending (according to key operator< definition) order.
 *
 * @Iterators and references invalidation:
 * All iterators are invalidated after each operation, that changes the tree.
 * All references are valid after insertion, and invalidated after deletion.
 *
 * @Requirements from Key: has operator< implemented, copy-constructible,
 *     assignable.
 * @Requirements from Value: Copy-constructible. Don't have to be default
 *     constructible.
 *
 * If not defined otherwise, n is number of nodes in tree,
 * and memory complexity is O(1)
 */
template<typename Key, typename Value>
class AVL {

	struct Node {
		Key key;
		Value* value;
		int height;
		Node *left, *right, *parent;
		Node(const Key& key, const Value& value) :
				key(key),
						value(nullptr),
						height(0),
						left(nullptr),
						right(nullptr),
						parent(nullptr) {
			this->value = new Value(value);
		}
		Node(const Node&) = delete;
		Node& operator=(const Node&) = delete;
		~Node() {
			delete value;
		}
	};

	Node *root;

	class inorderIterator {
		Node *node;

	public:
		inorderIterator(Node* node = nullptr) :
				node(node) {
		}

		/* !IMPORTANT! iterator must be validated before.
		 *     ++ on invalid iterators (e.g. end()) is undefined.
		 * @Return: iterator pointing to next (in-order) node
		 */
		inorderIterator& operator++() {
			node = next_inorder(node);
			return *this;
		}
		inorderIterator operator++(int) {
			inorderIterator copy(*this);
			++(*this);
			return copy;
		}

		/* Iterators are compared by adresses of nodes they point to in memory
		 */
		bool operator==(const inorderIterator& it) const {
			return this->node == it.node;
		}
		bool operator!=(const inorderIterator& it) const {
			return !(*this == it);
		}

		/* !IMPORTANT! iterator must be validated before dereferencing.
		 *     Dereferencing invalid iterators (e.g. end()) is undefined.
		 * @Return: copy of value (not the key) at node at iterator.
		 */
		Value& operator*() const {
			return *(node->value);
		}

		/* Return copy of key. */
		Key key() const {
			return node->key;
		}
		/* Return accessible reference to value */
		Value& value() const {
			return *(node->value);
		}
	};

	/*
	 * @Return: pointer to next node in-order
	 */
	static Node* next_inorder(Node* node) {
		if (node->right)
			return leftmost(node->right);
		while (!is_leftchild(node) && node->parent) {
			node = node->parent;
		}
		return node->parent;
	}

	/*
	 * Test of keys equality, doesn't require == operator
	 */
	static bool equal(const Key& k1, const Key& k2) {
		return !(k1 < k2 || k1 > k2);
	}

	/* Calculates actual height, based on subtrees of r.
	 * Differs from height property: subtrees of r and r itself may be empty.
	 */
	static int height(Node* r) {
		if (!r)
			return -1;
		return aux::max(r->right ? r->right->height : -1,
				r->left ? r->left->height : -1) + 1;
	}

	/* Undefined for empty nodes */
	static int balance(Node* r) {
		assert(r);
		return height(r->left) - height(r->right);
	}

	/* Undefined for empty nodes */
	static bool balance_is_invalid(Node* r) {
		assert(r);
		return aux::abs(balance(r)) > 1;
	}

	/*
	 * Recursive search in tree.
	 *
	 * @Return: nullptr if node with key k not present,
	 *    pointer to node otherwise.
	 */
	static Node* find_r(const Key& k, Node* r) {
		if (!r)
			return nullptr;
		if (equal(k, r->key)) {
			return r;
		} else if (k < r->key) {
			return find_r(k, r->left);
		} else {
			return find_r(k, r->right);
		}
	}

	/* Given some node returns it leftmost successor, or node itself,
	 * if it has no left child. Node r may be null.
	 * @Return: pointer to leftmost successor of r, or to r, if r has no left child.
	 * @Time complexity: O(h), where h is the height of r's subtree.
	 */
	static Node* leftmost(Node* r) {
		if (!r)
			return r; // TODO may be unnecessary
		while (r->left) {
			r = r->left;
		}
		return r;
	}

	static bool is_leaf(Node* r) {
		if (!r)
			return false;
		return !(r->left || r->right);
	}

	/* AVL Rolls. */
	static Node* LL_roll(Node* r) {
		Node *unbalanced = r;
		r = r->left;
		unbalanced->left = r->right;
		r->right = unbalanced;
		r->parent = unbalanced->parent;
		unbalanced->parent = r;
		if (unbalanced->left)
			unbalanced->left->parent = unbalanced;
		unbalanced->height = height(unbalanced);
		r->height = height(r);
		return r;
	}
	static Node* RR_roll(Node* r) {
		Node *unbalanced = r;
		r = r->right;
		unbalanced->right = r->left;
		r->left = unbalanced;
		r->parent = unbalanced->parent;
		unbalanced->parent = r;
		if (unbalanced->right)
			unbalanced->right->parent = unbalanced;
		unbalanced->height = height(unbalanced);
		r->height = height(r);
		return r;
	}
	static Node* RL_roll(Node* r) {
		r->right = LL_roll(r->right);
		return RR_roll(r);
	}
	static Node* LR_roll(Node* r) {
		r->left = RR_roll(r->left);
		return LL_roll(r);
	}

	/* Decides which type of roll to apply, if needed.
	 * If balance factor is valid (i.e. between -1 and 1),
	 * @Return: updated root of rebalanced sub-tree.
	 * @Time complexity: O(1)
	 *  */
	static Node* check_and_roll(Node* r) {
		if (balance(r) > 1) {
			if (balance(r->left) >= 0) {
				return LL_roll(r);
			} else {
				return LR_roll(r);
			}
		} else if (balance(r) < -1) {
			if (balance(r->right) <= 0) {
				return RR_roll(r);
			} else {
				return RL_roll(r);
			}
		} else {
			return r;
		}
	}

	/* Recursive insertion to tree, where tree is rebalanced after insertion.
	 * This function assumes, that tree doesn't contain an item with given key.
	 *
	 * @Return: pointer to updated (after rebalancing) root node
	 * @Memory complexity: O(h), where h is the height of r's subtree.
	 * */
	static Node* insert_r(const Key& k, const Value& v, Node* r) {
		if (!r)
			return new Node(k, v);
		if (k < r->key) {
			r->left = insert_r(k, v, r->left);
			r->left->parent = r;
		} else {
			r->right = insert_r(k, v, r->right);
			r->right->parent = r;
		}
		r->height = height(r);
		return check_and_roll(r);
	}

	/* Checks whether given node is the left child of it's parent node.
	 * If node has no parent (like root node) it isn't left child.
	 *
	 * @Return: false if r is null, root (root has no parent), or is right child.
	 *     true if r has a parent and is it's left child.
	 */
	static bool is_leftchild(Node *r) {
		if (!r || !r->parent)
			return false;
		return r->parent->left == r;
	}

	/* Assumes that tree does contain item with key k. */
	static Node* remove_r(const Key& k, Node *r) { // FIXME
		if (!r)
			return r;
		if (k < r->key) {
			r->left = remove_r(k, r->left);
		} else if (k > r->key) {
			r->right = remove_r(k, r->right);
		} else {
			// TODO complete this
			if (is_leaf(r)) {
				if (r->parent) {
					(is_leftchild(r) ? r->parent->left : r->parent->right) =
							nullptr; // TODO refactor
				}
				delete r;
				r = nullptr;
			} else if (!r->right || !r->left) { // 1 child
				Node *child = r->right ? r->right : r->left;
				child->parent = r->parent;
				if (r->parent) {
					(is_leftchild(r) ? r->parent->left : r->parent->right) =
							child;
				}
				delete r;
				r = child;
			} else { // 2 children
				Node* next = next_inorder(r);
				aux::swap(r->value, next->value);
				aux::swap(r->key, next->key);
				remove_r(k, next);
			}
		}
		return r; // TODO currently not AVL
	}

	static int size_r(Node *r) {
		if (!r)
			return 0;
		return 1 + size_r(r->left) + size_r(r->right);
	}

	static void destroy_r(Node *r) {
		if (!r)
			return;
		destroy_r(r->left);
		destroy_r(r->right);
		delete r;
	}

	/* Gets two sorted arrays, of keys and pointers to values
	 * (to allow values to be not default-constructible)
	 * Arrays must be sorted.
	 * Constructs AVL tree recursively.
	 */
	static Node *tree_from_array(Key* k_arr, Value** v_arr, int from, int to) {
		if (from > to)
			return nullptr;
		int mid = (from + to) / 2;
		Node *tmp_root = new Node(k_arr[mid], *(v_arr[mid]));
		tmp_root->left = tree_from_array(k_arr, v_arr, from, mid - 1);
		if (tmp_root->left)
			tmp_root->left->parent = tmp_root;
		tmp_root->right = tree_from_array(k_arr, v_arr, mid + 1, to);
		if (tmp_root->right)
			tmp_root->right->parent = tmp_root;
		return tmp_root;
	}

public:
	AVL() :
			root(nullptr) {
	}
	/* Alternative C'tor. Creates tree, which consists of a single leaf
	 * with given key and value */
	AVL(const Key& k, const Value& v) :
			root(nullptr) {
		root = new Node(k, v);
	}

	/*
	 * @Return: in-order iterator to smallest (by definition of Key's < operator) node.
	 *     If tree is empty - iterator to end()
	 * @Time complexity: O(log(n))
	 */
	inorderIterator begin() const {
		return inorderIterator(leftmost(root));
	}
	/*
	 * @Return: iterator to empty/non-existing node.
	 *     This iterator should be never dereferenced or incremented.
	 * @Time complexity: O(1)
	 */
	inorderIterator end() const {
		return inorderIterator();
	}

	/* @Return: true if tree is empty, i.e doesn't contain any nodes.
	 * @Time complexity: O(1)
	 */
	bool empty() const {
		return !root;
	}

	/*
	 *
	 * @Return: in-order iterator to element with key k, or iterator to end()
	 *     if item isn't present.
	 * @Time complexity: O(log(n))
	 * @Memory complexity: O(log(n))
	 */
	inorderIterator find(const Key& k) const {
		return inorderIterator(find_r(k, root));
	}

	/* Insert an item with given key and value.
	 * If item is already present - tree stays unchanged, and false returned.
	 *
	 * @Return: false if item with key is in dictionary.
	 * @Time complexity: O(log(n)), as find and insert_r both are O(log(n))
	 * @Memory complexity: O(log(n))
	 */
	bool insert(const Key& k, const Value& v) {
		if (find(k) != end())
			return false;
		root = insert_r(k, v, root);
		return true;
	}

	/*
	 *
	 * If element with Key k isn't present - does nothing.
	 * @Time complexity: O(log(n))
	 * @Memory complexity: O(log(n))
	 */
	void remove(const Key& k) {
		if (find(k) == end())
			return;
		root = remove_r(k, root);
	}

	/*
	 * @Return: number of nodes in tree
	 * @Time complexity: O(n)
	 * @Memory complexity: O(log(n))
	 */
	int size() const {
		return size_r(root);
	}

	/* Frees all nodes
	 *
	 * @Time complexity: O(n)
	 * @Memory complexity: O(log(n))
	 */
	void clear() {
		destroy_r(root);
		root = nullptr;
	}

	/* Efficient tree merge.
	 * Left operand will contain all unique nodes from both trees,
	 * right stays unchanged. All pointers, iterators and references of the
	 * left tree are invalidated.
	 *
	 * If there's same keys, data from left tree will be taken.
	 *
	 * @Time complexity: O(m+n), where m and n are numbers of nodes in current
	 *     and joining tree.
	 * @Memory complexity: O(m+n)
	 */
	// TODO check case with same keys
	void merge(const AVL<Key, Value>& t) {
		int m_size = size() + t.size(); //O(m + n)
		Key* keys = new Key[m_size];
		Value** values = new Value*[m_size];
		//output to arrays
		auto l = begin(), r = t.begin(), l_end = end(), r_end = t.end();
		int i = 0;
		while (l != l_end || r != r_end) { //O(m + n)
			if (l != l_end && r != r_end) {
				if (r.key() < l.key()) {
					keys[i] = r.key();
					values[i] = new Value(r.value());
					++r;
				} else {
					keys[i] = l.key();
					values[i] = new Value(l.value());
					++l;
				}
			} else if (l != l_end) {
				keys[i] = l.key();
				values[i] = new Value(l.value());
				++l;
			} else { // r != r_end
				keys[i] = r.key();
				values[i] = new Value(r.value()); //TODO refactor
				++r;
			}
			++i;
		}
		m_size = i; //It may happen, that i<m+n because of repeating keys
		Node * tmp_root = tree_from_array(keys, values, 0, m_size - 1);
		clear();
		root = tmp_root;
		delete[] values;
		delete[] keys;
	}

	~AVL() {
		clear();
	}

};

#endif /* AVL_HPP_ */
