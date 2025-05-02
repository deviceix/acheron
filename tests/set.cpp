/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <string>
#include <set>
#include <acheron/set>
#include <gtest/gtest.h>

class SetTest : public ::testing::Test
{
protected:
    ach::set<int> int_set;
    ach::set<std::string> string_set;
};

TEST_F(SetTest, DefaultConstruction)
{
    EXPECT_TRUE(int_set.empty());
    EXPECT_EQ(int_set.size(), 0);
}

TEST_F(SetTest, RangeConstruction)
{
    std::set<int> stdset = { 1, 2, 3, 4, 5 };
    ach::set<int> from_set(stdset.begin(), stdset.end());

    EXPECT_EQ(from_set.size(), stdset.size());
    
    auto it1 = from_set.begin();
    auto it2 = stdset.begin();
    
    while (it1 != from_set.end() && it2 != stdset.end())
    {
        EXPECT_EQ(*it1, *it2);
        ++it1;
        ++it2;
    }
}

TEST_F(SetTest, InitializerListConstruction)
{
    ach::set<int> init_set = { 5, 3, 1, 4, 2 };
    EXPECT_EQ(init_set.size(), 5);

    int expected = 1;
    for (const auto &val: init_set)
    {
        EXPECT_EQ(val, expected++);
    }
}

TEST_F(SetTest, CopyConstruction)
{
    int_set.insert(1);
    int_set.insert(2);
    int_set.insert(3);

    ach::set<int> copy(int_set);
    EXPECT_EQ(copy.size(), int_set.size());

    auto it1 = copy.begin();
    auto it2 = int_set.begin();
    
    while (it1 != copy.end() && it2 != int_set.end())
    {
        EXPECT_EQ(*it1, *it2);
        ++it1;
        ++it2;
    }
}

TEST_F(SetTest, MoveConstruction)
{
    int_set.insert(1);
    int_set.insert(2);
    int_set.insert(3);

    ach::set<int> moved(std::move(int_set));
    EXPECT_EQ(moved.size(), 3);
    EXPECT_TRUE(int_set.empty());

    int expected = 1;
    for (const auto &val: moved)
    {
        EXPECT_EQ(val, expected++);
    }
}

TEST_F(SetTest, InsertSingle)
{
    auto result1 = int_set.insert(1);
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(*(result1.first), 1);
    
    auto result2 = int_set.insert(2);
    EXPECT_TRUE(result2.second);
    
    auto result3 = int_set.insert(1);  /* dupe */
    EXPECT_FALSE(result3.second);
    EXPECT_EQ(*(result3.first), 1);
    
    EXPECT_EQ(int_set.size(), 2);
}

TEST_F(SetTest, InsertHint)
{
    int_set.insert(1);
    int_set.insert(3);
    
    auto hint = int_set.find(1);
    auto it = int_set.insert(hint, 2);
    
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(int_set.size(), 3);
    
    int expected = 1;
    for (const auto &val: int_set)
    {
        EXPECT_EQ(val, expected++);
    }
}

TEST_F(SetTest, InsertRange)
{
    std::set<int> source = { 1, 2, 3, 4, 5 };
    int_set.insert(source.begin(), source.end());
    
    EXPECT_EQ(int_set.size(), 5);
    
    int expected = 1;
    for (const auto &val: int_set)
    {
        EXPECT_EQ(val, expected++);
    }
}

TEST_F(SetTest, InsertInitializerList)
{
    int_set.insert({ 5, 3, 1, 4, 2 });
    
    EXPECT_EQ(int_set.size(), 5);
    
    int expected = 1;
    for (const auto &val: int_set)
    {
        EXPECT_EQ(val, expected++);
    }
}

TEST_F(SetTest, EmplaceOperations)
{
    auto result1 = int_set.emplace(1);
    EXPECT_TRUE(result1.second);
    
    auto result2 = int_set.emplace(1);  /* dupe */
    EXPECT_FALSE(result2.second);
    
    auto hint = int_set.find(1);
    auto it = int_set.emplace_hint(hint, 2);
    
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(int_set.size(), 2);
}

TEST_F(SetTest, EmplaceWithCustomType)
{
    struct Point
    {
        int x, y;
        
        Point(int x, int y) : x(x), y(y) {}
        
        bool operator<(const Point& other) const
        {
            return x < other.x || (x == other.x && y < other.y);
        }
    };
    
    ach::set<Point> point_set;
    
    auto result1 = point_set.emplace(1, 2);
    EXPECT_TRUE(result1.second);
    
    auto result2 = point_set.emplace(3, 4);
    EXPECT_TRUE(result2.second);
    
    auto result3 = point_set.emplace(1, 2);  /* dupe */
    EXPECT_FALSE(result3.second);
    
    EXPECT_EQ(point_set.size(), 2);
}

TEST_F(SetTest, EraseSingle)
{
    int_set = { 1, 2, 3, 4, 5 };
    
    auto it = int_set.find(3);
    auto next_it = int_set.erase(it);
    
    EXPECT_EQ(int_set.size(), 4);
    EXPECT_EQ(*next_it, 4);
    EXPECT_FALSE(int_set.contains(3));
}

TEST_F(SetTest, EraseKey)
{
    int_set = { 1, 2, 3, 4, 5 };
    
    auto count = int_set.erase(3);
    EXPECT_EQ(count, 1);
    
    count = int_set.erase(10);
    EXPECT_EQ(count, 0);
    
    EXPECT_EQ(int_set.size(), 4);
    EXPECT_FALSE(int_set.contains(3));
}

TEST_F(SetTest, EraseRange)
{
    int_set = { 1, 2, 3, 4, 5 };
    
    auto first = int_set.find(2);
    auto last = int_set.find(5);
    
    auto it = int_set.erase(first, last);
    EXPECT_EQ(*it, 5);
    EXPECT_EQ(int_set.size(), 2);
    
    auto expected = ach::set<int>{ 1, 5 };
    EXPECT_EQ(int_set, expected);
}

TEST_F(SetTest, Clear)
{
    int_set = { 1, 2, 3, 4, 5 };
    EXPECT_FALSE(int_set.empty());
    
    int_set.clear();
    EXPECT_TRUE(int_set.empty());
    EXPECT_EQ(int_set.size(), 0);
}

TEST_F(SetTest, FindContainsCount)
{
    int_set = { 1, 2, 3, 4, 5 };
    
    auto it = int_set.find(3);
    EXPECT_NE(it, int_set.end());
    EXPECT_EQ(*it, 3);
    
    it = int_set.find(10);  /* doesnt exist */
    EXPECT_EQ(it, int_set.end());
    
    EXPECT_TRUE(int_set.contains(3));
    EXPECT_FALSE(int_set.contains(10));
    
    EXPECT_EQ(int_set.count(3), 1);
    EXPECT_EQ(int_set.count(10), 0);
}

TEST_F(SetTest, LowerUpperBound)
{
    int_set = { 10, 20, 30, 40, 50 };
    
    auto lower = int_set.lower_bound(20);
    EXPECT_EQ(*lower, 20);
    
    lower = int_set.lower_bound(25);
    EXPECT_EQ(*lower, 30);
    
    auto upper = int_set.upper_bound(20);
    EXPECT_EQ(*upper, 30);
    
    upper = int_set.upper_bound(25);
    EXPECT_EQ(*upper, 30);
    
    upper = int_set.upper_bound(50);
    EXPECT_EQ(upper, int_set.end());
}

TEST_F(SetTest, EqualRange)
{
    int_set = { 10, 20, 30, 40, 50 };
    
    auto range = int_set.equal_range(20);
    EXPECT_EQ(*(range.first), 20);
    EXPECT_EQ(*(range.second), 30);
    
    range = int_set.equal_range(25);  /* doesnt exist */
    EXPECT_EQ(*(range.first), 30);
    EXPECT_EQ(*(range.second), 30);
}

TEST_F(SetTest, Iterators)
{
    int_set = { 1, 2, 3, 4, 5 };

    int expected = 1;
    for (auto it = int_set.begin(); it != int_set.end(); ++it)
    {
        EXPECT_EQ(*it, expected++);
    }

    expected = 5;
    for (auto it = int_set.rbegin(); it != int_set.rend(); ++it)
    {
        EXPECT_EQ(*it, expected--);
    }

    const auto& const_set = int_set;
    expected = 1;
    for (auto it = const_set.begin(); it != const_set.end(); ++it)
    {
        EXPECT_EQ(*it, expected++);
    }
    
    expected = 1;
    for (auto it = const_set.cbegin(); it != const_set.cend(); ++it)
    {
        EXPECT_EQ(*it, expected++);
    }
}

TEST_F(SetTest, SwapOperations)
{
    ach::set<int> set1 = { 1, 2, 3 };
    ach::set<int> set2 = { 4, 5, 6 };
    
    set1.swap(set2);
    
    EXPECT_EQ(set1.size(), 3);
    EXPECT_EQ(set2.size(), 3);
    EXPECT_EQ(*set1.begin(), 4);
    EXPECT_EQ(*set2.begin(), 1);

    swap(set1, set2);
    
    EXPECT_EQ(*set1.begin(), 1);
    EXPECT_EQ(*set2.begin(), 4);
}

TEST_F(SetTest, ComparisonOperators)
{
    ach::set<int> set1 = { 1, 2, 3 };
    ach::set<int> set2 = { 1, 2, 3 };
    ach::set<int> set3 = { 1, 2, 4 };
    ach::set<int> set4 = { 1, 2 };
    
    EXPECT_EQ(set1, set2);
    EXPECT_NE(set1, set3);
    EXPECT_NE(set1, set4);
    
    EXPECT_LT(set1, set3);
    EXPECT_GT(set3, set1);
    
    EXPECT_LT(set4, set1);
    EXPECT_GT(set1, set4);
    
    EXPECT_LE(set1, set2);
    EXPECT_LE(set1, set3);
    
    EXPECT_GE(set1, set2);
    EXPECT_GE(set3, set1);
}

TEST_F(SetTest, KeyCompValueComp)
{
    auto key_comp = int_set.key_comp();
    EXPECT_TRUE(key_comp(1, 2));
    EXPECT_FALSE(key_comp(2, 1));
    
    auto value_comp = int_set.value_comp();
    EXPECT_TRUE(value_comp(1, 2));
    EXPECT_FALSE(value_comp(2, 1));
}

TEST_F(SetTest, StringSetOperations)
{
    string_set.insert("apple");
    string_set.insert("banana");
    string_set.insert("cherry");
    
    EXPECT_EQ(string_set.size(), 3);
    EXPECT_TRUE(string_set.contains("banana"));
    EXPECT_FALSE(string_set.contains("orange"));
    
    auto it = string_set.find("banana");
    EXPECT_NE(it, string_set.end());
    EXPECT_EQ(*it, "banana");
    
    it = string_set.erase(it);
    EXPECT_EQ(*it, "cherry");
    EXPECT_EQ(string_set.size(), 2);
    EXPECT_FALSE(string_set.contains("banana"));
}

TEST_F(SetTest, CustomComparator)
{
    struct ReverseCompare
    {
        bool operator()(int a, int b) const
        {
            return a > b;
        }
    };
    
    ach::set<int, ReverseCompare> reverse_set = { 1, 2, 3, 4, 5 };
    
    int expected = 5;
    for (const auto &val : reverse_set)
    {
        EXPECT_EQ(val, expected--);
    }
}

TEST_F(SetTest, DuplicateElements)
{
    int_set.insert(1);
    int_set.insert(1);  /* dupe */
    int_set.insert(1);  /* dupe */
    int_set.insert(2);
    
    EXPECT_EQ(int_set.size(), 2);
    
    auto result = int_set.insert(2);  /* dupe */
    EXPECT_FALSE(result.second);
}
