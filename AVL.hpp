/*
 * AVL.hpp
 *
 *  Created on: 2016-11-22
 *      Author: Lev Pechersky
 */
#ifndef AVL_HPP_
#define AVL_HPP_

/* Auxiliary functions, unrelated to AVL tree class.
 * Separate namespace to avoid names collision. */
namespace aux {
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
 * Supports 'for' ranged loops traversal. In-order used, i.e. items will be
 * sorted in ascending (according to key operator< definition) order.
 *
 * @Iterators and references invalidation:
 * All iterators are invalidated after each operation, that changes the tree.
 * All references are valid after insertion, and invalidated after deletion.
 * All iterators, references and pointers are invalidated after
 *     merging (for the left operand only).
 *
 * @Requirements from Key: has operator< implemented, copy-constructible,
 *     assignable, default-constructible.
 * @Requirements from Value: Copy-constructible. Don't have to be default
 *     constructible.
 *
 * For each function, if not defined otherwise, n is number of nodes in tree,
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
				value(NULL),
				height(0),
				left(NULL),
				right(NULL),
				parent(NULL) {
			this->value = new Value(value);
		}
//		Node(const Node&) = delete;
//		Node& operator=(const Node&) = delete;
		~Node() {
			delete value;
		}
	};

	Node *root;

	class inorderIterator {
		friend class AVL<Key, Value> ;
		Node *node;
		inorderIterator(Node* node = NULL) :	node(node) {}

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

		/* Iterators are compared by adresses of nodes they point to in memory.
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
		/* Returns accessible reference to value. Same as operator*,
		 * added for consistency with key() function */
		Value& value() const {
			return *(node->value);
		}
	};

	/* Test of keys equality, doesn't require == operator.
	 */
	static bool equal(const Key& k1, const Key& k2) {
		return !(k1 < k2 || k2 < k1);
	}

	/* Calculates actual height, based on subtrees of r.
	 * Differs from height property: subtrees of r and r itself may be empty.
	 *
	 * @Return: height of r
	 * @Time complexity: O(1)
	 */
	static int height(Node* r) {
		if (!r)
			return -1;
		return aux::max(r->right ? r->right->height : -1,
				r->left ? r->left->height : -1) + 1;
	}

	/* Balance fator in AVL trees is defined as following:
	 * height(left_child) - height(right_child)
	 * Assumes node isn't null.
	 *
	 * @Return: balance factor of r, as described.
	 * @Time complexity: O(1)
	 */
	static int balance(Node* r) {
		assert(r);
		return height(r->left) - height(r->right);
	}

	/* Node is leaf if it has no children.
	 *
	 * @Return: true if node is leaf
	 * @Time complexity: O(1)
	 */
	static bool is_leaf(Node* r) {
		if (!r)
			return false;
		return !(r->left || r->right);
	}

	/* Checks whether given node is the left child of it's parent node.
	 * If node has no parent (like root node) it isn't left child.
	 *
	 * @Return: false if r is null, root (root has no parent), or is right child.
	 *     true if r has a parent and is it's left child.
	 * @Time complexity: O(1)
	 */
	static bool is_leftchild(Node *r) {
		if (!r || !r->parent)
			return false;
		return r->parent->left == r;
	}

	/* Given some node returns it leftmost successor, or node itself,
	 * if it has no left child. Node r may be null.
	 *
	 * @Return: pointer to leftmost successor of r, or to r, if r has no left
	 *     child.
	 * @Time complexity: O(log(n))
	 */
	static Node* leftmost(Node* r) {
		if (!r)
			return r;
		while (r->left) {
			r = r->left;
		}
		return r;
	}

	/* Tree in-order traversal. Given node returns pointer to next one in-order.
	 * Assumes node isn't null.
	 *
	 * @Return: pointer to next node in-order
	 * @Time complexity: O(log(n)) in worst case, but full traversal
	 *     takes O(n) time.
	 */
	static Node* next_inorder(Node* node) {
		assert(node);
		if (node->right)
			return leftmost(node->right);
		while (!is_leftchild(node) && node->parent) {
			node = node->parent;
		}
		return node->parent;
	}

	/* Decides which type of roll to apply, if needed.
	 * If balance factor is valid (i.e. between -1 and 1), changes nothing.
	 *
	 * @Return: updated root of rebalanced sub-tree or given r if balance factor
	 *     is valid.
	 * @Time complexity: O(1)
	 */
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

	/* Changes child node of parent of given old_child node.
	 * For nodes with no parent (i.e. root) does nothing.
	 * Assumes old_child isn't null.
	 * Used when swapping nodes in tree (e.g. rolls).
	 * @Time complexity: O(1)
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
	 * given node. Used when swapping nodes in tree (e.g. rolls).
	 * Assumes parent isn't null.
	 * @Time complexity: O(1)
	 */
	static void set_parent_of_children(Node* parent) {
		assert(parent);
		if (parent->left)
			parent->left->parent = parent;
		if (parent->right)
			parent->right->parent = parent;
	}

	/* AVL Rolls.
	 *
	 * @Return: pointer to updated (after rebalancing) root node
	 * @Time complexity: O(1)
	 */
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

	/*
	 * Recursive search in tree.
	 *
	 * @Return: NULL if node with key k not present,
	 *    pointer to node otherwise.
	 * @Time complexity: O(log(n))
	 * @Memory complexity: O(log(n))
	 */
	static Node* find_r(const Key& k, Node* r) {
		if (!r)
			return NULL;
		if (equal(k, r->key)) {
			return r;
		} else if (k < r->key) {
			return find_r(k, r->left);
		} else {
			return find_r(k, r->right);
		}
	}

	/* Recursive insertion to tree, where tree is rebalanced after insertion.
	 * This function assumes, that tree doesn't contain an item with given key.
	 *
	 * @Return: pointer to updated (after rebalancing) root node
	 * @Time complexity: O(log(n))
	 * @Memory complexity: O(log(n))
	 */
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

	/* Recursively removes nodes from tree and rebalances it.
	 * Root node of the tree may change due to deletion.
	 * Assumes that tree does contain item with key k.
	 *
	 * @Return: updated root of the tree after removing node with key k.
	 * @Time complexity: O(log(n))
	 * @Memory complexity: O(log(n))
	 */
	static Node* remove_r(const Key& k, Node *r) {
		if (!r)
			return r;
		if (k < r->key) {
			r->left = remove_r(k, r->left);
		} else if (r->key < k) {
			r->right = remove_r(k, r->right);
		} else {
			if (is_leaf(r)) { // no children
				set_child_of_parent(r, NULL);
				delete r;
				r = NULL;
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

	/* Given root node of (sub)tree returns number of nodes of that tree.
	 *
	 * @Return: number of nodes in the subtree of r
	 * @Time complexity: O(log(n))
	 * @Memory complexity: O(log(n))
	 */
	static int size_r(Node *r) {
		if (!r)
			return 0;
		return 1 + size_r(r->left) + size_r(r->right);
	}

	/* Frees all nodes recursively
	 *
	 * @Time complexity: O(log(n))
	 * @Memory complexity: O(log(n))
	 */
	static void destroy_r(Node *r) {
		if (!r)
			return;
		destroy_r(r->left);
		destroy_r(r->right);
		delete r;
	}

	/* Gets two sorted arrays, of keys and pointers to values
	 * (to allow values to be not default-constructible)
	 * Array of keys must be sorted.
	 * Constructs AVL tree recursively.
	 *
	 * @Return: root of the new tree.
	 * @Time complexity: O(p), where p is size of array, i.e. initial (to-from).
	 * @Memory complexity: O(log(p))
	 */
	static Node *tree_from_array(Key* k_arr, Value** v_arr, int from, int to) {
		if (from > to)
			return NULL;
		int mid = (from + to) / 2;
		Node *tmp_root = new Node(k_arr[mid], *(v_arr[mid]));
		delete v_arr[mid];
		tmp_root->left = tree_from_array(k_arr, v_arr, from, mid - 1);
		tmp_root->right = tree_from_array(k_arr, v_arr, mid + 1, to);
		set_parent_of_children(tmp_root);
		tmp_root->height = height(tmp_root);
		return tmp_root;
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
		inorderIterator l = begin(), r = t.begin(), l_end = end(), r_end = t.end();
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
				current = r++;
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
	AVL() :	root(NULL) {}
	/* Alternative C'tor. Creates tree, which consists of a single leaf
	 * with given key and value
	 * @Time complexity: O(1)
	 */
	AVL(const Key& k, const Value& v) :	root(NULL) {
		root = new Node(k, v);
	}

	/* Copy C'tor.
	 * Resulting tree will not be exact copy of original tree.
	 * It will contain all nodes, but tree structure may differ.
	 *
	 * @Time complexity: O(m), where m is number of nodes in tree t.
	 * @Memory complexity: O(m)
	 * */
	AVL(const AVL& t) :	root(NULL) {
		merge(t);
	}

	/* Assignment operator.
	 * Resulting tree will not be exact copy of original tree.
	 * It will contain all nodes, but tree structure may differ.
	 *
	 * @Return: *this
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

	/* Returns an in-order iterator to the first element of the container.
	 *
	 * @Return: in-order iterator to smallest (by definition of Key's
	 *     < operator) node. If tree is empty - iterator to end()
	 * @Time complexity: O(log(n))
	 */
	inorderIterator begin() const {
		return inorderIterator(leftmost(root));
	}

	/* Returns an iterator to the element following the last (i.e largest)
	 * element of the tree. This element acts as a placeholder, attempting to
	 * access it results in undefined behavior.
	 *
	 * @Return: iterator to empty/non-existing node.
	 *     This iterator should be never dereferenced or incremented.
	 * @Time complexity: O(1)
	 */
	inorderIterator end() const {
		return inorderIterator();
	}

	/* Checks whether the tree is empty, i.e. contains no nodes.
	 *
	 * @Return: true if tree is empty, i.e doesn't contain any nodes.
	 * @Time complexity: O(1)
	 */
	bool empty() const {
		return !root;
	}

	/* Searches the tree for item with key k.
	 *
	 * @Return: in-order iterator to element with key k, or iterator to end()
	 *     if item isn't present.
	 * @Time complexity: O(log(n))
	 * @Memory complexity: O(log(n))
	 */
	inorderIterator find(const Key& k) const {
		return inorderIterator(find_r(k, root));
	}

	/* Inserts an item with given key k and value v.
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

	/* Removes an element with key k from the tree.
	 * If element with Key k isn't present - does nothing.
	 *
	 * @Time complexity: O(log(n))
	 * @Memory complexity: O(log(n))
	 */
	void remove(const Key& k) {
		if (find(k) == end())
			return;
		root = remove_r(k, root);
	}

	/* Counts number of nodes in tree.
	 *
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
		root = NULL;
	}

	/* Efficient tree merge.
	 * Trees' nodes are copied to sorted temporary array and then merged tree
	 * is built, as if merged array was in-order of existing tree.
	 * Left tree will contain all unique nodes from both trees, right tree
	 * stays unchanged.
	 * If there's same keys, data from left tree will be taken.
	 *
	 * All pointers, iterators and references of the left tree are invalidated.
	 *
	 * @Time complexity: O(m+n), where m and n are numbers of nodes in current
	 *     and joining tree.
	 * @Memory complexity: O(m+n)
	 */
	void merge(const AVL<Key, Value>& t) {
		int merged_size = size() + t.size();
		Key* keys = new Key[merged_size];
		Value** values;
		try {
			values = new Value*[merged_size];
		} catch (std::bad_alloc&) {
			delete[] keys;
			throw;
		}
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
