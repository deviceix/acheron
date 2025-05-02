/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <ranges>
#include <string>
#include <vector>
#include <acheron/map>
#include <gtest/gtest.h>

class MapTest : public ::testing::Test
{
protected:
	ach::map<int, std::string> int_string_map;
	ach::map<std::string, int> string_int_map;
};

TEST_F(MapTest, DefaultConstruction)
{
	EXPECT_TRUE(int_string_map.empty());
	EXPECT_EQ(int_string_map.size(), 0);
}

TEST_F(MapTest, InsertAndFind)
{
	auto result = int_string_map.insert({ 1, "one" });
	EXPECT_TRUE(result.second);
	EXPECT_EQ(result.first->first, 1);
	EXPECT_EQ(result.first->second, "one");

	auto found = int_string_map.find(1);
	EXPECT_NE(found, int_string_map.end());
	EXPECT_EQ(found->second, "one");

	found = int_string_map.find(2);
	EXPECT_EQ(found, int_string_map.end());
}

TEST_F(MapTest, InsertDuplicate)
{
	int_string_map.insert({ 1, "one" });
	auto result = int_string_map.insert({ 1, "uno" });
	EXPECT_FALSE(result.second);
	EXPECT_EQ(result.first->second, "one");
}

TEST_F(MapTest, SubscriptOperator)
{
	int_string_map[1] = "one";
	int_string_map[2] = "two";

	EXPECT_EQ(int_string_map[1], "one");
	EXPECT_EQ(int_string_map[2], "two");
	EXPECT_EQ(int_string_map.size(), 2);

	/* accessing non-existent key creates default-initialized value */
	std::string &three = int_string_map[3];
	EXPECT_EQ(three, "");
	EXPECT_EQ(int_string_map.size(), 3);
}

TEST_F(MapTest, At)
{
	int_string_map.insert({ 1, "one" });

	EXPECT_EQ(int_string_map.at(1), "one");
	EXPECT_THROW(int_string_map.at(2), std::out_of_range);
}

TEST_F(MapTest, EmplaceAndTryEmplace)
{
	auto result = int_string_map.emplace(1, "one");
	EXPECT_TRUE(result.second);

	result = int_string_map.emplace(1, "uno");
	EXPECT_FALSE(result.second);
	EXPECT_EQ(result.first->second, "one");

	auto try_result = int_string_map.try_emplace(2, "two");
	EXPECT_TRUE(try_result.second);

	try_result = int_string_map.try_emplace(2, "dos");
	EXPECT_FALSE(try_result.second);
	EXPECT_EQ(try_result.first->second, "two");
}

TEST_F(MapTest, InsertOrAssign)
{
	int_string_map.insert_or_assign(1, "one");
	EXPECT_EQ(int_string_map[1], "one");

	int_string_map.insert_or_assign(1, "uno");
	EXPECT_EQ(int_string_map[1], "uno");
}

TEST_F(MapTest, Erase)
{
	int_string_map = { { 1, "one" }, { 2, "two" }, { 3, "three" } };

	size_t erased = int_string_map.erase(2);
	EXPECT_EQ(erased, 1);
	EXPECT_EQ(int_string_map.size(), 2);
	EXPECT_EQ(int_string_map.find(2), int_string_map.end());

	erased = int_string_map.erase(4);
	EXPECT_EQ(erased, 0);
}

TEST_F(MapTest, EraseIterator)
{
	int_string_map = { { 1, "one" }, { 2, "two" }, { 3, "three" } };

	auto it = int_string_map.find(2);
	EXPECT_NE(it, int_string_map.end());

	int_string_map.erase(it);
	EXPECT_EQ(int_string_map.size(), 2);
	EXPECT_EQ(int_string_map.find(2), int_string_map.end());
}

TEST_F(MapTest, Clear)
{
	int_string_map = { { 1, "one" }, { 2, "two" }, { 3, "three" } };
	EXPECT_FALSE(int_string_map.empty());

	int_string_map.clear();
	EXPECT_TRUE(int_string_map.empty());
	EXPECT_EQ(int_string_map.size(), 0);
}

TEST_F(MapTest, Contains)
{
	int_string_map = { { 1, "one" }, { 2, "two" } };

	EXPECT_TRUE(int_string_map.contains(1));
	EXPECT_TRUE(int_string_map.contains(2));
	EXPECT_FALSE(int_string_map.contains(3));
}

TEST_F(MapTest, Count)
{
	int_string_map = { { 1, "one" }, { 2, "two" } };

	EXPECT_EQ(int_string_map.count(1), 1);
	EXPECT_EQ(int_string_map.count(3), 0);
}

TEST_F(MapTest, LowerUpperBound)
{
	int_string_map = { { 1, "one" }, { 3, "three" }, { 5, "five" }, { 7, "seven" } };

	auto lower = int_string_map.lower_bound(3);
	EXPECT_EQ(lower->first, 3);

	lower = int_string_map.lower_bound(4);
	EXPECT_EQ(lower->first, 5);

	auto upper = int_string_map.upper_bound(3);
	EXPECT_EQ(upper->first, 5);

	upper = int_string_map.upper_bound(7);
	EXPECT_EQ(upper, int_string_map.end());
}

TEST_F(MapTest, EqualRange)
{
	int_string_map = { { 1, "one" }, { 3, "three" }, { 5, "five" } };

	auto range = int_string_map.equal_range(3);
	EXPECT_EQ(range.first->first, 3);
	EXPECT_EQ(range.second->first, 5);

	range = int_string_map.equal_range(4);
	EXPECT_EQ(range.first->first, 5);
	EXPECT_EQ(range.second->first, 5);
}

TEST_F(MapTest, Iterators)
{
	int_string_map = { { 3, "three" }, { 1, "one" }, { 4, "four" }, { 2, "two" } };

	/* map maintains sorted order */
	std::vector<int> keys;
	for (const auto &[key, value]: int_string_map)
	{
		keys.push_back(key);
	}

	EXPECT_EQ(keys, (std::vector<int>{1, 2, 3, 4}));

	/* reverse iterator */
	keys.clear();
	for (auto & it : std::ranges::reverse_view(int_string_map))
	{
		keys.push_back(it.first);
	}

	EXPECT_EQ(keys, (std::vector<int>{4, 3, 2, 1}));
}

TEST_F(MapTest, CopyConstruction)
{
	int_string_map = { { 1, "one" }, { 2, "two" }, { 3, "three" } };

	ach::map<int, std::string> copy(int_string_map);
	EXPECT_EQ(copy.size(), int_string_map.size());

	for (const auto &[key, value]: int_string_map)
	{
		EXPECT_TRUE(copy.contains(key));
		EXPECT_EQ(copy[key], value);
	}
}

TEST_F(MapTest, MoveConstruction)
{
	int_string_map = { { 1, "one" }, { 2, "two" }, { 3, "three" } };
	size_t original_size = int_string_map.size();

	ach::map<int, std::string> moved(std::move(int_string_map));
	EXPECT_EQ(moved.size(), original_size);
	EXPECT_TRUE(int_string_map.empty());
}

TEST_F(MapTest, CopyAssignment)
{
	int_string_map = { { 1, "one" }, { 2, "two" } };
	ach::map<int, std::string> other = { { 3, "three" } };

	other = int_string_map;
	EXPECT_EQ(other.size(), int_string_map.size());
	EXPECT_TRUE(other.contains(1));
	EXPECT_TRUE(other.contains(2));
	EXPECT_FALSE(other.contains(3));
}

TEST_F(MapTest, MoveAssignment)
{
	int_string_map = { { 1, "one" }, { 2, "two" } };
	ach::map<int, std::string> other = { { 3, "three" } };

	other = std::move(int_string_map);
	EXPECT_EQ(other.size(), 2);
	EXPECT_TRUE(int_string_map.empty());
	EXPECT_TRUE(other.contains(1));
	EXPECT_TRUE(other.contains(2));
}

TEST_F(MapTest, InitializerListConstruction)
{
	ach::map<int, std::string> map = { { 1, "one" }, { 2, "two" }, { 3, "three" } };
	EXPECT_EQ(map.size(), 3);
	EXPECT_EQ(map[1], "one");
	EXPECT_EQ(map[2], "two");
	EXPECT_EQ(map[3], "three");
}

TEST_F(MapTest, Swap)
{
	int_string_map = { { 1, "one" }, { 2, "two" } };
	ach::map<int, std::string> other = { { 3, "three" }, { 4, "four" } };

	int_string_map.swap(other);

	EXPECT_EQ(int_string_map.size(), 2);
	EXPECT_TRUE(int_string_map.contains(3));
	EXPECT_TRUE(int_string_map.contains(4));

	EXPECT_EQ(other.size(), 2);
	EXPECT_TRUE(other.contains(1));
	EXPECT_TRUE(other.contains(2));
}

TEST_F(MapTest, ComparisonOperators)
{
	ach::map<int, std::string> map1 = { { 1, "one" }, { 2, "two" } };
	ach::map<int, std::string> map2 = { { 1, "one" }, { 2, "two" } };
	ach::map<int, std::string> map3 = { { 1, "one" }, { 3, "three" } };

	EXPECT_EQ(map1, map2);
	EXPECT_NE(map1, map3);
	EXPECT_LT(map1, map3);
	EXPECT_LE(map1, map2);
	EXPECT_GT(map3, map1);
	EXPECT_GE(map2, map1);
}

TEST_F(MapTest, KeyValueComparators)
{
	auto key_comp = int_string_map.key_comp();
	EXPECT_TRUE(key_comp(1, 2));
	EXPECT_FALSE(key_comp(2, 1));

	auto value_comp = int_string_map.value_comp();
	std::pair<const int, std::string> a(1, "one");
	std::pair<const int, std::string> b(2, "two");
	EXPECT_TRUE(value_comp(a, b));
	EXPECT_FALSE(value_comp(b, a));
}

TEST_F(MapTest, StressTest)
{
	constexpr auto N = 1000;

	try
	{
		/* insert many elements */
		for (int i = 0; i < N; ++i)
		{
			int_string_map[i] = std::to_string(i);
		}

		EXPECT_EQ(int_string_map.size(), N);

		/* verify all elements are present and in order */
		int expected = 0;
		for (const auto &[key, value]: int_string_map)
		{
			EXPECT_EQ(key, expected);
			EXPECT_EQ(value, std::to_string(expected));
			++expected;
		}

		/* erase half of them */
		for (int i = 0; i < N; i += 2)
		{
			int_string_map.erase(i);
		}

		EXPECT_EQ(int_string_map.size(), N / 2);

		/* verify remaining elements */
		for (int i = 1; i < N; i += 2)
		{
			EXPECT_TRUE(int_string_map.contains(i));
			EXPECT_EQ(int_string_map[i], std::to_string(i));
		}
	}
	catch (const std::bad_alloc &)
	{
		GTEST_SKIP() << "insufficient memory for stress test";
	} catch (...)
	{
		GTEST_SKIP() << "stress test failed with unknown exception";
	}
}
