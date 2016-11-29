/*
 * AVL_test.cpp
 *
 *  Created on: 2016-11-22
 *      Author: Lev Pechersky
 */
#include <vector>
#include <algorithm>
#include <gtest\gtest.h>
#include "AVL.hpp"

//static char convert(int n) {
//	return n % 256 + 'a';
//}
//static std::vector<char> convert(std::vector<int> v) {
//	std::vector<char> c;
//	for (auto n : v) {
//		c.push_back(convert(n));
//	}
//	return c;
//}

class Test_key {
public:
	int x;
	Test_key() :
			x(-1) {
	}
	Test_key(const Test_key& i) :
			x(i.x) {
	}
	Test_key(int x) :
			x(x) {
	}
	Test_key& operator=(const Test_key& k) {
		x = k.x;
		return *this;
	}
};
bool operator<(const Test_key& a, const Test_key& b) {
	return a.x < b.x;
}
bool operator==(const Test_key& a, const Test_key& b) {
	return a.x == b.x;
}

class Test_value {
public:
	int x;
	Test_value(int x) :
			x(x) {
	}
	Test_value(const Test_value& i) :
			x(i.x) {
	}
	Test_value(const Test_key& k) :
			x(k.x) {
	}
	Test_value() = delete;
	Test_value& operator=(const Test_value&) = delete;
};

static Test_value convert(const Test_key& k) {
	return Test_value(k);
}
static std::vector<Test_value> convert(const std::vector<Test_key>& k) {
	std::vector<Test_value> c;
	for (auto n : k) {
		c.push_back(n);
	}
	return c;
}

typedef Test_key key_t;
typedef Test_value value_t;

TEST(AVL_Tree, one_leaf_tree_constructor) {
	const key_t k = 3;
	const value_t v = convert(k);
	AVL<key_t, value_t> tree(k, v);
	ASSERT_FALSE(tree.insert(k, v));
}

TEST(AVL_Tree, copy_constructing) {
	std::vector<key_t> k1 = { 2, 16, 40, 31, 7, 32, 11, 17 };
	std::vector<value_t> v1 = convert(k1);
	AVL<key_t, value_t> tree1;
	for (unsigned int i = 0; i < k1.size(); ++i) {
		tree1.insert(k1[i], v1[i]);
	}
	AVL<key_t, value_t> tree2(tree1);
	for (auto i : k1) {
		ASSERT_NE(tree2.find(i), tree2.end());
	}
}

TEST(AVL_Tree, assignment) {
	std::vector<key_t> k1 = { 2, 16, 40, 31, 7, 32, 11, 17 };
	std::vector<value_t> v1 = convert(k1);
	std::vector<key_t> k2 = { 10, 5, 18, 15, 22, 25 };
	std::vector<value_t> v2 = convert(k2);
	AVL<key_t, value_t> tree1;
	for (unsigned int i = 0; i < k1.size(); ++i) {
		tree1.insert(k1[i], v1[i]);
	}
	AVL<key_t, value_t> tree2;
	for (unsigned int i = 0; i < k2.size(); ++i) {
		tree2.insert(k2[i], v2[i]);
	}
	tree2 = tree1;
	for (auto i : k1) {
		ASSERT_NE(tree2.find(i), tree2.end());
	}
	for (auto i : k2) {
		ASSERT_EQ(tree2.find(i), tree2.end());
	}
}

TEST(AVL_Tree, begin_equals_end_empty_tree) {
	AVL<key_t, value_t> tree;
	ASSERT_EQ(tree.begin(), tree.end());
}

TEST(AVL_Tree, insert_first_success) {
	const key_t k = 3;
	const value_t v = convert(k);
	AVL<key_t, value_t> tree;
	ASSERT_TRUE(tree.insert(k, v));
}

TEST(AVL_Tree, insert_first_already_exist) {
	const key_t k = 3;
	const value_t v = convert(k);
	AVL<key_t, value_t> tree;
	tree.insert(k, v);
	ASSERT_FALSE(tree.insert(k, v));
}

TEST(AVL_Tree, insert_few_success_no_rolls) {
	std::vector<key_t> k = { 3, 10, 1 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_TRUE(tree.insert(k[i], v[i]));
	}
	ASSERT_NE(tree.end(), tree.find(k[0]));
	ASSERT_NE(tree.end(), tree.find(k[1]));
	ASSERT_NE(tree.end(), tree.find(k[2]));
}

TEST(AVL_Tree, find_fail_empty_tree) {
	const key_t k = 3;
	AVL<key_t, value_t> tree;
	ASSERT_EQ(tree.end(), tree.find(k));
}

TEST(AVL_Tree, find_root_success) {
	const key_t k = 3;
	const value_t v = convert(k);
	AVL<key_t, value_t> tree;
	tree.insert(k, v);
	ASSERT_NE(tree.end(), tree.find(k));
	ASSERT_FALSE(tree.insert(k, v));
}

TEST(AVL_Tree, iterator_preincrement) {
	std::vector<key_t> k = { 3, 10, 1 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto j = tree.begin();
	ASSERT_EQ(j.key(), key_t(1));
	++j;
	ASSERT_EQ(j.key(), key_t(3));
	++j;
	ASSERT_EQ(j.key(), key_t(10));
	++j;
	ASSERT_EQ(j, tree.end());
}

TEST(AVL_Tree, iterator_postincrement) {
	std::vector<key_t> k = { 3, 10, 1 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto j = tree.begin();
	ASSERT_EQ(j.key(), key_t(1));
	++j;
	ASSERT_EQ(j.key(), key_t(3));
	++j;
	ASSERT_EQ(j.key(), key_t(10));
	++j;
	ASSERT_EQ(j, tree.end());
}

TEST(AVL_Tree, insert_ll_roll_3) {
	std::vector<key_t> k = { 10, 5, 3 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(k.begin(), k.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(it.key(), k[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_rr_roll_3) {
	std::vector<key_t> k = { 3, 5, 10 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(k.begin(), k.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(it.key(), k[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_rl_roll_3) {
	std::vector<key_t> k = { 10, 15, 11 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(k.begin(), k.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(it.key(), k[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_lr_roll_3) {
	std::vector<key_t> k = { 10, 2, 5 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(k.begin(), k.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(it.key(), k[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_ll_roll_more) {
	std::vector<key_t> k = { 13, 10, 15, 16, 11, 5, 8, 4, 3 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(k.begin(), k.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(it.key(), k[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_rr_roll_more) {
	std::vector<key_t> k = { 10, 5, 18, 15, 22, 25 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(k.begin(), k.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(it.key(), k[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_rl_roll_more) {
	std::vector<key_t> k = { 13, 10, 15, 16, 11, 5, 4, 6, 7 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(k.begin(), k.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(it.key(), k[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_lr_roll_more) {
	std::vector<key_t> k = { 5, 2, 7, 6, 9, 1, 4, 3, 16, 15 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(k.begin(), k.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(it.key(), k[i]);
		++it;
	}
}

TEST(AVL_Tree, delete_root_tree_of_only_root) {
	const key_t k = 3;
	const value_t v = convert(k);
	AVL<key_t, value_t> tree;
	tree.insert(k, v);
	ASSERT_NO_FATAL_FAILURE(tree.remove(k));
	ASSERT_TRUE(tree.empty());
}

TEST(AVL_Tree, delete_unexisting_node_tree_unchanged) {
	std::vector<key_t> k = { 2, 16, 40, 31, 7, 5, 32, 11, 17 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree.remove(key_t(-1)));
	for (auto i : k) {
		ASSERT_NE(tree.find(i), tree.end());
	}
}

TEST(AVL_Tree, delete_no_roll_leaf) {
	std::vector<key_t> k = { 3, 10, 1 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree.remove(key_t(1)));
	ASSERT_NO_FATAL_FAILURE(tree.remove(key_t(10)));
	ASSERT_EQ(tree.find(key_t(1)), tree.end());
	ASSERT_EQ(tree.find(key_t(10)), tree.end());
	ASSERT_NE(tree.find(key_t(3)), tree.end());
}

TEST(AVL_Tree, delete_no_roll_root_when_root_has_1_leaf) {
	std::vector<key_t> k = { 3, 10 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree.remove(key_t(3)));
	auto new_root = tree.begin();
	ASSERT_EQ(new_root, tree.find(key_t(10)));
}

TEST(AVL_Tree, delete_no_roll_root_when_root_has_2_leaves) {
	std::vector<key_t> k = { 3, 10, 1 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree.remove(key_t(3)));
	ASSERT_NE(tree.find(key_t(1)), tree.end());
	ASSERT_NE(tree.find(key_t(10)), tree.end());
	ASSERT_EQ(tree.find(key_t(3)), tree.end());
}

TEST(AVL_Tree, delete_roll_RL) {
	std::vector<key_t> k = { 20, 10, 30, 25 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	const key_t to_delete = 10;
	ASSERT_NO_FATAL_FAILURE(tree.remove(to_delete));
	for (auto x : k) {
		if (x < to_delete || to_delete < x) {
			ASSERT_NE(tree.find(x), tree.end());
		} else {
			ASSERT_EQ(tree.find(x), tree.end());
		}
	}
}

TEST(AVL_Tree, delete_roll_RR) {
	std::vector<key_t> k = { 20, 10, 30, 35 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	const key_t to_delete = 10;
	ASSERT_NO_FATAL_FAILURE(tree.remove(to_delete));
	for (auto x : k) {
		if (x < to_delete || to_delete < x) {
			ASSERT_NE(tree.find(x), tree.end());
		} else {
			ASSERT_EQ(tree.find(x), tree.end());
		}
	}
}
TEST(AVL_Tree, delete_roll_LR) {
	std::vector<key_t> k = { 20, 10, 30, 15 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	const key_t to_delete = 30;
	ASSERT_NO_FATAL_FAILURE(tree.remove(to_delete));
	for (auto x : k) {
		if (x < to_delete || to_delete < x) {
			ASSERT_NE(tree.find(x), tree.end());
		} else {
			ASSERT_EQ(tree.find(x), tree.end());
		}
	}
}
TEST(AVL_Tree, delete_roll_LL) {
	std::vector<key_t> k = { 20, 10, 30, 5 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	const key_t to_delete = 30;
	ASSERT_NO_FATAL_FAILURE(tree.remove(to_delete));
	for (auto x : k) {
		if (x < to_delete || to_delete < x) {
			ASSERT_NE(tree.find(x), tree.end());
		} else {
			ASSERT_EQ(tree.find(x), tree.end());
		}
	}
}

TEST(AVL_Tree, tree_of_trees_creation) {
	AVL<key_t, AVL<key_t, value_t>> tree;
	AVL<key_t, value_t> inner_tree;
	ASSERT_TRUE(tree.insert(key_t(100), inner_tree));
	ASSERT_TRUE((*tree.find(key_t(100))).insert(key_t(1), value_t(1)));
}

TEST(AVL_Tree, merge_left_empty) {
	std::vector<key_t> k = { 2, 16, 40, 31, 7, 5, 32, 11, 17 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree1, tree2;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree1.insert(k[i], v[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree1.merge(tree2));
	std::vector<key_t> merged;
	merged.insert(merged.end(), k.begin(), k.end());
	for (auto k : merged) {
		ASSERT_NE(tree1.find(k), tree1.end());
	}
}

TEST(AVL_Tree, merge_right_empty) {
	std::vector<key_t> k = { 2, 16, 40, 31, 7, 5, 32, 11, 17 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree1, tree2;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree2.insert(k[i], v[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree1.merge(tree2));
	std::vector<key_t> merged;
	merged.insert(merged.end(), k.begin(), k.end());
	for (auto k : merged) {
		ASSERT_NE(tree1.find(k), tree1.end());
	}
}

TEST(AVL_Tree, merge_all_keys_unique) {
	std::vector<key_t> k1 = { 2, 16, 40, 31, 7, 32, 11, 17 };
	std::vector<value_t> v1 = convert(k1);
	std::vector<key_t> k2 = { 10, 5, 18, 15, 22, 25 };
	std::vector<value_t> v2 = convert(k2);
	AVL<key_t, value_t> tree1, tree2;
	for (unsigned int i = 0; i < k1.size(); ++i) {
		tree1.insert(k1[i], v1[i]);
	}
	for (unsigned int i = 0; i < k2.size(); ++i) {
		tree2.insert(k2[i], v2[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree1.merge(tree2));
	std::vector<key_t> merged;
	merged.insert(merged.end(), k1.begin(), k1.end());
	merged.insert(merged.end(), k2.begin(), k2.end());
	for (auto k : merged) {
		ASSERT_NE(tree1.find(k), tree1.end());
	}
}

TEST(AVL_Tree, merge_some_keys_same) {
	std::vector<key_t> k1 = { 2, 16, 32, 11, 17 };
	std::vector<value_t> v1 = convert(k1);
	std::vector<key_t> k2 = { 10, 5, 11, 18, 15, 22, 17, 25 };
	std::vector<value_t> v2 = convert(k2);
	AVL<key_t, value_t> tree1, tree2;
	for (unsigned int i = 0; i < k1.size(); ++i) {
		tree1.insert(k1[i], v1[i]);
	}
	for (unsigned int i = 0; i < k2.size(); ++i) {
		tree2.insert(k2[i], v2[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree1.merge(tree2));
	std::vector<key_t> merged;
	merged.insert(merged.end(), k1.begin(), k1.end());
	merged.insert(merged.end(), k2.begin(), k2.end());
	for (auto k : merged) {
		ASSERT_NE(tree1.find(k), tree1.end());
	}
}

TEST(AVL_Tree, insert_all_delete_all) {
	std::vector<key_t> k = { 41, 3, 5, 15, 25, 31, 32, 40, 45, 38, 33, 43, 13 };
	std::vector<value_t> v = convert(k);
	AVL<key_t, value_t> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	const key_t to_delete = 10;
	ASSERT_NO_FATAL_FAILURE(tree.remove(to_delete));
	for (auto x : k) {
		if (x < to_delete || to_delete < x) {
			ASSERT_NE(tree.find(x), tree.end());
		} else {
			ASSERT_EQ(tree.find(x), tree.end());
		}
	}
}
