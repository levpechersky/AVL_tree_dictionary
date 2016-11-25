/*
 * AVL_test.cpp
 *
 *  Created on: 2016-11-22
 *      Author: Lev Pechersky
 */
#include <vector>
#include <gtest\gtest.h>
#include "AVL.hpp"

TEST(AVL_Tree, simple_creation) {
	AVL<int, char> tree;
}

TEST(AVL_Tree, begin_equals_end_empty_tree) {
	AVL<int, char> tree;
	ASSERT_EQ(tree.begin(), tree.end());
}

TEST(AVL_Tree, insert_first_success) {
	const int k = 3;
	const char v = 'c';
	AVL<int, char> tree;
	ASSERT_TRUE(tree.insert(k, v));
}

TEST(AVL_Tree, insert_first_already_exist) {
	const int k = 3;
	const char v = 'c';
	AVL<int, char> tree;
	tree.insert(k, v);
	ASSERT_FALSE(tree.insert(k, v));
}

TEST(AVL_Tree, insert_few_success) {
	std::vector<int> k = { 3, 10, 1 };
	std::vector<char> v = { 'c', 'j', 'a' };
	AVL<int, char> tree;
	for (unsigned int i = 0; i < k.size(); ++i) {
		tree.insert(k[i], v[i]);
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
	const char v = 'c';
	AVL<int, char> tree;
	tree.insert(k, v);
	ASSERT_NE(tree.end(), tree.find(k));
	ASSERT_EQ(v, *(tree.find(k)));
}

TEST(AVL_Tree, iterator_next_preincrement) {
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

TEST(AVL_Tree, iterator_next_postincrement) {
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
