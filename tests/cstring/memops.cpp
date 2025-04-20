/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <algorithm>
#include <cstring>
#include <random>
#include <vector>
#include <acheron/cstring>
#include <gtest/gtest.h>

static constexpr std::size_t BLOCK_SIZE = 4096;
static constexpr std::size_t MISALIGNED_OFFSET = 1;

class MemoryTestFixture : public testing::Test
{
protected:
	std::vector<uint8_t> block1;
	std::vector<uint8_t> block2;

	void SetUp() override
	{
		block1.resize(BLOCK_SIZE);
		block2.resize(BLOCK_SIZE);

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 255);

		std::ranges::generate(block1, [&]
		{
			return dis(gen);
		});

		std::ranges::generate(block2, [&]
		{
			return dis(gen);
		});
	}
};

TEST_F(MemoryTestFixture, MemcmpTest)
{
	/* aligned & identical */
	std::memcpy(block2.data(), block1.data(), BLOCK_SIZE);
	EXPECT_EQ(0, ach::memcmp(block1.data(), block2.data(), BLOCK_SIZE));

	/* aligned & different */
	block2[BLOCK_SIZE / 2] = ~block1[BLOCK_SIZE / 2];
	EXPECT_NE(0, ach::memcmp(block1.data(), block2.data(), BLOCK_SIZE));

	/* small sizes */
	EXPECT_EQ(0, ach::memcmp(block1.data(), block1.data(), 7));

	/* misaligned */
	std::memcpy(block2.data(), block1.data(), BLOCK_SIZE);
	EXPECT_EQ(0, ach::memcmp(block1.data() + MISALIGNED_OFFSET,
							  block2.data() + MISALIGNED_OFFSET,
							  BLOCK_SIZE - MISALIGNED_OFFSET * 2));

	/* differences at start, middle, end */
	std::memcpy(block2.data(), block1.data(), BLOCK_SIZE);
	block2[0] = ~block1[0];
	EXPECT_NE(0, ach::memcmp(block1.data(), block2.data(), BLOCK_SIZE));
        
	std::memcpy(block2.data(), block1.data(), BLOCK_SIZE);
	block2[BLOCK_SIZE / 2] = ~block1[BLOCK_SIZE / 2];
	EXPECT_NE(0, ach::memcmp(block1.data(), block2.data(), BLOCK_SIZE));
        
	std::memcpy(block2.data(), block1.data(), BLOCK_SIZE);
	block2[BLOCK_SIZE - 1] = ~block1[BLOCK_SIZE - 1];
	EXPECT_NE(0, ach::memcmp(block1.data(), block2.data(), BLOCK_SIZE));
	
	/* possible edges */
	EXPECT_EQ(0, ach::memcmp(block1.data(), block1.data(), 0));
}

TEST_F(MemoryTestFixture, MemcpyTest)
{
	/* aligned & various */
	std::memset(block2.data(), 0, BLOCK_SIZE);
	ach::memcpy(block2.data(), block1.data(), BLOCK_SIZE);
	EXPECT_EQ(0, std::memcmp(block1.data(), block2.data(), BLOCK_SIZE));

	/* small */
	std::memset(block2.data(), 0, BLOCK_SIZE);
	ach::memcpy(block2.data(), block1.data(), 7);
	EXPECT_EQ(0, std::memcmp(block1.data(), block2.data(), 7));

	/* misaligned */
	std::memset(block2.data(), 0, BLOCK_SIZE);
	ach::memcpy(block2.data(), block1.data() + MISALIGNED_OFFSET, BLOCK_SIZE - MISALIGNED_OFFSET);
	EXPECT_EQ(0, std::memcmp(block1.data() + MISALIGNED_OFFSET, block2.data(), BLOCK_SIZE - MISALIGNED_OFFSET));

	/* misaligned dest */
	std::memset(block2.data(), 0, BLOCK_SIZE);
	ach::memcpy(block2.data() + MISALIGNED_OFFSET, block1.data(), BLOCK_SIZE - MISALIGNED_OFFSET);
	EXPECT_EQ(0, std::memcmp(block1.data(), block2.data() + MISALIGNED_OFFSET, BLOCK_SIZE - MISALIGNED_OFFSET));

	/* misaligned dest & src */
	std::memset(block2.data(), 0, BLOCK_SIZE);
	ach::memcpy(block2.data() + MISALIGNED_OFFSET, block1.data() + MISALIGNED_OFFSET, BLOCK_SIZE - MISALIGNED_OFFSET * 2);
	EXPECT_EQ(0, std::memcmp(block1.data() + MISALIGNED_OFFSET, block2.data() + MISALIGNED_OFFSET, BLOCK_SIZE - MISALIGNED_OFFSET * 2));

	/* zero size */
	const std::uint8_t before = block2[0];
	ach::memcpy(block2.data(), block1.data(), 0);
	EXPECT_EQ(before, block2[0]); /* note: nothing should change */
}

TEST_F(MemoryTestFixture, MemmoveTest)
{
	/* note: non-overlapping regions should behave like memcpy */
	std::memset(block2.data(), 0, BLOCK_SIZE);
	ach::memmove(block2.data(), block1.data(), BLOCK_SIZE);
	EXPECT_EQ(0, std::memcmp(block1.data(), block2.data(), BLOCK_SIZE));
        
	/* forward move; overlapped area */
	constexpr size_t OVERLAP = 100;
	std::memcpy(block2.data(), block1.data(), BLOCK_SIZE);
	ach::memmove(block2.data() + OVERLAP, block2.data(), BLOCK_SIZE - OVERLAP);
	EXPECT_EQ(0, std::memcmp(block1.data(), block2.data(), OVERLAP));
	EXPECT_EQ(0, std::memcmp(block1.data(), block2.data() + OVERLAP, BLOCK_SIZE - OVERLAP));
        
	/* backward move; overlapping region */
	std::memcpy(block2.data(), block1.data(), BLOCK_SIZE);
	ach::memmove(block2.data(), block2.data() + OVERLAP, BLOCK_SIZE - OVERLAP);
	EXPECT_EQ(0, std::memcmp(block1.data() + OVERLAP, block2.data(), BLOCK_SIZE - OVERLAP));

	/* overlapping move */
	std::memcpy(block2.data(), block1.data(), BLOCK_SIZE);
	ach::memmove(block2.data() + 3, block2.data(), 7);
	EXPECT_EQ(0, std::memcmp(block1.data(), block2.data() + 3, 7));
	
	/* self-move */
	std::memcpy(block2.data(), block1.data(), BLOCK_SIZE);
	ach::memmove(block2.data(), block2.data(), BLOCK_SIZE);
	EXPECT_EQ(0, std::memcmp(block1.data(), block2.data(), BLOCK_SIZE));
        
	/* zero size */
	uint8_t before = block2[0];
	ach::memmove(block2.data(), block1.data(), 0);
	EXPECT_EQ(before, block2[0]); /* noop */
}

TEST_F(MemoryTestFixture, MemsetTest)
{
	constexpr uint8_t TEST_VALUE = 0xA5;

	/* aligned & various */
	ach::memset(block1.data(), TEST_VALUE, BLOCK_SIZE);
	for (size_t i = 0; i < BLOCK_SIZE; i++)
	{
		EXPECT_EQ(TEST_VALUE, block1[i]);
	}

	/* small */
	std::memset(block1.data(), 0, BLOCK_SIZE);
	ach::memset(block1.data(), TEST_VALUE, 7);
	for (size_t i = 0; i < 7; i++)
	{
		EXPECT_EQ(TEST_VALUE, block1[i]);
	}
	EXPECT_EQ(0, block1[7]); /* not much */
        
	/* misaligned pointer */
	std::memset(block1.data(), 0, BLOCK_SIZE);
	ach::memset(block1.data() +MISALIGNED_OFFSET, TEST_VALUE, BLOCK_SIZE -MISALIGNED_OFFSET);
	EXPECT_EQ(0, block1[0]);
	for (size_t i =MISALIGNED_OFFSET; i < BLOCK_SIZE; i++)
	{
		EXPECT_EQ(TEST_VALUE, block1[i]);
	}
        
	/* non-byte-sized pattern */
	constexpr auto FULL_PATTERN = 0x12345678;
	std::memset(block1.data(), 0, BLOCK_SIZE);
	ach::memset(block1.data(), FULL_PATTERN, BLOCK_SIZE);
	for (size_t i = 0; i < BLOCK_SIZE; i++)
	{
		/* should truncate to uint8_t */
		EXPECT_EQ(static_cast<uint8_t>(FULL_PATTERN), block1[i]);
	}
        
	/* edge: zero size */
	std::memset(block1.data(), 0, BLOCK_SIZE);
	block1[0] = TEST_VALUE;
	ach::memset(block1.data() + 1, FULL_PATTERN, 0);
	EXPECT_EQ(TEST_VALUE, block1[0]);
	EXPECT_EQ(0, block1[1]);
}
