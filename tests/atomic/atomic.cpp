/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <acheron/__atomic/atomic.hpp>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

class AtomicTestFixture : public testing::Test
{
protected:
    ach::atomic<int> atomic_int;
    static constexpr auto NUM_THREADS = 8;
    static constexpr auto ITERATIONS = 10000;
};

TEST_F(AtomicTestFixture, BasicOperations)
{
    atomic_int.store(42, ach::memory_order::relaxed);
    EXPECT_EQ(atomic_int.load(ach::memory_order::relaxed), 42);

    atomic_int.store(100, ach::memory_order::release);
    EXPECT_EQ(atomic_int.load(ach::memory_order::acquire), 100);
}

TEST_F(AtomicTestFixture, MessagePassing)
{
    ach::atomic data = 0;
    ach::atomic flag(false);

    std::thread sender([&]
    {
        data.store(42, ach::memory_order::relaxed);
        ach::release_fence();
        flag.store(true, ach::memory_order::release);
    });

    std::thread receiver([&]
    {
        /* spin wait here */
        while (!flag.load(ach::memory_order::acquire)) {}
        ach::acquire_fence();
        EXPECT_EQ(data.load(ach::memory_order::relaxed), 42);
    });

    sender.join();
    receiver.join();
}

TEST_F(AtomicTestFixture, StoreBuffer)
{
    ach::atomic x(0);
    ach::atomic y(0);
    auto r1 = 0;
    auto r2 = 0;

    std::thread t1([&]
    {
        x.store(1, ach::memory_order::release);
        r1 = y.load(ach::memory_order::acquire);
    });

    std::thread t2([&]
    {
        y.store(1, ach::memory_order::release);
        r2 = x.load(ach::memory_order::acquire);
    });

    t1.join();
    t2.join();
    EXPECT_TRUE(r1 == 1 || r2 == 1);
}

TEST_F(AtomicTestFixture, ConcurrentIncrement)
{
    atomic_int.store(0);

    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads.emplace_back([this]
        {
            for (int j = 0; j < ITERATIONS; j++)
                atomic_int.fetch_add(1, ach::memory_order::acq_rel);
        });
    }

    for (auto& t : threads)
        t.join();

    EXPECT_EQ(atomic_int.load(), NUM_THREADS * ITERATIONS);
}

TEST_F(AtomicTestFixture, ABATest)
{
    ach::atomic val(1);
    ach::atomic t1_ready(false);
    ach::atomic t2_first_done(false);
    ach::atomic t2_second_done(false);

    std::thread t1([&]
    {
        auto expected = 1;
        t1_ready.store(true, ach::memory_order::release);
        while (!t2_second_done.load(ach::memory_order::acquire))
            std::this_thread::yield();

        const bool success = val.compare_exchange_strong(expected, 3,
                                                  ach::memory_order::acq_rel);
        EXPECT_TRUE(success);
        EXPECT_EQ(val.load(), 3);
    });

    std::thread t2([&]
    {
        while (!t1_ready.load(ach::memory_order::acquire))
            std::this_thread::yield();

        val.store(2, ach::memory_order::release);
        t2_first_done.store(true, ach::memory_order::release);

        val.store(1, ach::memory_order::release); /* ABA: changed back */
        t2_second_done.store(true, ach::memory_order::release);
    });

    t1.join();
    t2.join();
}
