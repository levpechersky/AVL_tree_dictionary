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
	for(auto x : tree){
		ASSERT_NE(std::find(v.begin(), v.end(), x), v.end());
	}
}
