/* this file is a part of Acheron library which is under MIT license; see LICENSE for more info */

#include <acheron/stack>
#include <acheron/list>
#include <gtest/gtest.h>
#include <string>

class StackTest : public ::testing::Test
{
protected:
	ach::stack<int> int_stack;
	ach::stack<std::string> string_stack;
};

TEST_F(StackTest, DefaultConstruction)
{
	EXPECT_TRUE(int_stack.empty());
	EXPECT_EQ(int_stack.size(), 0);
}

TEST_F(StackTest, PushAndTop)
{
	int_stack.push(1);
	EXPECT_EQ(int_stack.top(), 1);
	EXPECT_EQ(int_stack.size(), 1);

	int_stack.push(2);
	EXPECT_EQ(int_stack.top(), 2);
	EXPECT_EQ(int_stack.size(), 2);
}

TEST_F(StackTest, Pop)
{
	int_stack.push(1);
	int_stack.push(2);
	int_stack.push(3);

	int_stack.pop();
	EXPECT_EQ(int_stack.top(), 2);
	EXPECT_EQ(int_stack.size(), 2);

	int_stack.pop();
	EXPECT_EQ(int_stack.top(), 1);
	EXPECT_EQ(int_stack.size(), 1);

	int_stack.pop();
	EXPECT_TRUE(int_stack.empty());
}

TEST_F(StackTest, Emplace)
{
	string_stack.emplace("Hello");
	EXPECT_EQ(string_stack.top(), "Hello");

	string_stack.emplace(5, 'A');
	EXPECT_EQ(string_stack.top(), "AAAAA");
}

TEST_F(StackTest, CopyConstruction)
{
	int_stack.push(1);
	int_stack.push(2);
	int_stack.push(3);

	ach::stack<int> copy(int_stack);
	EXPECT_EQ(copy.size(), int_stack.size());
	EXPECT_EQ(copy.top(), 3);
}

TEST_F(StackTest, MoveConstruction)
{
	int_stack.push(1);
	int_stack.push(2);
	int_stack.push(3);

	ach::stack<int> moved(std::move(int_stack));
	EXPECT_EQ(moved.size(), 3);
	EXPECT_EQ(moved.top(), 3);
	EXPECT_TRUE(int_stack.empty());
}

TEST_F(StackTest, CopyAssignment)
{
	int_stack.push(1);
	int_stack.push(2);

	ach::stack<int> other;
	other = int_stack;

	EXPECT_EQ(other.size(), int_stack.size());
	EXPECT_EQ(other.top(), 2);
}

TEST_F(StackTest, MoveAssignment)
{
	int_stack.push(1);
	int_stack.push(2);

	ach::stack<int> other;
	other = std::move(int_stack);

	EXPECT_EQ(other.size(), 2);
	EXPECT_EQ(other.top(), 2);
	EXPECT_TRUE(int_stack.empty());
}

TEST_F(StackTest, ContainerConstruction)
{
	ach::vector<int> vec = { 1, 2, 3 };
	ach::stack<int> stack_from_vec(vec);

	EXPECT_EQ(stack_from_vec.size(), 3);
	EXPECT_EQ(stack_from_vec.top(), 3);
}

TEST_F(StackTest, IteratorConstruction)
{
	std::vector<int> vec = { 1, 2, 3, 4, 5 };
	ach::stack<int> stack_from_iters(vec.begin(), vec.end());

	EXPECT_EQ(stack_from_iters.size(), 5);
	EXPECT_EQ(stack_from_iters.top(), 5);
}

TEST_F(StackTest, Swap)
{
	ach::stack<int> stack1;
	stack1.push(1);
	stack1.push(2);

	ach::stack<int> stack2;
	stack2.push(3);
	stack2.push(4);
	stack2.push(5);

	stack1.swap(stack2);

	EXPECT_EQ(stack1.size(), 3);
	EXPECT_EQ(stack1.top(), 5);

	EXPECT_EQ(stack2.size(), 2);
	EXPECT_EQ(stack2.top(), 2);
}

TEST_F(StackTest, ComparisonOperators)
{
	ach::stack<int> stack1;
	stack1.push(1);
	stack1.push(2);

	ach::stack<int> stack2;
	stack2.push(1);
	stack2.push(2);

	ach::stack<int> stack3;
	stack3.push(1);
	stack3.push(3);

	EXPECT_EQ(stack1, stack2);
	EXPECT_NE(stack1, stack3);
	EXPECT_LT(stack1, stack3);
	EXPECT_LE(stack1, stack2);
	EXPECT_GT(stack3, stack1);
	EXPECT_GE(stack2, stack1);
}

TEST_F(StackTest, WithDifferentContainer)
{
	/* test with list as underlying container */
	ach::stack<int, ach::list<int> > list_stack;

	list_stack.push(1);
	list_stack.push(2);
	list_stack.push(3);

	EXPECT_EQ(list_stack.size(), 3);
	EXPECT_EQ(list_stack.top(), 3);

	list_stack.pop();
	EXPECT_EQ(list_stack.top(), 2);
}

TEST_F(StackTest, LIFOBehavior)
{
	/* verify Last-In-First-Out behavior */
	int_stack.push(1);
	int_stack.push(2);
	int_stack.push(3);
	int_stack.push(4);
	int_stack.push(5);

	std::vector<int> popped_order;
	while (!int_stack.empty())
	{
		popped_order.push_back(int_stack.top());
		int_stack.pop();
	}

	std::vector<int> expected = { 5, 4, 3, 2, 1 };
	EXPECT_EQ(popped_order, expected);
}
