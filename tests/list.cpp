/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <acheron/list>
#include <gtest/gtest.h>
#include <vector>
#include <string>

class ListTest : public ::testing::Test
{
protected:
	ach::list<int> int_list;
	ach::list<std::string> string_list;
};

TEST_F(ListTest, DefaultConstruction)
{
	EXPECT_TRUE(int_list.empty());
	EXPECT_EQ(int_list.size(), 0);
}

TEST_F(ListTest, FillConstruction)
{
	ach::list filled(static_cast<size_t>(5), 42);
	EXPECT_EQ(filled.size(), 5);
	for (const auto &val: filled)
	{
		EXPECT_EQ(val, 42);
	}
}

TEST_F(ListTest, RangeConstruction)
{
	std::vector<int> vec = { 1, 2, 3, 4, 5 };
	ach::list<int> from_vec(vec.begin(), vec.end());

	EXPECT_EQ(from_vec.size(), vec.size());
	auto it = from_vec.begin();
	for (size_t i = 0; i < vec.size(); ++i, ++it)
	{
		EXPECT_EQ(*it, vec[i]);
	}
}

TEST_F(ListTest, InitializerListConstruction)
{
	ach::list<int> init_list = { 1, 2, 3, 4, 5 };
	EXPECT_EQ(init_list.size(), 5);

	int expected = 1;
	for (const auto &val: init_list)
	{
		EXPECT_EQ(val, expected++);
	}
}

TEST_F(ListTest, CopyConstruction)
{
	int_list.push_back(1);
	int_list.push_back(2);
	int_list.push_back(3);

	ach::list<int> copy(int_list);
	EXPECT_EQ(copy.size(), int_list.size());

	auto orig_it = int_list.begin();
	auto copy_it = copy.begin();
	while (orig_it != int_list.end())
	{
		EXPECT_EQ(*orig_it++, *copy_it++);
	}
}

TEST_F(ListTest, MoveConstruction)
{
	int_list.push_back(1);
	int_list.push_back(2);
	int_list.push_back(3);

	ach::list<int> moved(std::move(int_list));
	EXPECT_EQ(moved.size(), 3);
	EXPECT_TRUE(int_list.empty());

	int expected = 1;
	for (const auto &val: moved)
	{
		EXPECT_EQ(val, expected++);
	}
}

TEST_F(ListTest, PushBackPopBack)
{
	int_list.push_back(1);
	int_list.push_back(2);
	int_list.push_back(3);

	EXPECT_EQ(int_list.size(), 3);
	EXPECT_EQ(int_list.back(), 3);

	int_list.pop_back();
	EXPECT_EQ(int_list.size(), 2);
	EXPECT_EQ(int_list.back(), 2);
}

TEST_F(ListTest, PushFrontPopFront)
{
	int_list.push_front(1);
	int_list.push_front(2);
	int_list.push_front(3);

	EXPECT_EQ(int_list.size(), 3);
	EXPECT_EQ(int_list.front(), 3);

	int_list.pop_front();
	EXPECT_EQ(int_list.size(), 2);
	EXPECT_EQ(int_list.front(), 2);
}

TEST_F(ListTest, EmplaceBackFront)
{
	string_list.emplace_back("Hello");
	string_list.emplace_front("World");

	EXPECT_EQ(string_list.size(), 2);
	EXPECT_EQ(string_list.front(), "World");
	EXPECT_EQ(string_list.back(), "Hello");
}

TEST_F(ListTest, Insert)
{
	int_list.push_back(1);
	int_list.push_back(3);

	auto it = int_list.begin();
	++it; /* point to position before 3 */
	int_list.insert(it, 2);

	EXPECT_EQ(int_list.size(), 3);
	int expected = 1;
	for (const auto &val: int_list)
	{
		EXPECT_EQ(val, expected++);
	}
}

TEST_F(ListTest, Erase)
{
	int_list = { 1, 2, 3, 4, 5 };

	auto it = int_list.begin();
	++it;
	++it; /* point to 3 */
	it = int_list.erase(it);

	EXPECT_EQ(int_list.size(), 4);
	EXPECT_EQ(*it, 4);

	std::vector<int> expected = { 1, 2, 4, 5 };
	it = int_list.begin();
	for (const auto &exp: expected)
	{
		EXPECT_EQ(*it++, exp);
	}
}

TEST_F(ListTest, Clear)
{
	int_list = { 1, 2, 3, 4, 5 };
	EXPECT_FALSE(int_list.empty());

	int_list.clear();
	EXPECT_TRUE(int_list.empty());
	EXPECT_EQ(int_list.size(), 0);
}

TEST_F(ListTest, Resize)
{
	int_list = { 1, 2, 3 };

	int_list.resize(5);
	EXPECT_EQ(int_list.size(), 5);

	int_list.resize(2);
	EXPECT_EQ(int_list.size(), 2);
	EXPECT_EQ(int_list.back(), 2);
}

TEST_F(ListTest, Reverse)
{
	int_list = { 1, 2, 3, 4, 5 };
	int_list.reverse();

	std::vector<int> expected = { 5, 4, 3, 2, 1 };
	auto it = int_list.begin();
	for (const auto &exp: expected)
	{
		EXPECT_EQ(*it++, exp);
	}
}

TEST_F(ListTest, Sort)
{
	int_list = { 5, 2, 8, 1, 9, 3 };
	int_list.sort();

	std::vector<int> expected = { 1, 2, 3, 5, 8, 9 };
	auto it = int_list.begin();
	for (const auto &exp: expected)
	{
		EXPECT_EQ(*it++, exp);
	}
}

TEST_F(ListTest, Unique)
{
	int_list = { 1, 1, 2, 2, 2, 3, 3, 4 };
	size_t removed = int_list.unique();

	EXPECT_EQ(removed, 4);
	EXPECT_EQ(int_list.size(), 4);

	std::vector<int> expected = { 1, 2, 3, 4 };
	auto it = int_list.begin();
	for (const auto &exp: expected)
	{
		EXPECT_EQ(*it++, exp);
	}
}

TEST_F(ListTest, Remove)
{
	int_list = { 1, 2, 3, 2, 4, 2, 5 };
	size_t removed = int_list.remove(2);

	EXPECT_EQ(removed, 3);
	EXPECT_EQ(int_list.size(), 4);

	std::vector<int> expected = { 1, 3, 4, 5 };
	auto it = int_list.begin();
	for (const auto &exp: expected)
	{
		EXPECT_EQ(*it++, exp);
	}
}

TEST_F(ListTest, Merge)
{
	ach::list<int> list1 = { 1, 3, 5, 7 };
	ach::list<int> list2 = { 2, 4, 6, 8 };

	list1.merge(list2);

	EXPECT_TRUE(list2.empty());
	EXPECT_EQ(list1.size(), 8);

	int expected = 1;
	for (const auto &val: list1)
	{
		EXPECT_EQ(val, expected++);
	}
}

TEST_F(ListTest, Splice)
{
	ach::list<int> list1 = { 1, 2, 3 };
	ach::list<int> list2 = { 4, 5, 6 };

	list1.splice(list1.end(), list2);

	EXPECT_TRUE(list2.empty());
	EXPECT_EQ(list1.size(), 6);

	int expected = 1;
	for (const auto &val: list1)
	{
		EXPECT_EQ(val, expected++);
	}
}

TEST_F(ListTest, IteratorOperations)
{
	int_list = { 1, 2, 3, 4, 5 };

	auto it = int_list.begin();
	EXPECT_EQ(*it, 1);
	++it;
	EXPECT_EQ(*it, 2);

	auto rit = int_list.rbegin();
	EXPECT_EQ(*rit, 5);
	++rit;
	EXPECT_EQ(*rit, 4);
}

TEST_F(ListTest, ConstIterator)
{
	const ach::list<int> const_list = { 1, 2, 3, 4, 5 };

	auto expected = 1;
	for (int it : const_list)
	{
		EXPECT_EQ(it, expected++);
	}
}

TEST_F(ListTest, ComparisonOperators)
{
	ach::list<int> list1 = { 1, 2, 3 };
	ach::list<int> list2 = { 1, 2, 3 };
	ach::list<int> list3 = { 1, 2, 4 };

	EXPECT_EQ(list1, list2);
	EXPECT_NE(list1, list3);
	EXPECT_LT(list1, list3);
	EXPECT_LE(list1, list2);
	EXPECT_GT(list3, list1);
	EXPECT_GE(list2, list1);
}

TEST_F(ListTest, Swap)
{
	ach::list<int> list1 = { 1, 2, 3 };
	ach::list<int> list2 = { 4, 5, 6 };

	list1.swap(list2);

	EXPECT_EQ(list1.size(), 3);
	EXPECT_EQ(list2.size(), 3);

	EXPECT_EQ(list1.front(), 4);
	EXPECT_EQ(list2.front(), 1);
}
