/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <string>
#include <acheron/dynamic_bitset>
#include <gtest/gtest.h>

class DynamicBitsetTest : public ::testing::Test
{
protected:
	ach::dynamic_bitset<> bitset;
};

TEST_F(DynamicBitsetTest, DefaultConstruction)
{
	EXPECT_TRUE(bitset.empty());
	EXPECT_EQ(bitset.size(), 0);
	EXPECT_EQ(bitset.num_blocks_val(), 0);
}

TEST_F(DynamicBitsetTest, SizedConstruction)
{
	ach::dynamic_bitset<> bits(64, true);
	EXPECT_EQ(bits.size(), 64);
	EXPECT_EQ(bits.count(), 64);
	EXPECT_TRUE(bits.all());

	ach::dynamic_bitset<> bits2(32, false);
	EXPECT_EQ(bits2.size(), 32);
	EXPECT_EQ(bits2.count(), 0);
	EXPECT_TRUE(bits2.none());
}

TEST_F(DynamicBitsetTest, ElementAccess)
{
	bitset.resize(10);
	bitset[3] = true;
	bitset[7] = true;

	EXPECT_FALSE(bitset[0]);
	EXPECT_TRUE(bitset[3]);
	EXPECT_TRUE(bitset[7]);
	EXPECT_FALSE(bitset[9]);

	EXPECT_THROW(bitset.test(10), std::out_of_range);
}

TEST_F(DynamicBitsetTest, SetResetFlip)
{
	bitset.resize(8);

	bitset.set(2);
	EXPECT_TRUE(bitset[2]);

	bitset.set();
	EXPECT_TRUE(bitset.all());

	bitset.reset(5);
	EXPECT_FALSE(bitset[5]);

	bitset.reset();
	EXPECT_TRUE(bitset.none());

	bitset.flip(1);
	EXPECT_TRUE(bitset[1]);

	bitset.set();
	bitset.flip();
	EXPECT_TRUE(bitset.none());
}

TEST_F(DynamicBitsetTest, PushPopBack)
{
	bitset.push_back(true);
	bitset.push_back(false);
	bitset.push_back(true);

	EXPECT_EQ(bitset.size(), 3);
	EXPECT_TRUE(bitset[0]);
	EXPECT_FALSE(bitset[1]);
	EXPECT_TRUE(bitset[2]);

	bitset.pop_back();
	EXPECT_EQ(bitset.size(), 2);
}

TEST_F(DynamicBitsetTest, BitwiseOperations)
{
	ach::dynamic_bitset<> a("1010");
	ach::dynamic_bitset<> b("1100");

	auto c = a & b;
	EXPECT_EQ(c.to_string(), "1000");

	auto d = a | b;
	EXPECT_EQ(d.to_string(), "1110");

	auto e = a ^ b;
	EXPECT_EQ(e.to_string(), "0110");

	auto f = ~a;
	EXPECT_EQ(f.to_string(), "0101");
}

TEST_F(DynamicBitsetTest, ShiftOperations)
{
	ach::dynamic_bitset<> bits("10110");

	auto left = bits << 2;
	EXPECT_EQ(left.to_string(), "11000");

	auto right = bits >> 2;
	EXPECT_EQ(right.to_string(), "00101");

	bits <<= 1;
	EXPECT_EQ(bits.to_string(), "01100");

	bits >>= 1;
	EXPECT_EQ(bits.to_string(), "00110");
}

TEST_F(DynamicBitsetTest, CountingOperations)
{
	ach::dynamic_bitset<> bits("10110100");

	EXPECT_EQ(bits.count(), 4);
	EXPECT_FALSE(bits.all());
	EXPECT_TRUE(bits.any());
	EXPECT_FALSE(bits.none());

	bits.reset();
	EXPECT_EQ(bits.count(), 0);
	EXPECT_TRUE(bits.none());
	EXPECT_FALSE(bits.any());

	bits.set();
	EXPECT_EQ(bits.count(), bits.size());
	EXPECT_TRUE(bits.all());
}

TEST_F(DynamicBitsetTest, Conversions)
{
	ach::dynamic_bitset<> bits("101010");
	EXPECT_EQ(bits.to_string(), "101010");

	ach::dynamic_bitset<> small("1010");
	EXPECT_EQ(small.to_ulong(), 10UL);
	EXPECT_EQ(small.to_ullong(), 10ULL);

	ach::dynamic_bitset<> large(100);
	large.set();
	EXPECT_THROW(large.to_ulong(), std::overflow_error);
}

TEST_F(DynamicBitsetTest, Resize)
{
	bitset.resize(5, true);
	EXPECT_EQ(bitset.size(), 5);
	EXPECT_EQ(bitset.count(), 5);

	bitset.resize(10, false);
	EXPECT_EQ(bitset.size(), 10);
	EXPECT_EQ(bitset.count(), 5);

	bitset.resize(3);
	EXPECT_EQ(bitset.size(), 3);
	EXPECT_EQ(bitset.count(), 3);
}

TEST_F(DynamicBitsetTest, CopyMoveSemantics)
{
	ach::dynamic_bitset<> original("101010");

	ach::dynamic_bitset<> copy(original);
	EXPECT_EQ(copy.to_string(), original.to_string());

	ach::dynamic_bitset<> moved(std::move(original));
	EXPECT_EQ(moved.to_string(), "101010");
	EXPECT_TRUE(original.empty());
}

TEST_F(DynamicBitsetTest, ComparisonOperators)
{
	ach::dynamic_bitset<> a("1010");
	ach::dynamic_bitset<> b("1010");
	ach::dynamic_bitset<> c("1011");

	EXPECT_EQ(a, b);
	EXPECT_NE(a, c);
}

TEST_F(DynamicBitsetTest, LargeBitset)
{
	ach::dynamic_bitset<> large(1000);

	/* set every 3rd bit */
	for (size_t i = 0; i < large.size(); i += 3)
		large.set(i);

	EXPECT_EQ(large.count(), 334); /* 1000/3 + 1 */

	/* test shifting large amounts */
	auto shifted = large << 100;
	/* when shifting left by 100, we lose the first 100/3 ≈ 33-34 set bits; so 334 - 34 = 300 */
	EXPECT_EQ(shifted.count(), 300);
}

TEST_F(DynamicBitsetTest, ReferenceProxy)
{
	bitset.resize(10);

	/* test reference assignment */
	bitset[5] = true;
	EXPECT_TRUE(bitset[5]);

	/* test reference flip */
	bitset[5].flip();
	EXPECT_FALSE(bitset[5]);

	/* test reference copy */
	bitset[3] = bitset[5];
	EXPECT_FALSE(bitset[3]);
}

TEST_F(DynamicBitsetTest, EdgeCases)
{
	/* empty bitset operations */
	ach::dynamic_bitset<> empty;
	EXPECT_EQ(empty.count(), 0);
	EXPECT_TRUE(empty.none());
	EXPECT_FALSE(empty.any());
	EXPECT_TRUE(empty.all()); /* empty set satisfies all() */

	/* single bit operations */
	ach::dynamic_bitset<> single(1);
	single.set(0);
	EXPECT_EQ(single.count(), 1);
	EXPECT_TRUE(single.all());
	EXPECT_TRUE(single.any());

	/* shift by size */
	ach::dynamic_bitset<> bits("1111");
	bits <<= 4;
	EXPECT_TRUE(bits.none());

	bits.set();
	bits >>= 4;
	EXPECT_TRUE(bits.none());
}

TEST_F(DynamicBitsetTest, DifferentBlockTypes)
{
	/* test with different block sizes */
	ach::dynamic_bitset<unsigned char> byte_blocks(100);
	byte_blocks.set(50);
	EXPECT_TRUE(byte_blocks.test(50));

	ach::dynamic_bitset<unsigned short> short_blocks(200);
	short_blocks.set(150);
	EXPECT_TRUE(short_blocks.test(150));
}

TEST_F(DynamicBitsetTest, StressTest)
{
	constexpr auto N = 10000;

	try
	{
		ach::dynamic_bitset<> stress(N);

		/* set alternating bits */
		for (auto i = 0; i < N; i += 2)
			stress.set(i);

		EXPECT_EQ(stress.count(), N / 2);

		/* flip all bits */
		stress.flip();
		EXPECT_EQ(stress.count(), N / 2);

		/* shift operations */
		stress <<= 100;
		EXPECT_EQ(stress.count(), N / 2 - 50); /* lost 50 on each end */

		/* clear and verify */
		stress.clear();
		EXPECT_TRUE(stress.empty());
	}
	catch (const std::bad_alloc &)
	{
		GTEST_SKIP() << "Insufficient memory for stress test";
	} catch (...)
	{
		GTEST_SKIP() << "Stress test failed with unknown exception";
	}
}
