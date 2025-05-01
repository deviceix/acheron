/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <string>
#include <vector>
#include <acheron/vector>
#include <gtest/gtest.h>

class VectorTest : public ::testing::Test
{
protected:
	ach::vector<int> int_vector;
	ach::vector<std::string> string_vector;
};

TEST_F(VectorTest, DefaultConstruction)
{
	EXPECT_TRUE(int_vector.empty());
	EXPECT_EQ(int_vector.size_val(), 0);
	EXPECT_EQ(int_vector.capacity_val(), 0);
}

TEST_F(VectorTest, FillConstruction)
{
	ach::vector filled(static_cast<size_t>(5), 42);
	EXPECT_EQ(filled.size_val(), 5);
	EXPECT_GE(filled.capacity_val(), 5);
	for (const auto &val: filled)
	{
		EXPECT_EQ(val, 42);
	}
}

TEST_F(VectorTest, RangeConstruction)
{
	std::vector<int> vec = { 1, 2, 3, 4, 5 };
	ach::vector<int> from_vec(vec.begin(), vec.end());

	EXPECT_EQ(from_vec.size_val(), vec.size());
	for (size_t i = 0; i < vec.size(); ++i)
	{
		EXPECT_EQ(from_vec[i], vec[i]);
	}
}

TEST_F(VectorTest, InitializerListConstruction)
{
	ach::vector<int> init_vector = { 1, 2, 3, 4, 5 };
	EXPECT_EQ(init_vector.size_val(), 5);

	int expected = 1;
	for (const auto &val: init_vector)
	{
		EXPECT_EQ(val, expected++);
	}
}

TEST_F(VectorTest, CopyConstruction)
{
	int_vector.push_back(1);
	int_vector.push_back(2);
	int_vector.push_back(3);

	ach::vector<int> copy(int_vector);
	EXPECT_EQ(copy.size_val(), int_vector.size_val());

	for (size_t i = 0; i < int_vector.size_val(); ++i)
	{
		EXPECT_EQ(copy[i], int_vector[i]);
	}
}

TEST_F(VectorTest, MoveConstruction)
{
	int_vector.push_back(1);
	int_vector.push_back(2);
	int_vector.push_back(3);

	ach::vector<int> moved(std::move(int_vector));
	EXPECT_EQ(moved.size_val(), 3);
	EXPECT_TRUE(int_vector.empty());

	int expected = 1;
	for (const auto &val: moved)
	{
		EXPECT_EQ(val, expected++);
	}
}

TEST_F(VectorTest, ElementAccess)
{
	int_vector = { 1, 2, 3, 4, 5 };

	EXPECT_EQ(int_vector[0], 1);
	EXPECT_EQ(int_vector[4], 5);
	EXPECT_EQ(int_vector.at(2), 3);
	EXPECT_EQ(int_vector.front(), 1);
	EXPECT_EQ(int_vector.back(), 5);

	EXPECT_THROW(int_vector.at(10), std::out_of_range);
}

TEST_F(VectorTest, PushBackPopBack)
{
	int_vector.push_back(1);
	int_vector.push_back(2);
	int_vector.push_back(3);

	EXPECT_EQ(int_vector.size_val(), 3);
	EXPECT_EQ(int_vector.back(), 3);

	int_vector.pop_back();
	EXPECT_EQ(int_vector.size_val(), 2);
	EXPECT_EQ(int_vector.back(), 2);
}

TEST_F(VectorTest, EmplaceBack)
{
	string_vector.emplace_back("Hello");
	string_vector.emplace_back(5, 'A');

	EXPECT_EQ(string_vector.size_val(), 2);
	EXPECT_EQ(string_vector[0], "Hello");
	EXPECT_EQ(string_vector[1], "AAAAA");
}

TEST_F(VectorTest, Insert)
{
	int_vector.push_back(1);
	int_vector.push_back(3);

	auto it = int_vector.begin() + 1;
	int_vector.insert(it, 2);

	EXPECT_EQ(int_vector.size_val(), 3);
	int expected = 1;
	for (const auto &val: int_vector)
	{
		EXPECT_EQ(val, expected++);
	}
}

TEST_F(VectorTest, InsertMultiple)
{
	int_vector = { 1, 5 };

	auto it = int_vector.begin() + 1;
	int_vector.insert(it, 3, 2);

	EXPECT_EQ(int_vector.size_val(), 5);
	std::vector<int> expected = { 1, 2, 2, 2, 5 };
	for (size_t i = 0; i < expected.size(); ++i)
	{
		EXPECT_EQ(int_vector[i], expected[i]);
	}
}

TEST_F(VectorTest, InsertRange)
{
	int_vector = { 1, 5 };
	std::vector<int> to_insert = { 2, 3, 4 };

	auto it = int_vector.begin() + 1;
	int_vector.insert(it, to_insert.begin(), to_insert.end());

	EXPECT_EQ(int_vector.size_val(), 5);
	int expected = 1;
	for (const auto &val: int_vector)
	{
		EXPECT_EQ(val, expected++);
	}
}

TEST_F(VectorTest, Erase)
{
	int_vector = { 1, 2, 3, 4, 5 };

	auto it = int_vector.begin() + 2;
	it = int_vector.erase(it);

	EXPECT_EQ(int_vector.size_val(), 4);
	EXPECT_EQ(*it, 4);

	std::vector<int> expected = { 1, 2, 4, 5 };
	for (size_t i = 0; i < expected.size(); ++i)
	{
		EXPECT_EQ(int_vector[i], expected[i]);
	}
}

TEST_F(VectorTest, EraseRange)
{
	int_vector = { 1, 2, 3, 4, 5 };

	auto first = int_vector.begin() + 1;
	auto last = int_vector.begin() + 4;
	auto it = int_vector.erase(first, last);

	EXPECT_EQ(int_vector.size_val(), 2);
	EXPECT_EQ(*it, 5);

	EXPECT_EQ(int_vector[0], 1);
	EXPECT_EQ(int_vector[1], 5);
}

TEST_F(VectorTest, Clear)
{
	int_vector = { 1, 2, 3, 4, 5 };
	EXPECT_FALSE(int_vector.empty());

	int_vector.clear();
	EXPECT_TRUE(int_vector.empty());
	EXPECT_EQ(int_vector.size_val(), 0);
	EXPECT_GT(int_vector.capacity_val(), 0);
}

TEST_F(VectorTest, Reserve)
{
	int_vector.reserve(100);
	EXPECT_EQ(int_vector.size_val(), 0);
	EXPECT_GE(int_vector.capacity_val(), 100);

	size_t old_capacity = int_vector.capacity_val();
	int_vector.push_back(1);
	EXPECT_EQ(int_vector.capacity_val(), old_capacity);
}

TEST_F(VectorTest, ShrinkToFit)
{
	int_vector.reserve(100);
	int_vector.push_back(1);
	int_vector.push_back(2);

	EXPECT_GT(int_vector.capacity_val(), int_vector.size_val());

	int_vector.shrink_to_fit();
	EXPECT_EQ(int_vector.capacity_val(), int_vector.size_val());
}

TEST_F(VectorTest, Resize)
{
	int_vector = { 1, 2, 3 };

	int_vector.resize(5);
	EXPECT_EQ(int_vector.size_val(), 5);
	EXPECT_EQ(int_vector[0], 1);
	EXPECT_EQ(int_vector[1], 2);
	EXPECT_EQ(int_vector[2], 3);

	int_vector.resize(2);
	EXPECT_EQ(int_vector.size_val(), 2);
	EXPECT_EQ(int_vector[0], 1);
	EXPECT_EQ(int_vector[1], 2);
}

TEST_F(VectorTest, ResizeWithValue)
{
	int_vector = { 1, 2, 3 };

	int_vector.resize(5, 42);
	EXPECT_EQ(int_vector.size_val(), 5);
	EXPECT_EQ(int_vector[3], 42);
	EXPECT_EQ(int_vector[4], 42);
}

TEST_F(VectorTest, IteratorOperations)
{
	int_vector = { 1, 2, 3, 4, 5 };

	auto it = int_vector.begin();
	EXPECT_EQ(*it, 1);
	++it;
	EXPECT_EQ(*it, 2);

	auto rit = int_vector.rbegin();
	EXPECT_EQ(*rit, 5);
	++rit;
	EXPECT_EQ(*rit, 4);
}

TEST_F(VectorTest, ConstIterator)
{
	const ach::vector<int> const_vector = { 1, 2, 3, 4, 5 };

	auto expected = 1;
	for (int it : const_vector)
	{
		EXPECT_EQ(it, expected++);
	}
}

TEST_F(VectorTest, DataPointer)
{
	int_vector = { 1, 2, 3, 4, 5 };

	int* data = int_vector.data_ptr();
	EXPECT_EQ(data[0], 1);
	EXPECT_EQ(data[4], 5);
}

TEST_F(VectorTest, ComparisonOperators)
{
	ach::vector<int> vector1 = { 1, 2, 3 };
	ach::vector<int> vector2 = { 1, 2, 3 };
	ach::vector<int> vector3 = { 1, 2, 4 };

	EXPECT_EQ(vector1, vector2);
	EXPECT_NE(vector1, vector3);
	EXPECT_LT(vector1, vector3);
	EXPECT_LE(vector1, vector2);
	EXPECT_GT(vector3, vector1);
	EXPECT_GE(vector2, vector1);
}

TEST_F(VectorTest, Swap)
{
	ach::vector<int> vector1 = { 1, 2, 3 };
	ach::vector<int> vector2 = { 4, 5, 6 };

	vector1.swap(vector2);

	EXPECT_EQ(vector1.size_val(), 3);
	EXPECT_EQ(vector2.size_val(), 3);

	EXPECT_EQ(vector1[0], 4);
	EXPECT_EQ(vector2[0], 1);
}

TEST_F(VectorTest, CapacityGrowth)
{
	size_t old_capacity = int_vector.capacity_val();
	for (int i = 0; i < 10; ++i)
	{
		int_vector.push_back(i);
		if (int_vector.capacity_val() != old_capacity)
		{
			EXPECT_GT(int_vector.capacity_val(), old_capacity);
			old_capacity = int_vector.capacity_val();
		}
	}
}

TEST_F(VectorTest, MoveSemantics)
{
	string_vector.push_back("Hello");
	string_vector.push_back("World");

	ach::vector<std::string> moved_vector = std::move(string_vector);
	EXPECT_TRUE(string_vector.empty());
	EXPECT_EQ(moved_vector.size_val(), 2);
	EXPECT_EQ(moved_vector[0], "Hello");
	EXPECT_EQ(moved_vector[1], "World");
}

TEST_F(VectorTest, AssignOperations)
{
	int_vector.assign(5, 42);
	EXPECT_EQ(int_vector.size_val(), 5);
	for (const auto &val: int_vector)
	{
		EXPECT_EQ(val, 42);
	}

	std::vector<int> data = { 1, 2, 3 };
	int_vector.assign(data.begin(), data.end());
	EXPECT_EQ(int_vector.size_val(), 3);
	for (size_t i = 0; i < data.size(); ++i)
	{
		EXPECT_EQ(int_vector[i], data[i]);
	}

	int_vector.assign({ 4, 5, 6, 7 });
	EXPECT_EQ(int_vector.size_val(), 4);
	EXPECT_EQ(int_vector[0], 4);
	EXPECT_EQ(int_vector[3], 7);
}
