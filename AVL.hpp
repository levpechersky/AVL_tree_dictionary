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

//static int abs(int x) {
//	return x < 0 ? -x : x;
//}

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
 * All iterators, references and pointers are invalidated after
 *     merging (for the left operand only).
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
		friend class AVL<Key, Value> ;
		Node *node;
		inorderIterator(Node* node = nullptr) :
				node(node) {
		}

	public:

		/* !IMPORTANT! iterator must be validated before.
		 *     ++ on invalid iterators (e.g. end()) is undefined.
		 * @Return: iterator pointing to next (in-order) node
		 * @Time complexity: O(log(n)) in worst case, but full traversal
		 *     takes O(n) time.
		 */
		inorderIterator& operator++() {
			node = next_inorder(node);
			return *this;
		}
		/* Postfix version. */
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
	 * @Time complexity: O(1)
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

//	/* Undefined for empty nodes */
//	static bool balance_is_invalid(Node* r) {
//		assert(r);
//		return aux::abs(balance(r)) > 1;
//	}

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
			return r;
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
	static Node* remove_r(const Key& k, Node *r) {
		if (!r)
			return r;
		if (k < r->key) {
			r->left = remove_r(k, r->left);
		} else if (k > r->key) {
			r->right = remove_r(k, r->right);
		} else {
			if (is_leaf(r)) { // no children
				set_child_of_parent(r, nullptr);
				delete r;
				r = nullptr;
			} else if (!r->right || !r->left) { // 1 child
				Node *child = r->right ? r->right : r->left;
				child->parent = r->parent;
				set_child_of_parent(r, child);
				delete r;
				r = child;
			} else { // 2 children
				Node* next = next_inorder(r);
				aux::swap(r->value, next->value);
				aux::swap(r->key, next->key);
				remove_r(k, next);
			}
		}
		if (!r)
			return r;
		r->height = height(r);
		return check_and_roll(r);
	}

	static int size_r(Node *r) {
		if (!r)
			return 0;
		return 1 + size_r(r->left) + size_r(r->right);
	}

	/* Frees all nodes recursively */
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
		tmp_root->right = tree_from_array(k_arr, v_arr, mid + 1, to);
		set_parent_of_children(tmp_root);
		return tmp_root;
	}

	/* Changes child node of parent of given old_child node.
	 * For nodes with no parent (i.e. root) does nothing.
	 * Assumes old_child isn't null.
	 * Used when swapping nodes in tree (e.g. rolls)
	 */
	static void set_child_of_parent(Node* old_child, Node* new_child) {
		assert(old_child);
		if (old_child->parent) {
			if (is_leftchild(old_child)) {
				old_child->parent->left = new_child;
			} else {
				old_child->parent->right = new_child;
			}
		}
	}

	/* Changes parent pointer of given node children (if any) to point to the
	 * given node. Used when swapping nodes in tree (e.g. rolls)
	 * Assumes parent isn't null.
	 */
	static void set_parent_of_children(Node* parent) {
		assert(parent);
		if (parent->left)
			parent->left->parent = parent;
		if (parent->right)
			parent->right->parent = parent;
	}

	/* Helper function for trees merging.
	 * Gets 2 trees (this and t) and fills two given and !allocated! arrays
	 * with keys and values of both trees in ascending order of keys.
	 *
	 * After merging, output arrays keys and values will only contain unique
	 * keys. If there are same keys, value from the left tree will be taken.
	 *
	 *
	 * @Return: number of unique keys in merged array.
	 * @Time complexity: O(m+n), where m and n are numbers of nodes in current
	 *     and joining tree.
	 * @Memory complexity: O(m+n)
	 * */
	int trees_to_arrays(const AVL<Key, Value>& t, Key* keys, Value** values) {
		auto l = begin(), r = t.begin(), l_end = end(), r_end = t.end();
		int i = 0;
		while (l != l_end || r != r_end) {
			inorderIterator current;
			if (l != l_end && r != r_end) {
				if (r.key() < l.key()) {
					current = r++;
				} else {
					current = l++;
				}
			} else
			if (l != l_end) {
				current = l++;
			} else { // r != r_end
				current = r++; // TODO test uncovered
			}
			keys[i] = current.key();
			values[i] = new Value(current.value());
			++i;
		}
		return i;
	}

public:
	/* Default C'tor. Creates empty tree.
	 * @Time complexity: O(1)
	 */
	AVL() :
			root(nullptr) {
	}
	/* Alternative C'tor. Creates tree, which consists of a single leaf
	 * with given key and value
	 * @Time complexity: O(1)
	 */
	AVL(const Key& k, const Value& v) :
			root(nullptr) {
		root = new Node(k, v); // TODO test uncovered
	}

	/* Copy C'tor.
	 * Resulting tree will not be exact copy of original tree.
	 * It will contain all nodes, but tree structure may differ.
	 * @Time complexity: O(m), where m is number of nodes in tree t.
	 * @Memory complexity: O(m)
	 * */
	AVL(const AVL& t) :
			root(nullptr) {
		merge(t); // TODO test uncovered
	}

	/* Assignment operator.
	 * Resulting tree will not be exact copy of original tree.
	 * It will contain all nodes, but tree structure may differ.
	 * @Time complexity: O(n + m), where m is number of nodes in tree t.
	 * @Memory complexity: O(n + m)
	 */
	AVL& operator=(const AVL& t) {
		if (this != &t) {
			clear();
			merge(t);
		}
		return *this;
	}

	/*
	 * @Return: in-order iterator to smallest (by definition of Key's
	 *     < operator) node. If tree is empty - iterator to end()
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
	 * Left tree will contain all unique nodes from both trees,
	 * right stays unchanged. All pointers, iterators and references of the
	 * left tree are invalidated.
	 *
	 * If there's same keys, data from left tree will be taken.
	 *
	 * @Time complexity: O(m+n), where m and n are numbers of nodes in current
	 *     and joining tree.
	 * @Memory complexity: O(m+n)
	 */
	void merge(const AVL<Key, Value>& t) {
		int merged_size = size() + t.size(); // m + n
		Key* keys = new Key[merged_size];
		Value** values = new Value*[merged_size];
		merged_size = trees_to_arrays(t, keys, values);
		Node * tmp_root = tree_from_array(keys, values, 0, merged_size - 1);
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
