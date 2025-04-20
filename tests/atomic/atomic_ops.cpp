/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <array>
#include <thread>
#include <vector>
#include <acheron/__atomic/atomic.hpp>
#include <acheron/__atomic/atomic_ops.hpp>
#include <gtest/gtest.h>

class AtomicOpsTestFixture : public testing::Test
{
protected:
	static constexpr auto NUM_THREADS = 8;
	static constexpr auto ITERATIONS = 10000;
};

TEST_F(AtomicOpsTestFixture, PublishAndConsume)
{
	ach::atomic atom(0);

	publish_store(atom, 42);
	EXPECT_EQ(ach::consume_load(atom), 42);

	ach::atomic flag(false);
	auto result = 0;

	std::thread producer([&]()
	{
		publish_store(atom, 100);
		publish_store(flag, true);
	});

	std::thread consumer([&]()
	{
		while (!ach::consume_load(flag)) {}
		result = ach::consume_load(atom);
	});

	producer.join();
	consumer.join();

	EXPECT_EQ(result, 100);
}

TEST_F(AtomicOpsTestFixture, FetchAddAndSub)
{
	ach::atomic<int> atom(10);

	EXPECT_EQ(ach::fetch_add(atom, 5), 10);
	EXPECT_EQ(ach::consume_load(atom), 15);

	EXPECT_EQ(ach::fetch_sub(atom, 7), 15);
	EXPECT_EQ(ach::consume_load(atom), 8);

	ach::atomic counter(0);
	std::vector<std::thread> threads;

	threads.reserve(NUM_THREADS);
	for (int i = 0; i < NUM_THREADS; ++i)
	{
		threads.emplace_back([&counter]()
		{
			for (int j = 0; j < ITERATIONS; ++j)
				fetch_add(counter, 1);
		});
	}

	for (auto &t: threads)
	{
		t.join();
	}

	EXPECT_EQ(ach::consume_load(counter), NUM_THREADS * ITERATIONS);
}

TEST_F(AtomicOpsTestFixture, CompareExchange)
{
	ach::atomic atom(5);

	int expected = 5;
	bool success = compare_exchange(atom, expected, 10);
	EXPECT_TRUE(success);
	EXPECT_EQ(expected, 5);
	EXPECT_EQ(ach::consume_load(atom), 10);

	expected = 5;
	success = compare_exchange(atom, expected, 15);
	EXPECT_FALSE(success);
	EXPECT_EQ(expected, 10);
	EXPECT_EQ(ach::consume_load(atom), 10);
}

TEST_F(AtomicOpsTestFixture, Exchange)
{
	ach::atomic atom(20);

	EXPECT_EQ(ach::exchange(atom, 30), 20);
	EXPECT_EQ(ach::consume_load(atom), 30);
}

TEST_F(AtomicOpsTestFixture, Increment)
{
	ach::atomic atom(41);

	EXPECT_EQ(ach::increment(atom), 42);
	EXPECT_EQ(ach::consume_load(atom), 42);

	ach::atomic counter(0);
	std::vector<std::thread> threads;
	threads.reserve(NUM_THREADS);
	for (int i = 0; i < NUM_THREADS; ++i)
	{
		threads.emplace_back([&counter]()
		{
			for (int j = 0; j < ITERATIONS; ++j)
				increment(counter);
		});
	}

	for (auto &t: threads)
		t.join();

	EXPECT_EQ(ach::consume_load(counter), NUM_THREADS * ITERATIONS);
}

TEST_F(AtomicOpsTestFixture, DoubleCheckedInit)
{
	ach::atomic initialized(false);
	auto value = 0;

	double_checked_init(initialized, value, [](int &v)
	{
		v = 42;
	});
	EXPECT_EQ(value, 42);
	EXPECT_TRUE(ach::consume_load(initialized));

	double_checked_init(initialized, value, [](int &v)
	{
		v = 100;
	});
	EXPECT_EQ(value, 42); /* value should not change */

	ach::atomic init_flag(false);
	int shared_value = 0;
	ach::atomic init_counter(0);

	std::vector<std::thread> threads;
	threads.reserve(NUM_THREADS);
	for (int i = 0; i < NUM_THREADS; ++i)
	{
		threads.emplace_back([&]
		{
			double_checked_init(init_flag, shared_value, [&](int &v)
			{
				v = 123;
				fetch_add(init_counter, 1);
			});
		});
	}

	for (auto &t: threads)
	{
		t.join();
	}

	EXPECT_EQ(shared_value, 123);
	EXPECT_TRUE(ach::consume_load(init_flag));
	EXPECT_EQ(ach::consume_load(init_counter), 1);
}

TEST_F(AtomicOpsTestFixture, SpinWaitUntilEquals)
{
	ach::atomic atom(0);
	bool wait_completed = false;

	std::thread waiter([&]
	{
		spin_wait_until_equals(atom, 42);
		wait_completed = true;
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	EXPECT_FALSE(wait_completed);

	publish_store(atom, 42);
	waiter.join();

	EXPECT_TRUE(wait_completed);
}

TEST_F(AtomicOpsTestFixture, MessagePassing)
{
	struct Message
	{
		int data[4];
	};

	Message msg = {};
	ach::atomic<bool> ready(false);

	std::thread sender([&]()
	{
		msg.data[0] = 1;
		msg.data[1] = 2;
		msg.data[2] = 3;
		msg.data[3] = 4;

		publish_store(ready, true);
	});

	std::thread receiver([&]
	{
		while (!ach::consume_load(ready)) {} /* spin-wait til ready */
		EXPECT_EQ(msg.data[0], 1);
		EXPECT_EQ(msg.data[1], 2);
		EXPECT_EQ(msg.data[2], 3);
		EXPECT_EQ(msg.data[3], 4);
	});

	sender.join();
	receiver.join();
}

TEST_F(AtomicOpsTestFixture, DataRace)
{
	ach::atomic atom(0);
	std::array<int, NUM_THREADS> local_copies = {};

	std::vector<std::thread> writers;
	writers.reserve(NUM_THREADS);
	for (auto i = 0; i < NUM_THREADS; ++i)
	{
		writers.emplace_back([&atom, i]
		{
			publish_store(atom, i + 1);
		});
	}

	std::vector<std::thread> readers;
	readers.reserve(NUM_THREADS);
	for (int i = 0; i < NUM_THREADS; ++i)
	{
		readers.emplace_back([&atom, &local_copies, i]
		{
			local_copies[i] = consume_load(atom);
		});
	}

	for (auto &t: writers)
		t.join();

	for (auto &t: readers)
		t.join();

	/* can't really produce exact but all should be valid anyway anyhow(?) */
	for (int i = 0; i < NUM_THREADS; ++i)
	{
		EXPECT_GE(local_copies[i], 0);
		EXPECT_LE(local_copies[i], NUM_THREADS);
	}
}
