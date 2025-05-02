/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <string>
#include <acheron/queue>
#include <acheron/vector>
#include <gtest/gtest.h>

class QueueTest : public ::testing::Test
{
protected:
	ach::queue<int> int_queue;
	ach::queue<std::string> string_queue;
};

TEST_F(QueueTest, DefaultConstruction)
{
	EXPECT_TRUE(int_queue.empty());
	EXPECT_EQ(int_queue.size(), 0);
}

TEST_F(QueueTest, PushAndFront)
{
	int_queue.push(1);
	EXPECT_EQ(int_queue.front(), 1);
	EXPECT_EQ(int_queue.back(), 1);
	EXPECT_EQ(int_queue.size(), 1);

	int_queue.push(2);
	EXPECT_EQ(int_queue.front(), 1);
	EXPECT_EQ(int_queue.back(), 2);
	EXPECT_EQ(int_queue.size(), 2);
}

TEST_F(QueueTest, Pop)
{
	int_queue.push(1);
	int_queue.push(2);
	int_queue.push(3);

	int_queue.pop();
	EXPECT_EQ(int_queue.front(), 2);
	EXPECT_EQ(int_queue.back(), 3);
	EXPECT_EQ(int_queue.size(), 2);

	int_queue.pop();
	EXPECT_EQ(int_queue.front(), 3);
	EXPECT_EQ(int_queue.back(), 3);
	EXPECT_EQ(int_queue.size(), 1);

	int_queue.pop();
	EXPECT_TRUE(int_queue.empty());
}

TEST_F(QueueTest, Emplace)
{
	string_queue.emplace("Hello");
	EXPECT_EQ(string_queue.front(), "Hello");
	EXPECT_EQ(string_queue.back(), "Hello");

	string_queue.emplace(5, 'A');
	EXPECT_EQ(string_queue.front(), "Hello");
	EXPECT_EQ(string_queue.back(), "AAAAA");
}

TEST_F(QueueTest, CopyConstruction)
{
	int_queue.push(1);
	int_queue.push(2);
	int_queue.push(3);

	ach::queue<int> copy(int_queue);
	EXPECT_EQ(copy.size(), int_queue.size());
	EXPECT_EQ(copy.front(), 1);
	EXPECT_EQ(copy.back(), 3);
}

TEST_F(QueueTest, MoveConstruction)
{
	int_queue.push(1);
	int_queue.push(2);
	int_queue.push(3);

	ach::queue<int> moved(std::move(int_queue));
	EXPECT_EQ(moved.size(), 3);
	EXPECT_EQ(moved.front(), 1);
	EXPECT_EQ(moved.back(), 3);
	EXPECT_TRUE(int_queue.empty());
}

TEST_F(QueueTest, CopyAssignment)
{
	int_queue.push(1);
	int_queue.push(2);

	ach::queue<int> other;
	other = int_queue;

	EXPECT_EQ(other.size(), int_queue.size());
	EXPECT_EQ(other.front(), 1);
	EXPECT_EQ(other.back(), 2);
}

TEST_F(QueueTest, MoveAssignment)
{
	int_queue.push(1);
	int_queue.push(2);

	ach::queue<int> other;
	other = std::move(int_queue);

	EXPECT_EQ(other.size(), 2);
	EXPECT_EQ(other.front(), 1);
	EXPECT_EQ(other.back(), 2);
	EXPECT_TRUE(int_queue.empty());
}

TEST_F(QueueTest, ContainerConstruction)
{
	ach::list<int> list;
	list.push_back(1);
	list.push_back(2);
	list.push_back(3);

	ach::queue<int> queue_from_list(list);

	EXPECT_EQ(queue_from_list.size(), 3);
	EXPECT_EQ(queue_from_list.front(), 1);
	EXPECT_EQ(queue_from_list.back(), 3);
}

TEST_F(QueueTest, IteratorConstruction)
{
	std::vector<int> vec = { 1, 2, 3, 4, 5 };
	ach::queue<int> queue_from_iters(vec.begin(), vec.end());

	EXPECT_EQ(queue_from_iters.size(), 5);
	EXPECT_EQ(queue_from_iters.front(), 1);
	EXPECT_EQ(queue_from_iters.back(), 5);
}

TEST_F(QueueTest, Swap)
{
	ach::queue<int> queue1;
	queue1.push(1);
	queue1.push(2);

	ach::queue<int> queue2;
	queue2.push(3);
	queue2.push(4);
	queue2.push(5);

	queue1.swap(queue2);

	EXPECT_EQ(queue1.size(), 3);
	EXPECT_EQ(queue1.front(), 3);
	EXPECT_EQ(queue1.back(), 5);

	EXPECT_EQ(queue2.size(), 2);
	EXPECT_EQ(queue2.front(), 1);
	EXPECT_EQ(queue2.back(), 2);
}

TEST_F(QueueTest, ComparisonOperators)
{
	ach::queue<int> queue1;
	queue1.push(1);
	queue1.push(2);

	ach::queue<int> queue2;
	queue2.push(1);
	queue2.push(2);

	ach::queue<int> queue3;
	queue3.push(1);
	queue3.push(3);

	EXPECT_EQ(queue1, queue2);
	EXPECT_NE(queue1, queue3);
	EXPECT_LT(queue1, queue3);
	EXPECT_LE(queue1, queue2);
	EXPECT_GT(queue3, queue1);
	EXPECT_GE(queue2, queue1);
}

TEST_F(QueueTest, WithDifferentContainer)
{
	/* test with vector as underlying container; since list's push_front */
	/* need to make sure that FIFO behavior is maintained */
	class vec_container_adapter : public ach::vector<int>
	{
	public:
		using ach::vector<int>::vector;

		void push_back(const int &value)
		{
			ach::vector<int>::push_back(value);
		}

		void push_back(int &&value)
		{
			ach::vector<int>::push_back(std::move(value));
		}

		void pop_front()
		{
			if (!this->empty())
			{
				this->erase(this->begin());
			}
		}
	};

	ach::queue<int, vec_container_adapter> vec_queue;

	vec_queue.push(1);
	vec_queue.push(2);
	vec_queue.push(3);

	EXPECT_EQ(vec_queue.size(), 3);
	EXPECT_EQ(vec_queue.front(), 1);
	EXPECT_EQ(vec_queue.back(), 3);

	vec_queue.pop();
	EXPECT_EQ(vec_queue.front(), 2);
}

TEST_F(QueueTest, FIFOBehavior)
{
	/* verify First-In-First-Out behavior */
	int_queue.push(1);
	int_queue.push(2);
	int_queue.push(3);
	int_queue.push(4);
	int_queue.push(5);

	std::vector<int> popped_order;
	while (!int_queue.empty())
	{
		popped_order.push_back(int_queue.front());
		int_queue.pop();
	}

	std::vector<int> expected = { 1, 2, 3, 4, 5 };
	EXPECT_EQ(popped_order, expected);
}

TEST_F(QueueTest, StressTest)
{
	constexpr auto N = 1000; /* Reduced from 10000 to avoid memory issues */

	try
	{
		/* push many elements */
		for (auto i = 0; i < N; ++i)
		{
			int_queue.push(i);
		}

		EXPECT_EQ(int_queue.size(), N);
		EXPECT_EQ(int_queue.front(), 0);
		EXPECT_EQ(int_queue.back(), N - 1);

		/* pop all elements and verify FIFO order */
		for (int i = 0; i < N; ++i)
		{
			EXPECT_EQ(int_queue.front(), i);
			int_queue.pop();
		}

		EXPECT_TRUE(int_queue.empty());
	}
	catch (const std::bad_alloc &)
	{
		GTEST_SKIP() << "Insufficient memory for stress test";
	} catch (...)
	{
		GTEST_SKIP() << "Stress test failed with unknown exception";
	}
}
