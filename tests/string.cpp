/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <string>
#include <acheron/string>
#include <gtest/gtest.h>

TEST(AcheronStringTest, BasicConstruction)
{
    const ach::string s1 = "";
    EXPECT_TRUE(s1.empty());
    EXPECT_EQ(s1.size(), 0);

    const auto test_str = "Hello, World!";
    const ach::string s2(test_str);
    EXPECT_STREQ(s2.c_str(), test_str);
    EXPECT_EQ(s2.size(), std::strlen(test_str));

    const ach::string s3(5, 'a');
    EXPECT_EQ(s3.size(), 5);
    EXPECT_EQ(s3, ach::string("aaaaa"));
}

TEST(AcheronStringTest, Modification)
{
    ach::string s1 = "Hello";

    s1 += " World";
    EXPECT_EQ(s1, ach::string("Hello World"));

    s1.push_back('!');
    EXPECT_EQ(s1, ach::string("Hello World!"));

    s1.resize(5);
    EXPECT_EQ(s1, ach::string("Hello"));

    s1.resize(7, 'x');
    EXPECT_EQ(s1, ach::string("Helloxx"));
}

TEST(AcheronStringTest, Access)
{
    ach::string s1 = "Hello";

    EXPECT_EQ(s1[0], 'H');
    EXPECT_EQ(s1.front(), 'H');
    EXPECT_EQ(s1.back(), 'o');

    EXPECT_EQ(s1.at(1), 'e');
    EXPECT_THROW(s1.at(5), std::out_of_range);
}

TEST(AcheronStringTest, Iterators)
{
    ach::string s1 = "Hello";
    std::string result;
    for (auto it = s1.begin(); it != s1.end(); ++it)
        result += *it;
    EXPECT_EQ(result, "Hello");

    result.clear();
    for (auto it = s1.rbegin(); it != s1.rend(); ++it)
        result += *it;
    EXPECT_EQ(result, "olleH");
}

TEST(AcheronStringTest, Conversion)
{
    const ach::string s1 = "Hello";

    const std::string_view view = s1;
    EXPECT_EQ(view, "Hello");
    EXPECT_EQ(view.length(), s1.length());
}

TEST(AcheronStringTest, Capacity)
{
    ach::string s1 = "";
    const size_t initial_cap = s1.capacity();
    for (auto i = 0; i < 100; ++i)
        s1 += "X";

    EXPECT_GE(s1.capacity(), initial_cap);
    EXPECT_EQ(s1.size(), 100);

    s1.resize(10);
    s1.shrink_to_fit();
    EXPECT_EQ(s1.size(), 10);
}
