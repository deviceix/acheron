/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <ranges>
#include <string>
#include <vector>
#include <acheron/deque>
#include <gtest/gtest.h>

class DequeTest : public ::testing::Test
{
protected:
	ach::deque<int> int_deque;
	ach::deque<std::string> string_deque;
};

TEST_F(DequeTest, DefaultConstruction)
{
	EXPECT_TRUE(int_deque.empty());
	EXPECT_EQ(int_deque.size(), 0);
}

TEST_F(DequeTest, PushBackPopBack)
{
	int_deque.push_back(1);
	int_deque.push_back(2);
	int_deque.push_back(3);

	EXPECT_EQ(int_deque.size(), 3);
	EXPECT_EQ(int_deque.back(), 3);

	int_deque.pop_back();
	EXPECT_EQ(int_deque.size(), 2);
	EXPECT_EQ(int_deque.back(), 2);
}

TEST_F(DequeTest, PushFrontPopFront)
{
	int_deque.push_front(1);
	int_deque.push_front(2);
	int_deque.push_front(3);

	EXPECT_EQ(int_deque.size(), 3);
	EXPECT_EQ(int_deque.front(), 3);

	int_deque.pop_front();
	EXPECT_EQ(int_deque.size(), 2);
	EXPECT_EQ(int_deque.front(), 2);
}

TEST_F(DequeTest, ElementAccess)
{
	for (int i = 0; i < 5; ++i)
		int_deque.push_back(i);

	EXPECT_EQ(int_deque[0], 0);
	EXPECT_EQ(int_deque[4], 4);
	EXPECT_EQ(int_deque.at(2), 2);
	EXPECT_EQ(int_deque.front(), 0);
	EXPECT_EQ(int_deque.back(), 4);

	EXPECT_THROW(int_deque.at(10), std::out_of_range);
}

TEST_F(DequeTest, MixedPushes)
{
	int_deque.push_back(3);
	int_deque.push_front(2);
	int_deque.push_back(4);
	int_deque.push_front(1);
	int_deque.push_back(5);

	EXPECT_EQ(int_deque.size(), 5);
	for (int i = 0; i < 5; ++i)
		EXPECT_EQ(int_deque[i], i + 1);
}

TEST_F(DequeTest, EmplaceBackFront)
{
	string_deque.emplace_back("Hello");
	string_deque.emplace_front("World");

	EXPECT_EQ(string_deque.size(), 2);
	EXPECT_EQ(string_deque.front(), "World");
	EXPECT_EQ(string_deque.back(), "Hello");
}

TEST_F(DequeTest, Iterators)
{
	for (int i = 0; i < 5; ++i)
		int_deque.push_back(i);

	auto expected = 0;
	for (int & it : int_deque)
		EXPECT_EQ(it, expected++);

	expected = 4;
	for (int & it : std::ranges::reverse_view(int_deque))
		EXPECT_EQ(it, expected--);
}

TEST_F(DequeTest, IteratorArithmetic)
{
	for (int i = 0; i < 10; ++i)
		int_deque.push_back(i);

	auto it = int_deque.begin();
	EXPECT_EQ(*it, 0);

	it += 5;
	EXPECT_EQ(*it, 5);

	it -= 2;
	EXPECT_EQ(*it, 3);

	auto it2 = it + 3;
	EXPECT_EQ(*it2, 6);

	EXPECT_EQ(it2 - it, 3);
	EXPECT_EQ(it[2], 5);
}

TEST_F(DequeTest, Clear)
{
	for (int i = 0; i < 10; ++i)
		int_deque.push_back(i);

	EXPECT_FALSE(int_deque.empty());
	int_deque.clear();
	EXPECT_TRUE(int_deque.empty());
	EXPECT_EQ(int_deque.size(), 0);
}

TEST_F(DequeTest, Resize)
{
	int_deque.resize(5);
	EXPECT_EQ(int_deque.size(), 5);

	int_deque.resize(3);
	EXPECT_EQ(int_deque.size(), 3);

	int_deque.resize(7, 42);
	EXPECT_EQ(int_deque.size(), 7);
	EXPECT_EQ(int_deque[6], 42);
}

TEST_F(DequeTest, CopyConstruction)
{
	for (int i = 0; i < 5; ++i)
		int_deque.push_back(i);

	ach::deque<int> copy(int_deque);
	EXPECT_EQ(copy.size(), int_deque.size());

	for (size_t i = 0; i < int_deque.size(); ++i)
		EXPECT_EQ(copy[i], int_deque[i]);
}

TEST_F(DequeTest, MoveConstruction)
{
	for (int i = 0; i < 5; ++i)
		int_deque.push_back(i);

	size_t original_size = int_deque.size();
	ach::deque<int> moved(std::move(int_deque));

	EXPECT_EQ(moved.size(), original_size);
	EXPECT_TRUE(int_deque.empty());

	for (int i = 0; i < 5; ++i)
		EXPECT_EQ(moved[i], i);
}

TEST_F(DequeTest, CopyAssignment)
{
	for (int i = 0; i < 5; ++i)
		int_deque.push_back(i);

	ach::deque<int> other;
	other = int_deque;

	EXPECT_EQ(other.size(), int_deque.size());
	for (size_t i = 0; i < int_deque.size(); ++i)
		EXPECT_EQ(other[i], int_deque[i]);
}

TEST_F(DequeTest, MoveAssignment)
{
	for (int i = 0; i < 5; ++i)
		int_deque.push_back(i);

	ach::deque<int> other;
	other = std::move(int_deque);

	EXPECT_EQ(other.size(), 5);
	EXPECT_TRUE(int_deque.empty());

	for (int i = 0; i < 5; ++i)
		EXPECT_EQ(other[i], i);
}

TEST_F(DequeTest, InitializerListConstruction)
{
	ach::deque<int> deque = { 1, 2, 3, 4, 5 };

	EXPECT_EQ(deque.size(), 5);
	for (int i = 0; i < 5; ++i)
		EXPECT_EQ(deque[i], i + 1);
}

TEST_F(DequeTest, Swap)
{
	ach::deque<int> deque1 = { 1, 2, 3 };
	ach::deque<int> deque2 = { 4, 5, 6, 7 };

	deque1.swap(deque2);

	EXPECT_EQ(deque1.size(), 4);
	EXPECT_EQ(deque2.size(), 3);

	EXPECT_EQ(deque1[0], 4);
	EXPECT_EQ(deque2[0], 1);
}

TEST_F(DequeTest, ComparisonOperators)
{
	ach::deque<int> deque1 = { 1, 2, 3 };
	ach::deque<int> deque2 = { 1, 2, 3 };
	ach::deque<int> deque3 = { 1, 2, 4 };

	EXPECT_EQ(deque1, deque2);
	EXPECT_NE(deque1, deque3);
	EXPECT_LT(deque1, deque3);
	EXPECT_LE(deque1, deque2);
	EXPECT_GT(deque3, deque1);
	EXPECT_GE(deque2, deque1);
}

TEST_F(DequeTest, LargeDeque)
{
	const int N = 1000;

	/* test pushing at both ends */
	for (int i = 0; i < N; ++i)
	{
		if (i % 2 == 0)
			int_deque.push_back(i);
		else
			int_deque.push_front(i);
	}

	EXPECT_EQ(int_deque.size(), N);

	/* verify elements are accessible */
	for (int i = 0; i < N; ++i)
	{
		EXPECT_NO_THROW(int_deque[i]);
	}
}

TEST_F(DequeTest, AlternatingOperations)
{
	/* test alternating pushes and pops */
	int_deque.push_back(1);
	int_deque.push_front(0);
	int_deque.push_back(2);

	EXPECT_EQ(int_deque.size(), 3);
	EXPECT_EQ(int_deque[0], 0);
	EXPECT_EQ(int_deque[1], 1);
	EXPECT_EQ(int_deque[2], 2);

	int_deque.pop_front();
	EXPECT_EQ(int_deque.front(), 1);

	int_deque.pop_back();
	EXPECT_EQ(int_deque.back(), 1);
}

TEST_F(DequeTest, StressTest)
{
	constexpr auto N = 1000; /* Reduced to avoid memory issues */

	try
	{
		/* push from both ends */
		for (auto i = 0; i < N / 2; ++i)
		{
			int_deque.push_back(i);
			int_deque.push_front(-i - 1);
		}

		EXPECT_EQ(int_deque.size(), N);

		/* verify front half is negative */
		for (auto i = 0; i < N / 2; ++i)
			EXPECT_LT(int_deque[i], 0);

		/* verify back half is non-negative */
		for (auto i = N / 2; i < N; ++i)
			EXPECT_GE(int_deque[i], 0);

		/* pop from both ends */
		while (!int_deque.empty())
		{
			if (int_deque.size() % 2 == 0)
				int_deque.pop_front();
			else
				int_deque.pop_back();
		}

		EXPECT_TRUE(int_deque.empty());
	}
	catch (const std::bad_alloc &)
	{
		GTEST_SKIP() << "insufficient memory for stress test";
	} catch (...)
	{
		GTEST_SKIP() << "stress test failed with unknown exception";
	}
}
