/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <algorithm>
#include <cstring>
#include <random>
#include <string>
#include <vector>
#include <acheron/cstring>
#include <gtest/gtest.h>

static constexpr std::size_t STR_BUFFER_SIZE = 4096;
static constexpr std::size_t MISALIGNED_OFFSET = 1;

class StringTestFixture : public testing::Test
{
protected:
    std::vector<char> buffer1;
    std::vector<char> buffer2;
    std::vector<char> buffer3;

    const char* short_str = "Hello";
    const char* medium_str = "This is a test string with moderate length for testing string functions";
    void SetUp() override
    {
        buffer1.resize(STR_BUFFER_SIZE);
        buffer2.resize(STR_BUFFER_SIZE);
        buffer3.resize(STR_BUFFER_SIZE);

        std::ranges::fill(buffer1, '\0');
        std::ranges::fill(buffer2, '\0');
        std::ranges::fill(buffer3, '\0');
    }
};

TEST_F(StringTestFixture, StrlenTest)
{
    EXPECT_EQ(5, ach::strlen(short_str));
    EXPECT_EQ(std::strlen(medium_str), ach::strlen(medium_str));
    EXPECT_EQ(0, ach::strlen(""));

    std::strcpy(buffer1.data(), medium_str);
    EXPECT_EQ(std::strlen(medium_str), ach::strlen(buffer1.data()));

    std::strcpy(buffer1.data() + MISALIGNED_OFFSET, medium_str);
    EXPECT_EQ(std::strlen(medium_str), ach::strlen(buffer1.data() + MISALIGNED_OFFSET));
    for (size_t i = 0; i < 8; i++)
    {
        std::ranges::fill(buffer1, '\0');
        std::strcpy(buffer1.data() + i, medium_str);
        EXPECT_EQ(std::strlen(medium_str), ach::strlen(buffer1.data() + i));
    }
}

TEST_F(StringTestFixture, StrnlenTest)
{
    EXPECT_EQ(5, ach::strnlen(short_str, 10));
    EXPECT_EQ(5, ach::strnlen(short_str, 5));
    EXPECT_EQ(3, ach::strnlen(short_str, 3));
    EXPECT_EQ(std::min(std::strlen(medium_str), static_cast<size_t>(20)), ach::strnlen(medium_str, 20));
    EXPECT_EQ(0, ach::strnlen("", 10));
    EXPECT_EQ(0, ach::strnlen(short_str, 0));
}

TEST_F(StringTestFixture, StrCpyTest)
{
    ach::strcpy(buffer1.data(), short_str);
    EXPECT_STREQ(short_str, buffer1.data());
    EXPECT_EQ(buffer1.data(), ach::strcpy(buffer1.data(), short_str));

    ach::strcpy(buffer1.data(), medium_str);
    EXPECT_STREQ(medium_str, buffer1.data());

    std::ranges::fill(buffer1, 'X');
    ach::strcpy(buffer1.data(), "");
    EXPECT_EQ('\0', buffer1[0]);
    EXPECT_EQ('X', buffer1[1]);

    std::ranges::fill(buffer1, '\0');
    ach::strcpy(buffer1.data() + MISALIGNED_OFFSET, medium_str);
    EXPECT_STREQ(medium_str, buffer1.data() + MISALIGNED_OFFSET);
    for (size_t i = 0; i < 8; i++)
    {
        std::fill(buffer1.begin(), buffer1.end(), '\0');
        ach::strcpy(buffer1.data() + i, medium_str);
        EXPECT_STREQ(medium_str, buffer1.data() + i);
    }
}

TEST_F(StringTestFixture, StrncpyTest)
{
    ach::strncpy(buffer1.data(), short_str, 10);
    EXPECT_STREQ(short_str, buffer1.data());

    std::ranges::fill(buffer1, 'X');
    ach::strncpy(buffer1.data(), short_str, 10);
    EXPECT_STREQ(short_str, buffer1.data());
    for (size_t i = std::strlen(short_str); i < 10; i++)
    {
        EXPECT_EQ('\0', buffer1[i]) << "expected null byte at position " << i;
    }
    EXPECT_EQ('X', buffer1[10]) << "expected non-null byte at position 10";

    std::ranges::fill(buffer1, 'X');
    ach::strncpy(buffer1.data(), short_str, 3);
    EXPECT_EQ(0, std::memcmp(short_str, buffer1.data(), 3));
    EXPECT_EQ('X', buffer1[3]) << "Expected no null termination";

    EXPECT_EQ(buffer1.data(), ach::strncpy(buffer1.data(), short_str, 10));
}

TEST_F(StringTestFixture, StrlcpyTest)
{
    size_t result = ach::strlcpy(buffer1.data(), short_str, STR_BUFFER_SIZE);
    EXPECT_STREQ(short_str, buffer1.data());
    EXPECT_EQ(std::strlen(short_str), result);

    std::ranges::fill(buffer1, 'X');
    result = ach::strlcpy(buffer1.data(), short_str, 4);
    EXPECT_STREQ("Hel", buffer1.data());
    EXPECT_EQ(std::strlen(short_str), result);

    std::ranges::fill(buffer1, 'X');
    result = ach::strlcpy(buffer1.data(), short_str, 0);
    EXPECT_EQ('X', buffer1[0]);
    EXPECT_EQ(std::strlen(short_str), result);

    std::ranges::fill(buffer1, 'X');
    result = ach::strlcpy(buffer1.data(), "", 10);
    EXPECT_STREQ("", buffer1.data());
    EXPECT_EQ(0, result);
}

TEST_F(StringTestFixture, StrcatTest)
{
    std::strcpy(buffer1.data(), "Hello");
    ach::strcat(buffer1.data(), " World");
    EXPECT_STREQ("Hello World", buffer1.data());

    buffer1[0] = '\0';
    ach::strcat(buffer1.data(), short_str);
    EXPECT_STREQ(short_str, buffer1.data());

    std::strcpy(buffer1.data(), short_str);
    ach::strcat(buffer1.data(), "");
    EXPECT_STREQ(short_str, buffer1.data());

    buffer1[0] = '\0';
    ach::strcat(buffer1.data(), "A");
    ach::strcat(buffer1.data(), "B");
    ach::strcat(buffer1.data(), "C");
    EXPECT_STREQ("ABC", buffer1.data());

    std::strcpy(buffer1.data(), "Test");
    EXPECT_EQ(buffer1.data(), ach::strcat(buffer1.data(), " Return"));
}

TEST_F(StringTestFixture, StrncatTest)
{
    std::strcpy(buffer1.data(), "Hello");
    ach::strncat(buffer1.data(), " World", 10);
    EXPECT_STREQ("Hello World", buffer1.data());

    std::strcpy(buffer1.data(), "Hello");
    ach::strncat(buffer1.data(), " World", 3);
    EXPECT_STREQ("Hello Wo", buffer1.data());

    std::strcpy(buffer1.data(), short_str);
    ach::strncat(buffer1.data(), " Extra", 0);
    EXPECT_STREQ(short_str, buffer1.data());

    std::strcpy(buffer1.data(), "Test");
    EXPECT_EQ(buffer1.data(), ach::strncat(buffer1.data(), " Return", 10));
}

TEST_F(StringTestFixture, StrlcatTest)
{
    std::strcpy(buffer1.data(), "Hello");
    size_t result = ach::strlcat(buffer1.data(), " World", STR_BUFFER_SIZE);
    EXPECT_STREQ("Hello World", buffer1.data());
    EXPECT_EQ(std::strlen("Hello") + std::strlen(" World"), result);

    std::strcpy(buffer1.data(), "Hello");
    result = ach::strlcat(buffer1.data(), " World", 8); /* only space for "Hello" + " W" + '\0' */
    EXPECT_STREQ("Hello W", buffer1.data());
    EXPECT_EQ(std::strlen("Hello") + std::strlen(" World"), result);

    std::strcpy(buffer1.data(), "Hello");
    result = ach::strlcat(buffer1.data(), " World", 6); /* only space for "Hello" + '\0' */
    EXPECT_STREQ("Hello", buffer1.data());
    EXPECT_EQ(std::strlen("Hello") + std::strlen(" World"), result);

    buffer1[0] = '\0';
    result = ach::strlcat(buffer1.data(), "", 10);
    EXPECT_STREQ("", buffer1.data());
    EXPECT_EQ(0, result);

    result = ach::strlcat(buffer1.data(), "Test", 10);
    EXPECT_STREQ("Test", buffer1.data());
    EXPECT_EQ(4, result);
}

TEST_F(StringTestFixture, StrcmpTest)
{
    EXPECT_EQ(0, ach::strcmp("test", "test"));

    EXPECT_LT(ach::strcmp("abc", "def"), 0);
    EXPECT_GT(ach::strcmp("def", "abc"), 0);

    EXPECT_LT(ach::strcmp("abc", "abcdef"), 0);
    EXPECT_GT(ach::strcmp("abcdef", "abc"), 0);

    EXPECT_EQ(0, ach::strcmp("", ""));
    EXPECT_LT(ach::strcmp("", "a"), 0);
    EXPECT_GT(ach::strcmp("a", ""), 0);

    std::strcpy(buffer1.data(), "test string");
    std::strcpy(buffer2.data(), "test string");

    EXPECT_EQ(0, ach::strcmp(buffer1.data(), buffer2.data()));
    for (size_t i = 0; i < 8; i++)
    {
        std::strcpy(buffer1.data() + i, "test string");
        EXPECT_EQ(0, ach::strcmp(buffer1.data() + i, "test string"));
    }

    char str1[] = "Test";
    char str2[] = "Test";
    str1[0] = static_cast<char>(200); /* 1st char > 127 */
    str2[0] = static_cast<char>(100);
    EXPECT_GT(ach::strcmp(str1, str2), 0);
}

TEST_F(StringTestFixture, StrncmpTest)
{
    EXPECT_EQ(0, ach::strncmp("test", "test", 4));
    EXPECT_EQ(0, ach::strncmp("test", "testing", 4));

    EXPECT_LT(ach::strncmp("abc", "def", 3), 0);
    EXPECT_GT(ach::strncmp("def", "abc", 3), 0);
    EXPECT_EQ(0, ach::strncmp("different", "strings", 0));

    EXPECT_EQ(0, ach::strncmp("abcdef", "abcxxx", 3));
    EXPECT_LT(ach::strncmp("abcdef", "abcxxx", 4), 0);

    EXPECT_LT(ach::strncmp(nullptr, "test", 4), 0);
    EXPECT_GT(ach::strncmp("test", nullptr, 4), 0);
    EXPECT_EQ(0, ach::strncmp(nullptr, nullptr, 4));
}

TEST_F(StringTestFixture, StrcasecmpTest)
{
    EXPECT_EQ(0, ach::strcasecmp("test", "TEST"));
    EXPECT_EQ(0, ach::strcasecmp("Test", "tEsT"));

    EXPECT_LT(ach::strcasecmp("abc", "def"), 0);
    EXPECT_GT(ach::strcasecmp("DEF", "abc"), 0);

    EXPECT_LT(ach::strcasecmp("abc", "ABCDEF"), 0);
    EXPECT_GT(ach::strcasecmp("ABCDEF", "abc"), 0);

    EXPECT_EQ(0, ach::strcasecmp("", ""));
    EXPECT_LT(ach::strcasecmp("", "A"), 0);
    EXPECT_GT(ach::strcasecmp("a", ""), 0);
    EXPECT_LT(ach::strcasecmp(nullptr, "test"), 0);
    EXPECT_GT(ach::strcasecmp("test", nullptr), 0);
    EXPECT_EQ(0, ach::strcasecmp(nullptr, nullptr));
}

TEST_F(StringTestFixture, StrncasecmpTest)
{
    EXPECT_EQ(0, ach::strncasecmp("test", "TEST", 4));
    EXPECT_EQ(0, ach::strncasecmp("Test", "tEsT", 4));
    EXPECT_EQ(0, ach::strncasecmp("test", "TESTING", 4));
    EXPECT_LT(ach::strncasecmp("abc", "DEF", 3), 0);
    EXPECT_GT(ach::strncasecmp("DEF", "abc", 3), 0);
    EXPECT_EQ(0, ach::strncasecmp("DIFFERENT", "strings", 0));
    EXPECT_EQ(0, ach::strncasecmp("abcDEF", "ABCxxx", 3));
    EXPECT_LT(ach::strncasecmp("abcDEF", "ABCxxx", 4), 0);
}

TEST_F(StringTestFixture, StrchrTest)
{
    const char* test_str = "This is a test string";

    EXPECT_EQ(test_str + 2, ach::strchr(test_str, 'i'));
    EXPECT_EQ(test_str, ach::strchr(test_str, 'T'));
    EXPECT_EQ(test_str + std::strlen(test_str) - 1, ach::strchr(test_str, 'g'));
    EXPECT_EQ(test_str + std::strlen(test_str), ach::strchr(test_str, '\0'));
    EXPECT_EQ(nullptr, ach::strchr(test_str, 'z'));
    EXPECT_EQ(nullptr, ach::strchr("", 'a'));
    EXPECT_EQ("", ach::strchr("", '\0'));
}

TEST_F(StringTestFixture, StrrchrTest)
{
    const char* test_str = "This is a test string with multiple 'i' characters";
    EXPECT_EQ(test_str + 37, ach::strrchr(test_str, 'i'));
    EXPECT_EQ(test_str, ach::strrchr(test_str, 'T'));
    EXPECT_EQ(test_str + std::strlen(test_str) - 1, ach::strrchr(test_str, 's'));
    EXPECT_EQ(test_str + std::strlen(test_str), ach::strrchr(test_str, '\0'));
    EXPECT_EQ(nullptr, ach::strrchr(test_str, 'z'));
    EXPECT_EQ(nullptr, ach::strrchr("", 'a'));
    EXPECT_EQ("", ach::strrchr("", '\0'));
}

TEST_F(StringTestFixture, StrnchrTest)
{
    const char* test_str = "This is a test string";
    EXPECT_EQ(test_str + 2, ach::strnchr(test_str, 10, 'i'));
    EXPECT_EQ(nullptr, ach::strnchr(test_str, 1, 'i'));
    EXPECT_EQ(test_str + 10, ach::strnchr(test_str, 11, 't'));
    EXPECT_EQ(nullptr, ach::strnchr(test_str, 0, 'T'));
    EXPECT_EQ(test_str + 2, ach::strnchr(test_str, 100, 'i'));
    EXPECT_EQ(nullptr, ach::strnchr(test_str, 100, 'z'));
}

TEST_F(StringTestFixture, StrnstrTest)
{
    const char* haystack = "This is a test string for substring search";
    EXPECT_EQ(haystack + 10, ach::strnstr(haystack, "test", 50));
    EXPECT_EQ(nullptr, ach::strnstr(haystack, "test", 9));
    EXPECT_EQ(haystack + 10, ach::strnstr(haystack, "test", 14));
    EXPECT_EQ(haystack, ach::strnstr(haystack, "", 50));
    EXPECT_EQ(nullptr, ach::strnstr(haystack, "test", 0));
    EXPECT_EQ(nullptr, ach::strnstr(haystack, "notfound", 50));
    EXPECT_EQ(nullptr, ach::strnstr(haystack, "string for", 20));
}

TEST_F(StringTestFixture, StrstrTest)
{
    const char* haystack = "This is a test string for substring search";
    std::cout << std::strstr(haystack, "search");
    EXPECT_EQ(haystack + 10, ach::strstr(haystack, "test"));
    EXPECT_EQ(haystack, ach::strstr(haystack, "This"));
    EXPECT_EQ(haystack + 36, ach::strstr(haystack, "search"));
    EXPECT_EQ(haystack, ach::strstr(haystack, ""));
    EXPECT_EQ(nullptr, ach::strstr(haystack, "notfound"));
    EXPECT_EQ(haystack + 2, ach::strstr(haystack, "is is"));
    EXPECT_EQ(nullptr, ach::strstr(haystack, "TEST"));
}
