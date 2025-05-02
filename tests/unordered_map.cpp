/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <acheron/unordered_map>
#include <gtest/gtest.h>
#include <string>
#include <vector>

class UnorderedMapTest : public ::testing::Test
{
protected:
	ach::unordered_map<int, std::string> int_map;
	ach::unordered_map<std::string, int> string_map;
};

TEST_F(UnorderedMapTest, DefaultConstruction)
{
	EXPECT_TRUE(int_map.empty());
	EXPECT_EQ(int_map.size(), 0);
	EXPECT_GT(int_map.bucket_count_val(), 0);
}

TEST_F(UnorderedMapTest, InsertAndFind)
{
	auto result = int_map.insert({1, "one"});
	EXPECT_TRUE(result.second);
	EXPECT_EQ(result.first->first, 1);
	EXPECT_EQ(result.first->second, "one");

	auto found = int_map.find(1);
	EXPECT_NE(found, int_map.end());
	EXPECT_EQ(found->second, "one");

	found = int_map.find(2);
	EXPECT_EQ(found, int_map.end());
}

TEST_F(UnorderedMapTest, InsertDuplicate)
{
	int_map.insert({1, "one"});
	auto result = int_map.insert({1, "uno"});
	EXPECT_FALSE(result.second);
	EXPECT_EQ(result.first->second, "one");
}

TEST_F(UnorderedMapTest, SubscriptOperator)
{
	int_map[1] = "one";
	int_map[2] = "two";

	EXPECT_EQ(int_map[1], "one");
	EXPECT_EQ(int_map[2], "two");
	EXPECT_EQ(int_map.size(), 2);

	/* accessing non-existent key creates default-initialized value */
	std::string& three = int_map[3];
	EXPECT_EQ(three, "");
	EXPECT_EQ(int_map.size(), 3);
}

TEST_F(UnorderedMapTest, At)
{
	int_map.insert({1, "one"});

	EXPECT_EQ(int_map.at(1), "one");
	EXPECT_THROW(int_map.at(2), std::out_of_range);
}

TEST_F(UnorderedMapTest, EmplaceAndTryEmplace)
{
	auto result = int_map.emplace(1, "one");
	EXPECT_TRUE(result.second);

	result = int_map.emplace(1, "uno");
	EXPECT_FALSE(result.second);
	EXPECT_EQ(result.first->second, "one");

	auto try_result = int_map.try_emplace(2, "two");
	EXPECT_TRUE(try_result.second);

	try_result = int_map.try_emplace(2, "dos");
	EXPECT_FALSE(try_result.second);
	EXPECT_EQ(try_result.first->second, "two");
}

TEST_F(UnorderedMapTest, InsertOrAssign)
{
	int_map.insert_or_assign(1, "one");
	EXPECT_EQ(int_map[1], "one");

	int_map.insert_or_assign(1, "uno");
	EXPECT_EQ(int_map[1], "uno");
}

TEST_F(UnorderedMapTest, Erase)
{
	int_map = {{1, "one"}, {2, "two"}, {3, "three"}};

	size_t erased = int_map.erase(2);
	EXPECT_EQ(erased, 1);
	EXPECT_EQ(int_map.size(), 2);
	EXPECT_EQ(int_map.find(2), int_map.end());

	erased = int_map.erase(4);
	EXPECT_EQ(erased, 0);
}

TEST_F(UnorderedMapTest, EraseIterator)
{
	int_map = {{1, "one"}, {2, "two"}, {3, "three"}};

	auto it = int_map.find(2);
	EXPECT_NE(it, int_map.end());

	auto next = int_map.erase(it);
	EXPECT_EQ(int_map.size(), 2);
	EXPECT_EQ(int_map.find(2), int_map.end());
}

TEST_F(UnorderedMapTest, Clear)
{
	int_map = {{1, "one"}, {2, "two"}, {3, "three"}};
	EXPECT_FALSE(int_map.empty());

	int_map.clear();
	EXPECT_TRUE(int_map.empty());
	EXPECT_EQ(int_map.size(), 0);
}

TEST_F(UnorderedMapTest, Contains)
{
	int_map = {{1, "one"}, {2, "two"}};

	EXPECT_TRUE(int_map.contains(1));
	EXPECT_TRUE(int_map.contains(2));
	EXPECT_FALSE(int_map.contains(3));
}

TEST_F(UnorderedMapTest, Count)
{
	int_map = {{1, "one"}, {2, "two"}};

	EXPECT_EQ(int_map.count(1), 1);
	EXPECT_EQ(int_map.count(3), 0);
}

TEST_F(UnorderedMapTest, EqualRange)
{
	int_map = {{1, "one"}, {2, "two"}};

	auto range = int_map.equal_range(1);
	EXPECT_NE(range.first, int_map.end());
	EXPECT_EQ(range.first->first, 1);

	size_t count = std::distance(range.first, range.second);
	EXPECT_EQ(count, 1);
}

TEST_F(UnorderedMapTest, Iterators)
{
	int_map = {{1, "one"}, {2, "two"}, {3, "three"}};

	std::vector<int> keys;
	for (const auto& [key, value] : int_map)
	{
		keys.push_back(key);
	}

	std::sort(keys.begin(), keys.end());
	EXPECT_EQ(keys, (std::vector<int>{1, 2, 3}));
}

TEST_F(UnorderedMapTest, BucketInterface)
{
	int_map = {{1, "one"}, {2, "two"}};

	EXPECT_GT(int_map.bucket_count_val(), 0);

	size_t bucket_1 = int_map.bucket(1);
	EXPECT_LT(bucket_1, int_map.bucket_count_val());

	size_t bucket_size = int_map.bucket_size(bucket_1);
	EXPECT_GE(bucket_size, 0);
}

TEST_F(UnorderedMapTest, LoadFactor)
{
	int_map = {{1, "one"}, {2, "two"}};

	float load = int_map.load_factor();
	EXPECT_GE(load, 0.0f);
	EXPECT_LE(load, int_map.max_load_factor());
}

TEST_F(UnorderedMapTest, RehashAndReserve)
{
	size_t initial_buckets = int_map.bucket_count_val();

	int_map.rehash(100);
	EXPECT_GE(int_map.bucket_count_val(), 100);

	int_map.reserve(50);
	EXPECT_GE(int_map.bucket_count_val(), 50 / int_map.max_load_factor());
}

TEST_F(UnorderedMapTest, CopyConstruction)
{
	int_map = {{1, "one"}, {2, "two"}, {3, "three"}};

	ach::unordered_map<int, std::string> copy(int_map);
	EXPECT_EQ(copy.size(), int_map.size());

	for (const auto& [key, value] : int_map)
	{
		EXPECT_TRUE(copy.contains(key));
		EXPECT_EQ(copy[key], value);
	}
}

TEST_F(UnorderedMapTest, MoveConstruction)
{
	int_map = {{1, "one"}, {2, "two"}, {3, "three"}};
	size_t original_size = int_map.size();

	ach::unordered_map<int, std::string> moved(std::move(int_map));
	EXPECT_EQ(moved.size(), original_size);
	EXPECT_TRUE(int_map.empty());
}

TEST_F(UnorderedMapTest, CopyAssignment)
{
	int_map = {{1, "one"}, {2, "two"}};
	ach::unordered_map<int, std::string> other = {{3, "three"}};

	other = int_map;
	EXPECT_EQ(other.size(), int_map.size());
	EXPECT_TRUE(other.contains(1));
	EXPECT_TRUE(other.contains(2));
	EXPECT_FALSE(other.contains(3));
}

TEST_F(UnorderedMapTest, MoveAssignment)
{
	int_map = {{1, "one"}, {2, "two"}};
	ach::unordered_map<int, std::string> other = {{3, "three"}};

	other = std::move(int_map);
	EXPECT_EQ(other.size(), 2);
	EXPECT_TRUE(int_map.empty());
	EXPECT_TRUE(other.contains(1));
	EXPECT_TRUE(other.contains(2));
}

TEST_F(UnorderedMapTest, InitializerListConstruction)
{
	ach::unordered_map<int, std::string> map = {{1, "one"}, {2, "two"}, {3, "three"}};
	EXPECT_EQ(map.size(), 3);
	EXPECT_EQ(map[1], "one");
	EXPECT_EQ(map[2], "two");
	EXPECT_EQ(map[3], "three");
}

TEST_F(UnorderedMapTest, Swap)
{
	int_map = {{1, "one"}, {2, "two"}};
	ach::unordered_map<int, std::string> other = {{3, "three"}, {4, "four"}};

	int_map.swap(other);

	EXPECT_EQ(int_map.size(), 2);
	EXPECT_TRUE(int_map.contains(3));
	EXPECT_TRUE(int_map.contains(4));

	EXPECT_EQ(other.size(), 2);
	EXPECT_TRUE(other.contains(1));
	EXPECT_TRUE(other.contains(2));
}

TEST_F(UnorderedMapTest, ComparisonOperators)
{
	ach::unordered_map<int, std::string> map1 = {{1, "one"}, {2, "two"}};
	ach::unordered_map<int, std::string> map2 = {{1, "one"}, {2, "two"}};
	ach::unordered_map<int, std::string> map3 = {{1, "one"}, {3, "three"}};

	EXPECT_EQ(map1, map2);
	EXPECT_NE(map1, map3);
}

TEST_F(UnorderedMapTest, MaxLoadFactor)
{
	float default_mlf = int_map.max_load_factor();
	EXPECT_GT(default_mlf, 0.0f);

	int_map.max_load_factor(0.5f);
	EXPECT_EQ(int_map.max_load_factor(), 0.5f);
}

TEST_F(UnorderedMapTest, Observers)
{
	auto hasher = int_map.hash_function();
	EXPECT_EQ(hasher(42), std::hash<int>{}(42));

	auto key_eq = int_map.key_eq();
	EXPECT_TRUE(key_eq(1, 1));
	EXPECT_FALSE(key_eq(1, 2));
}

TEST_F(UnorderedMapTest, RobinHoodBehavior)
{
	/* test Robin Hood probing by inserting keys that hash to same bucket */
	for (int i = 0; i < 100; ++i)
	{
		int_map[i] = std::to_string(i);
	}

	EXPECT_EQ(int_map.size(), 100);

	/* verify all inserted elements can be found */
	for (int i = 0; i < 100; ++i)
	{
		EXPECT_TRUE(int_map.contains(i));
		EXPECT_EQ(int_map[i], std::to_string(i));
	}
}

TEST_F(UnorderedMapTest, StressTest)
{
	const int N = 10000;

	/* insert many elements */
	for (int i = 0; i < N; ++i)
	{
		int_map[i] = std::to_string(i);
	}

	EXPECT_EQ(int_map.size(), N);

	/* erase half of them */
	for (int i = 0; i < N; i += 2)
	{
		int_map.erase(i);
	}

	EXPECT_EQ(int_map.size(), N / 2);

	/* verify remaining elements */
	for (int i = 1; i < N; i += 2)
	{
		EXPECT_TRUE(int_map.contains(i));
		EXPECT_EQ(int_map[i], std::to_string(i));
	}
}
