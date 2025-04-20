/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <algorithm>
#include <array>
#include <list>
#include <random>
#include <vector>
#include <acheron/__memory/allocator.hpp>
#include <gtest/gtest.h>

class AllocatorTestFixture : public testing::Test
{
protected:
    ach::allocator<int> int_allocator;
    static constexpr auto NUM_THREADS = 8;
    static constexpr auto ITERATIONS = 1000;
    static constexpr auto SMALL_ALLOC_SIZE = 64 / sizeof(int);
    static constexpr auto MEDIUM_ALLOC_SIZE = 512 / sizeof(int);
    static constexpr auto LARGE_ALLOC_SIZE = 2048 / sizeof(int);
};

TEST_F(AllocatorTestFixture, BasicAllocation)
{
    int *ptr = int_allocator.allocate(1);
    ASSERT_NE(ptr, nullptr);

    int_allocator.construct(ptr, 42);
    EXPECT_EQ(*ptr, 42);

    int_allocator.destroy(ptr);
    int_allocator.deallocate(ptr, 1);
}

TEST_F(AllocatorTestFixture, ZeroSizeAllocation)
{
    int *ptr = int_allocator.allocate(0);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(AllocatorTestFixture, MultipleAllocations)
{
    std::vector<int *> pointers;
    for (auto i = 0; i < ITERATIONS; ++i)
    {
        int *ptr = int_allocator.allocate(1);
        ASSERT_NE(ptr, nullptr);
        int_allocator.construct(ptr, i);
        pointers.push_back(ptr);
    }

    for (int i = 0; i < ITERATIONS; ++i)
    {
        EXPECT_EQ(*pointers[i], i);
    }

    /* FIFO dealloc */
    for (int i = ITERATIONS - 1; i >= 0; --i)
    {
        int_allocator.destroy(pointers[i]);
        int_allocator.deallocate(pointers[i], 1);
    }
}

TEST_F(AllocatorTestFixture, DifferentSizeAllocations)
{
    int *small_ptr = int_allocator.allocate(SMALL_ALLOC_SIZE);
    ASSERT_NE(small_ptr, nullptr);

    int *medium_ptr = int_allocator.allocate(MEDIUM_ALLOC_SIZE);
    ASSERT_NE(medium_ptr, nullptr);

    int *large_ptr = int_allocator.allocate(LARGE_ALLOC_SIZE);
    ASSERT_NE(large_ptr, nullptr);

    for (size_t i = 0; i < SMALL_ALLOC_SIZE; ++i)
        int_allocator.construct(small_ptr + i, 1);

    for (size_t i = 0; i < MEDIUM_ALLOC_SIZE; ++i)
        int_allocator.construct(medium_ptr + i, 2);

    for (size_t i = 0; i < LARGE_ALLOC_SIZE; ++i)
        int_allocator.construct(large_ptr + i, 3);

    for (size_t i = 0; i < SMALL_ALLOC_SIZE; ++i)
        EXPECT_EQ(small_ptr[i], 1);

    for (size_t i = 0; i < MEDIUM_ALLOC_SIZE; ++i)
        EXPECT_EQ(medium_ptr[i], 2);

    for (size_t i = 0; i < LARGE_ALLOC_SIZE; ++i)
        EXPECT_EQ(large_ptr[i], 3);

    for (size_t i = 0; i < SMALL_ALLOC_SIZE; ++i)
        int_allocator.destroy(small_ptr + i);
    int_allocator.deallocate(small_ptr, SMALL_ALLOC_SIZE);

    for (size_t i = 0; i < MEDIUM_ALLOC_SIZE; ++i)
        int_allocator.destroy(medium_ptr + i);
    int_allocator.deallocate(medium_ptr, MEDIUM_ALLOC_SIZE);

    for (size_t i = 0; i < LARGE_ALLOC_SIZE; ++i)
        int_allocator.destroy(large_ptr + i);
    int_allocator.deallocate(large_ptr, LARGE_ALLOC_SIZE);
}

TEST_F(AllocatorTestFixture, DifferentTypeAllocator)
{
    ach::allocator<double> double_allocator;
    double *ptr = double_allocator.allocate(10);
    ASSERT_NE(ptr, nullptr);
    for (int i = 0; i < 10; ++i)
        double_allocator.construct(ptr + i, i * 1.5);

    for (int i = 0; i < 10; ++i)
    {
        EXPECT_DOUBLE_EQ(ptr[i], i * 1.5);
    }

    for (int i = 0; i < 10; ++i)
        double_allocator.destroy(ptr + i);
    double_allocator.deallocate(ptr, 10);
}

struct NonTrivialType
{
    std::string data;
    int value;
    NonTrivialType() : data("test"), value(42) {}
    ~NonTrivialType() {}
};

TEST_F(AllocatorTestFixture, NonTrivialTypeTest)
{
    ach::allocator<NonTrivialType> nt_alloc;

    NonTrivialType* ptr = nt_alloc.allocate(1);
    nt_alloc.construct(ptr, NonTrivialType());

    EXPECT_EQ(ptr->data, "test");
    EXPECT_EQ(ptr->value, 42);

    nt_alloc.destroy(ptr);
    nt_alloc.deallocate(ptr, 1);
}

TEST_F(AllocatorTestFixture, MixedSizeAllocationDeallocations)
{
    struct AllocationRecord
    {
        void* ptr;
        size_t size;
    };

    std::vector<AllocationRecord> allocations;
    const std::array<size_t, 10> sizes = { 1, 2, 4, 8, 16, 24, 32, 48, 64, 96 };

    for (const size_t size : sizes)
    {
        for (auto i = 0; i < 10; ++i)
        {
            int* ptr = int_allocator.allocate(size);
            if (!ptr)
            {
                std::cout << "failed to allocate " << size << " ints" << std::endl;
                continue;
            }

            int_allocator.construct(ptr, 0xDEADBEEF);
            if (size > 1)
                int_allocator.construct(ptr + size - 1, 0xDEADC0DE);

            allocations.push_back({ ptr, size });
        }
    }

    for (const auto&[ptr, size] : allocations)
    {
        const auto p = static_cast<int*>(ptr);
        EXPECT_EQ(p[0], 0xDEADBEEF);
        if (size > 1)
        {
            EXPECT_EQ(p[size - 1], 0xDEADC0DE);
        }
    }

    std::mt19937 rng(42);
    std::ranges::shuffle(allocations, rng);
    for (const auto&[ptr, size] : allocations)
    {
        auto p = static_cast<int*>(ptr);
        int_allocator.destroy(p);

        if (size > 1)
            int_allocator.destroy(p + size - 1);
        int_allocator.deallocate(p, size);
    }
}

template<typename T>
struct TestContainer
{
    using value_type = T;
    using allocator_type = ach::allocator<T>;

    allocator_type alloc;
    T *data;
    size_t size;

    explicit TestContainer(const allocator_type &a = allocator_type()) : alloc(a), data(nullptr), size(0) {}

    TestContainer(const TestContainer &other) : alloc(std::allocator_traits<allocator_type>::
                                                    select_on_container_copy_construction(other.alloc)),
                                                data(nullptr), size(other.size)
    {
        if (size > 0)
        {
            data = alloc.allocate(size);
            for (size_t i = 0; i < size; ++i)
                alloc.construct(data + i, other.data[i]);
        }
    }

    ~TestContainer()
    {
        if (data)
        {
            for (size_t i = 0; i < size; ++i)
                alloc.destroy(data + i);
            alloc.deallocate(data, size);
        }
    }

    [[nodiscard]] allocator_type get_allocator() const
    {
        return alloc;
    }
};

TEST_F(AllocatorTestFixture, AllocatorPropagation)
{
    const TestContainer<int> c1;
    const TestContainer<int> &c2(c1);

    EXPECT_EQ(c1.get_allocator() == c2.get_allocator(), true);
    EXPECT_TRUE(c1.get_allocator() == c2.get_allocator());
}
