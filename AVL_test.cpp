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

static char int2char(int n) {
	return n % 256 + 'a';
}
static std::vector<char> int2char(std::vector<int> v) {
	std::vector<char> c;
	for (auto n : v) {
		c.push_back(int2char(n));
	}
	return c;
}

TEST(AVL_Tree, simple_creation) {
	AVL<int, char> tree;
}

TEST(AVL_Tree, begin_equals_end_empty_tree) {
	AVL<int, char> tree;
	ASSERT_EQ(tree.begin(), tree.end());
}

TEST(AVL_Tree, insert_first_success) {
	const int k = 3;
	const char v = int2char(k);
	AVL<int, char> tree;
	ASSERT_TRUE(tree.insert(k, v));
}

TEST(AVL_Tree, insert_first_already_exist) {
	const int k = 3;
	const char v = int2char(k);
	AVL<int, char> tree;
	tree.insert(k, v);
	ASSERT_FALSE(tree.insert(k, v));
}

TEST(AVL_Tree, insert_few_success_no_rolls) {
	std::vector<int> k = { 3, 10, 1 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_TRUE(tree.insert(k[i], v[i]));
	}
	ASSERT_NE(tree.end(), tree.find(k[0]));
	ASSERT_NE(tree.end(), tree.find(k[1]));
	ASSERT_NE(tree.end(), tree.find(k[2]));
}

TEST(AVL_Tree, find_fail_empty_tree) {
	const int k = 3;
	AVL<int, char> tree;
	ASSERT_EQ(tree.end(), tree.find(k));
}

TEST(AVL_Tree, find_root_success) {
	const int k = 3;
	const char v = int2char(k);
	AVL<int, char> tree;
	tree.insert(k, v);
	ASSERT_NE(tree.end(), tree.find(k));
	ASSERT_EQ(v, *(tree.find(k)));
}

TEST(AVL_Tree, iterator_preincrement) {
	std::vector<int> k = { 3, 10, 1 };
	std::vector<char> v = { 'c', 'j', 'a' };
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto j = tree.begin();
	ASSERT_EQ(*j, 'a');
	++j;
	ASSERT_EQ(*j, 'c');
	++j;
	ASSERT_EQ(*j, 'j');
	++j;
	ASSERT_EQ(j, tree.end());
}

TEST(AVL_Tree, iterator_postincrement) {
	std::vector<int> k = { 3, 10, 1 };
	std::vector<char> v = { 'c', 'j', 'a' };
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto j = tree.begin();
	ASSERT_EQ(*j, 'a');
	j++;
	ASSERT_EQ(*j, 'c');
	j++;
	ASSERT_EQ(*j, 'j');
	j++;
	ASSERT_EQ(j, tree.end());
}

TEST(AVL_Tree, insert_ll_roll_3) {
	std::vector<int> k = { 10, 5, 3 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(v.begin(), v.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(*it, v[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_rr_roll_3) {
	std::vector<int> k = { 3, 5, 10 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(v.begin(), v.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(*it, v[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_rl_roll_3) {
	std::vector<int> k = { 10, 15, 11 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(v.begin(), v.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(*it, v[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_lr_roll_3) {
	std::vector<int> k = { 10, 2, 5 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(v.begin(), v.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(*it, v[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_ll_roll_more) {
	std::vector<int> k = { 13, 10, 15, 16, 11, 5, 8, 4, 3 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(v.begin(), v.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(*it, v[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_rr_roll_more) {
	std::vector<int> k = { 10, 5, 18, 15, 22, 25 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(v.begin(), v.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(*it, v[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_rl_roll_more) {
	std::vector<int> k = { 13, 10, 15, 16, 11, 5, 4, 6, 7 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(v.begin(), v.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(*it, v[i]);
		++it;
	}
}

TEST(AVL_Tree, insert_lr_roll_more) {
	std::vector<int> k = { 5, 2, 7, 6, 9, 1, 4, 3, 16, 15 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	auto it = tree.begin();
	std::sort(v.begin(), v.end());
	for (unsigned int i = 0; i < k.size(); ++i) {
		ASSERT_EQ(*it, v[i]);
		++it;
	}
}

TEST(AVL_Tree, for_in_compactibility) {
	std::vector<int> k = { 2, 16, 40, 31, 7, 5, 32, 11, 17 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	for (auto x : tree) {
		ASSERT_NE(std::find(v.begin(), v.end(), x), v.end());
	}
}

TEST(AVL_Tree, delete_root_tree_of_only_root) {
	const int k = 3;
	const char v = int2char(k);
	AVL<int, char> tree;
	tree.insert(k, v);
	ASSERT_NO_FATAL_FAILURE(tree.remove(k));
	ASSERT_TRUE(tree.empty());
}

TEST(AVL_Tree, delete_unexisting_node_tree_unchanged) {
	std::vector<int> k = { 2, 16, 40, 31, 7, 5, 32, 11, 17 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree.remove(-1));
	for (auto i : k) {
		ASSERT_NE(tree.find(i), tree.end());
	}
}

TEST(AVL_Tree, delete_no_roll_leaf) {
	std::vector<int> k = { 3, 10, 1 };
	std::vector<char> v = { 'c', 'j', 'a' };
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree.remove(1));
	ASSERT_NO_FATAL_FAILURE(tree.remove(10));
	ASSERT_EQ(tree.find(1), tree.end());
	ASSERT_EQ(tree.find(10), tree.end());
	ASSERT_NE(tree.find(3), tree.end());
}

TEST(AVL_Tree, delete_no_roll_root_when_root_has_1_leaf) {
	std::vector<int> k = { 3, 10 };
	std::vector<char> v = { 'c', 'j' };
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree.remove(3));
	auto new_root = tree.begin();
	ASSERT_EQ(*new_root, 'j');
}

TEST(AVL_Tree, delete_no_roll_root_when_root_has_2_leaves) {
	std::vector<int> k = { 3, 10, 1 };
	std::vector<char> v = { 'c', 'j', 'a' };
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree.remove(3));
	ASSERT_NE(tree.find(1), tree.end());
	ASSERT_NE(tree.find(10), tree.end());
	ASSERT_EQ(tree.find(3), tree.end());
}

TEST(AVL_Tree, delete_roll_RL) {
	std::vector<int> k = { 20, 10, 30, 25 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	const int to_delete = 10;
	ASSERT_NO_FATAL_FAILURE(tree.remove(to_delete));
	for (auto x : k) {
		if (x != to_delete) {
			ASSERT_NE(tree.find(x), tree.end());
		} else {
			ASSERT_EQ(tree.find(x), tree.end());
		}
	}
}

TEST(AVL_Tree, delete_roll_RR) {
	std::vector<int> k = { 20, 10, 30, 35 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	const int to_delete = 10;
	ASSERT_NO_FATAL_FAILURE(tree.remove(to_delete));
	for (auto x : k) {
		if (x != to_delete) {
			ASSERT_NE(tree.find(x), tree.end());
		} else {
			ASSERT_EQ(tree.find(x), tree.end());
		}
	}
}
TEST(AVL_Tree, delete_roll_LR) {
	std::vector<int> k = { 20, 10, 30, 15 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	const int to_delete = 30;
	ASSERT_NO_FATAL_FAILURE(tree.remove(to_delete));
	for (auto x : k) {
		if (x != to_delete) {
			ASSERT_NE(tree.find(x), tree.end());
		} else {
			ASSERT_EQ(tree.find(x), tree.end());
		}
	}
}
TEST(AVL_Tree, delete_roll_LL) {
	std::vector<int> k = { 20, 10, 30, 5 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
	}
	const int to_delete = 30;
	ASSERT_NO_FATAL_FAILURE(tree.remove(to_delete));
	for (auto x : k) {
		if (x != to_delete) {
			ASSERT_NE(tree.find(x), tree.end());
		} else {
			ASSERT_EQ(tree.find(x), tree.end());
		}
	}
}

TEST(AVL_Tree, tree_of_trees_creation) {
	AVL<int, AVL<int, char>> tree;
	AVL<int, char> inner_tree;
	ASSERT_TRUE(tree.insert(100, inner_tree));
	ASSERT_TRUE((*tree.find(100)).insert(1, 'a'));
}

TEST(AVL_Tree, merge_left_empty) {
	std::vector<int> k = { 2, 16, 40, 31, 7, 5, 32, 11, 17 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree1, tree2;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree1.insert(k[i], v[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree1.merge(tree2));
	std::vector<int> merged;
	merged.insert(merged.end(), k.begin(), k.end());
	for (auto k : merged) {
		ASSERT_NE(tree1.find(k), tree1.end());
	}
}

TEST(AVL_Tree, merge_right_empty) {
	std::vector<int> k = { 2, 16, 40, 31, 7, 5, 32, 11, 17 };
	std::vector<char> v = int2char(k);
	AVL<int, char> tree1, tree2;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree2.insert(k[i], v[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree1.merge(tree2));
	std::vector<int> merged;
	merged.insert(merged.end(), k.begin(), k.end());
	for (auto k : merged) {
		ASSERT_NE(tree1.find(k), tree1.end());
	}
}

TEST(AVL_Tree, merge_all_keys_unique) {
	std::vector<int> k1 = { 2, 16, 40, 31, 7, 32, 11, 17 };
	std::vector<char> v1 = int2char(k1);
	std::vector<int> k2 = { 10, 5, 18, 15, 22, 25 };
	std::vector<char> v2 = int2char(k2);
	AVL<int, char> tree1, tree2;
	for (unsigned int i = 0; i < k1.size(); ++i) {
		tree1.insert(k1[i], v1[i]);
	}
	for (unsigned int i = 0; i < k2.size(); ++i) {
		tree2.insert(k2[i], v2[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree1.merge(tree2));
	std::vector<int> merged;
	merged.insert(merged.end(), k1.begin(), k1.end());
	merged.insert(merged.end(), k2.begin(), k2.end());
	for (auto k : merged) {
		ASSERT_NE(tree1.find(k), tree1.end());
	}
}

TEST(AVL_Tree, merge_some_keys_same) {
	std::vector<int> k1 = { 2, 16, 32, 11, 17 };
	std::vector<char> v1 = int2char(k1);
	std::vector<int> k2 = { 10, 5, 11, 18, 15, 22, 17, 25 };
	std::vector<char> v2 = int2char(k2);
	AVL<int, char> tree1, tree2;
	for (unsigned int i = 0; i < k1.size(); ++i) {
		tree1.insert(k1[i], v1[i]);
	}
	for (unsigned int i = 0; i < k2.size(); ++i) {
		tree2.insert(k2[i], v2[i]);
	}
	ASSERT_NO_FATAL_FAILURE(tree1.merge(tree2));
	std::vector<int> merged;
	merged.insert(merged.end(), k1.begin(), k1.end());
	merged.insert(merged.end(), k2.begin(), k2.end());
	for (auto k : merged) {
		ASSERT_NE(tree1.find(k), tree1.end());
	}
}

// TODO check merge case with same keys
