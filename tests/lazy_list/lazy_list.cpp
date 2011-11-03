// lazy_list.cpp : Defines the entry point for the console application.
//
#include <gtest/gtest.h>
#include <gtest/gtest-all.cc>
#include <flex_lib/lazy_list.h>

typedef flex_lib::lazy_list<uint64_t> TestLazyList;

TEST(LazyList, EmptyList)
{
	TestLazyList list;

	EXPECT_TRUE(list.empty());
}

TEST(LazyList, SequenceInit)
{
	uint64_t items1[] = {0, 1, 2, 3, 4, 5};

	TestLazyList l1(std::begin(items1), std::end(items1));
	EXPECT_EQ(sizeof(items1) / sizeof(items1[0]), l1.size());

	auto it = l1.begin();
	EXPECT_EQ(items1[0], *it ++);
	EXPECT_EQ(items1[1], *it ++);
	EXPECT_EQ(items1[2], *it ++);
	EXPECT_EQ(items1[3], *it ++);
	EXPECT_EQ(items1[4], *it ++);
	EXPECT_EQ(items1[5], *it ++);
	EXPECT_TRUE(it == l1.end());
}

TEST(LazyList, FunctorInit)
{
	int count = 0;
	uint64_t items1[6];
	auto ftor = [&count, &items1](bool& isEOS) mutable -> std::function<uint64_t ()>
	{
		if (count == 6)
		{
			isEOS = true;
			return std::function<uint64_t ()>();
		}
		int next_val = count ++;
		items1[next_val] = next_val;
		return [next_val](){return next_val;};
	};

	TestLazyList l1(std::move(ftor));
	EXPECT_EQ(6, l1.size());

	auto it = l1.begin();
	EXPECT_EQ(items1[0], *it ++);
	EXPECT_EQ(items1[1], *it ++);
	EXPECT_EQ(items1[2], *it ++);
	EXPECT_EQ(items1[3], *it ++);
	EXPECT_EQ(items1[4], *it ++);
	EXPECT_EQ(items1[5], *it ++);
	EXPECT_TRUE(it == l1.end());
	EXPECT_EQ(6, count);
}

TEST(LazyList, Head)
{
	uint64_t items1[] = {0, 1, 2, 3, 4, 5};

	TestLazyList l1(std::begin(items1), std::end(items1));

	EXPECT_EQ(items1[0], l1.head());
}

TEST(LazyList, Tail)
{
	uint64_t items1[] = {0, 1, 2, 3, 4, 5};

	TestLazyList l1(std::begin(items1), std::end(items1));
	auto l2 = l1.tail();

	auto it = l2.begin();
	EXPECT_EQ(items1[1], *it ++);
	EXPECT_EQ(items1[2], *it ++);
	EXPECT_EQ(items1[3], *it ++);
	EXPECT_EQ(items1[4], *it ++);
	EXPECT_EQ(items1[5], *it ++);
	EXPECT_TRUE(it == l2.end());
}

TEST(LazyList, ValidateLazy1)
{
	int count = 0;
	uint64_t items1[6];
	auto ftor = [&count, &items1](bool& isEOS) mutable -> std::function<uint64_t ()>
	{
		if (count == 6)
		{
			isEOS = true;
			return std::function<uint64_t ()>();
		}
		int next_val = count ++;
		items1[next_val] = next_val;
		return [next_val](){return next_val;};
	};

	TestLazyList l1(std::move(ftor));

	auto it = l1.begin();
	EXPECT_EQ(0, count);
	EXPECT_EQ(items1[0], *it ++);
	EXPECT_EQ(1, count);
	EXPECT_EQ(items1[1], *it ++);
	EXPECT_EQ(2, count);
	EXPECT_EQ(items1[2], *it ++);
	EXPECT_EQ(3, count);
	EXPECT_EQ(items1[3], *it ++);
	EXPECT_EQ(4, count);
	EXPECT_EQ(items1[4], *it ++);
	EXPECT_EQ(5, count);
	EXPECT_EQ(items1[5], *it ++);
	EXPECT_TRUE(it == l1.end());
	EXPECT_EQ(6, count);
}

TEST(LazyList, ValidateLazy2)
{
	int count = 0;
	int inner_count = 0;
	uint64_t items1[6];
	auto ftor = [&count, &items1, &inner_count](bool& isEOS) mutable -> std::function<uint64_t ()>
	{
		if (count == 6)
		{
			isEOS = true;
			return std::function<uint64_t ()>();
		}
		int next_val = count ++;
		int& ic = inner_count;
		items1[next_val] = next_val;
		return [next_val, &ic]() -> uint64_t {++ ic; return next_val;};
	};

	TestLazyList l1(std::move(ftor));

	auto it = l1.begin();
	EXPECT_EQ(0, count);
	EXPECT_EQ(0, inner_count);
	EXPECT_EQ(items1[0], *it);
	EXPECT_EQ(1, count);
	EXPECT_EQ(1, inner_count);
	++ it;
	EXPECT_EQ(1, inner_count);
	EXPECT_EQ(2, count);
	EXPECT_EQ(items1[1], *it);
	EXPECT_EQ(2, count);
	EXPECT_EQ(2, inner_count);
	++ it;
	EXPECT_EQ(2, inner_count);
	EXPECT_EQ(3, count);
	EXPECT_EQ(items1[2], *it);
	EXPECT_EQ(3, count);
	EXPECT_EQ(3, inner_count);
	++ it;
	EXPECT_EQ(3, inner_count);
	EXPECT_EQ(4, count);
	EXPECT_EQ(items1[3], *it);
	EXPECT_EQ(4, count);
	EXPECT_EQ(4, inner_count);
	++ it;
	EXPECT_EQ(4, inner_count);
	EXPECT_EQ(5, count);
	EXPECT_EQ(items1[4], *it);
	EXPECT_EQ(5, count);
	EXPECT_EQ(5, inner_count);
	++ it;
	EXPECT_EQ(5, inner_count);
	EXPECT_EQ(6, count);
	EXPECT_EQ(items1[5], *it);
	EXPECT_EQ(6, count);
	++ it;
	EXPECT_TRUE(it == l1.end());
	EXPECT_EQ(6, inner_count);
}

TEST(LazyList, ValidateLazyTail)
{
	int count = 0;
	uint64_t items1[6];
	auto ftor = [&count, &items1](bool& isEOS) mutable -> std::function<uint64_t ()>
	{
		if (count == 6)
		{
			isEOS = true;
			return std::function<uint64_t ()>();
		}
		int next_val = count ++;
		items1[next_val] = next_val;
		return [next_val](){return next_val;};
	};

	TestLazyList l1(std::move(ftor));
	auto l2 = l1.tail();

	auto it = l2.begin();
	EXPECT_EQ(0, count);
	EXPECT_EQ(items1[1], *it ++);
	EXPECT_EQ(2, count);
	EXPECT_EQ(items1[2], *it ++);
	EXPECT_EQ(3, count);
	EXPECT_EQ(items1[3], *it ++);
	EXPECT_EQ(4, count);
	EXPECT_EQ(items1[4], *it ++);
	EXPECT_EQ(5, count);
	EXPECT_EQ(items1[5], *it ++);
	EXPECT_TRUE(it == l2.end());
	EXPECT_EQ(6, count);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

