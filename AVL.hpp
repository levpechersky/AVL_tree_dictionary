/*
 * AVL.hpp
 *
 *  Created on: 2016-11-22
 *      Author: Lev Pechersky
 */
#ifndef AVL_HPP_
#define AVL_HPP_

/* AVL binary search tree
 *
 * Requirements from Key: has operator < implemented
 * Requirements from Value: Copy-constructible.
 *     Don't have to be default constructible
 *
 * If not defined otherwise, n is number of nodes in tree,
 * and memory complexity is O(1)
 */
template<typename Key, typename Value>
class AVL {

	struct Node {
		Key key;
		Value* value;
		int r_h, l_h;
		Node *left, *right, *parent;
		Node(const Key& key, const Value& value) :
				key(key),
						value(nullptr),
						r_h(0), l_h(0),
						left(nullptr),
						right(nullptr),
						parent(nullptr) {
			this->value = new Value(value);
		}
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
		 * @Returns iterator next (in-order) node
		 */
		inorderIterator& operator++() {
			if (node->right) {
				node = leftmost(node->right);
			} else {
				while (!is_leftchild(node) && node->parent) {
					node = node->parent;
				}
				node = node->parent;
			}
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
		 * @Return: copy of value at node at iterator.
		 */
		Value operator*() const {
			return *(node->value);
		}
	};

	/*
	 * Test of keys equality, doesn't require == operator
	 */
	static bool equal(const Key& k1, const Key& k2) {
		return !(k1 < k2 || k1 > k2);
	}

	/* Undefined for empty nodes */
	static int height(Node* r) { // TODO maybe extract max
		assert(r);
		return ((r->l_h > r->r_h) ? r->l_h : r->r_h) + 1;
	}

	/* Undefined for empty nodes */
	static int balance(Node* r){
		assert(r);
		return r->r_h - r->l_h;
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

	/* Given some node returns it leftmost successor */
	static Node* leftmost(Node* r) {
		if (!r)
			return r; // TODO may be unnecessary
		while (r->left) {
			r = r->left;
		}
		return r;
	}

	/* Returns last (i.e. closest by key) node,
	 * where new node can be attached.
	 */
	static Node* last_before_insert(const Key& k, Node* r) {
		Node* last = nullptr;
		while (r) {
			last = r;
			if (k < r->key) {
				r = r->left;
			} else {
				r = r->right;
			}
		}
		return last;
	}

	static Node* insert_r(const Key& k, const Value& v, Node* r) {
		Node* last = last_before_insert(k, r);
		r = new Node(k, v);
		r->parent = last;
		if (k < last->key) {
			last->left = r;
		} else {
			last->right = r;
		}
		return r; //TODO make it AVL
	}

	/* Checks whether given node is the left child of it's parent node.
	 * If node has no parent (like root node) it isn't left child.
	 *
	 */
	static bool is_leftchild(Node *r) {
		if (!r || !r->parent)
			return false;
		return r->parent->left == r;
	}

public:
	AVL() :
			root(nullptr) {
	}
	/* Alternative C'tor. Creates tree, which consists of a single leaf */
	AVL(const Key& k, const Value& v) :
			root(nullptr) {
		root = new Node(k, v);
	}

	/*
	 *
	 * @Return: in-order iterator to smallest (by definition of Key's < operator) node.
	 *     If tree is empty - iterator to end()
	 * @Time complexity: O(log(n))
	 */
	inorderIterator begin() const {
		return inorderIterator(leftmost(root));
	}

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
	 */
	bool insert(const Key& k, const Value& v) {
		if (find(k) != end())
			return false;
		empty() ? root = new Node(k, v) : insert_r(k, v, root);
		return true;
	}

	/*
	 *
	 * If element with Key k isn't present - does nothing.
	 * @Time complexity: O(log(n))
	 */
	void remove(const Key& k) {
		if (find(k) == end())
			return;
		// TODO complete this
	}

};

#endif /* AVL_HPP_ */
