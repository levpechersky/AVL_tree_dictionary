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

}



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

	/* AVL Rolls. */
	static Node* LL_roll(Node* r) {
		Node *unbalanced = r;
		r = r->left;
		unbalanced->left = r->right;
		r->right = unbalanced;
		r->parent = unbalanced->parent;
		unbalanced->parent = r;
		if(unbalanced->left)
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
		if(unbalanced->right)
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
		root = insert_r(k, v, root);
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
