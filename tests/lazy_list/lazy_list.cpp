// lazy_list.cpp : Defines the entry point for the console application.
//
#include <gtest/gtest.h>

#include <flex_lib/config/config.h>
#include <flex_lib/lazy_list.h>
#include <boost/optional.hpp>

typedef flex_lib::lazy_list<uint64_t> TestLazyList;

template<typename T>
class LazyNode
{
public:
    typedef std::function<T ()> generator_type;
    typedef LazyNode<T> this_type;
    
    LazyNode()
    {
        ;
    }
    
    LazyNode(const generator_type &gen)
        : m_generator(gen)
    {
        ;
    }
    
    LazyNode(generator_type &&gen)
        : m_generator(std::move(gen))
    {
        ;
    }
    
    T & operator *()
    {
        Dereference();
        return m_val.get();
    }
    
    const T & operator *() const
    {
        Dereference();
        return m_val.get();
    }    
private:
    void Dereference() const
    {
        if (m_val.is_initialized())
            return;
        
        m_val = m_generator();
    }
    
    mutable boost::optional<T> m_val;
    std::function<T ()> m_generator;
};

TEST(LazyList, EmptyList)
{
	TestLazyList list;

	EXPECT_TRUE(list.empty());
}

TEST(LazyList, SequencePtrInit)
{
	uint64_t items1[] = {0, 1, 2, 3, 4, 5};

	TestLazyList l1 = fl::lazy_from(std::begin(items1), std::end(items1)).toList();
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

TEST(LazyList, SequenceArrayInit)
{
	uint64_t items1[] = {0, 1, 2, 3, 4, 5};

	TestLazyList l1 = fl::lazy_from(items1).toList();
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

TEST(LazyList, SequenceIteratorInit)
{
	std::vector<uint64_t> items1{0, 1, 2, 3, 4, 5};

    TestLazyList l1 = fl::lazy_from(items1.begin(), items1.end()).toList();
    EXPECT_EQ(items1.size(), l1.size());

	auto it = l1.begin();
	EXPECT_EQ(items1[0], *it ++);
	EXPECT_EQ(items1[1], *it ++);
	EXPECT_EQ(items1[2], *it ++);
	EXPECT_EQ(items1[3], *it ++);
	EXPECT_EQ(items1[4], *it ++);
	EXPECT_EQ(items1[5], *it ++);
	EXPECT_TRUE(it == l1.end());
}

TEST(LazyList, SequenceContainerInit)
{
	std::vector<uint64_t> items1{0, 1, 2, 3, 4, 5};

    TestLazyList l1 = fl::lazy_from(items1).toList();
    EXPECT_EQ(items1.size(), l1.size());

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
    uint64_t count = 0;
	uint64_t items1[6];
    auto ftor = [&count, &items1](bool& isEnd)
	{
        if (count == 6)
        {
            isEnd = true;
            return 0ull;
        }

        uint64_t next_val = count ++;
        return items1[next_val] = next_val;
	};

    TestLazyList l1 = fl::lazy_generate(std::move(ftor)).toList();
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

template<typename T, size_t N>
auto MakeTestLazyList(T (&items)[N], size_t &counter)
{  
    return fl::lazy_generate(
    [&counter, &items](bool &isEos) -> T
    {
        if (counter == N)
        {
            isEos = true;
            return T();
        }
        
        auto next_val = counter ++;
        return items[next_val] = next_val;
    });   
}

TEST(LazyList, LazyFunctorInit)
{
	size_t count = 0;
	uint64_t items1[6];

    auto l1(MakeTestLazyList(items1, count).toList());
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

TEST(LazyList, ValidateLazy1)
{
    size_t count = 0;
	uint64_t items1[6];

    auto l1(MakeTestLazyList(items1, count).toList());

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
    auto ftor = [&count, &items1, &inner_count](bool &isEnd)
	{
		if (count == 6)
        {
            isEnd = true;
            return LazyNode<uint64_t>([] {return 0;});
        }

        int next_val = count ++;
		int& ic = inner_count;
		items1[next_val] = next_val;
        return LazyNode<uint64_t>([next_val, &ic]() -> uint64_t {++ ic; return next_val;});
	};

    auto l1 = fl::lazy_generate(std::move(ftor)).toList();

	auto it = l1.begin();
	EXPECT_EQ(0, count);
	EXPECT_EQ(0, inner_count);
	EXPECT_EQ(items1[0], **it);
	EXPECT_EQ(1, count);
	EXPECT_EQ(1, inner_count);
	++ it;
	EXPECT_EQ(1, inner_count);
	EXPECT_EQ(2, count);
	EXPECT_EQ(items1[1], **it);
	EXPECT_EQ(2, count);
	EXPECT_EQ(2, inner_count);
	++ it;
	EXPECT_EQ(2, inner_count);
	EXPECT_EQ(3, count);
	EXPECT_EQ(items1[2], **it);
	EXPECT_EQ(3, count);
	EXPECT_EQ(3, inner_count);
	++ it;
	EXPECT_EQ(3, inner_count);
	EXPECT_EQ(4, count);
	EXPECT_EQ(items1[3], **it);
	EXPECT_EQ(4, count);
	EXPECT_EQ(4, inner_count);
	++ it;
	EXPECT_EQ(4, inner_count);
	EXPECT_EQ(5, count);
	EXPECT_EQ(items1[4], **it);
	EXPECT_EQ(5, count);
	EXPECT_EQ(5, inner_count);
	++ it;
	EXPECT_EQ(5, inner_count);
	EXPECT_EQ(6, count);
	EXPECT_EQ(items1[5], **it);
	EXPECT_EQ(6, count);
	++ it;
	EXPECT_TRUE(it == l1.end());
	EXPECT_EQ(6, inner_count);
}

TEST(LazyList, Head)
{
	uint64_t items1[] = {0, 1, 2, 3, 4, 5};

    auto l1(fl::lazy_from(items1).toList());

	EXPECT_EQ(items1[0], l1.head());
}

TEST(LazyList, Tail)
{
	uint64_t items1[] = {0, 1, 2, 3, 4, 5};

    auto l2(fl::lazy_from(items1).tail().toList());

	auto it = l2.begin();
	EXPECT_EQ(items1[1], *it ++);
	EXPECT_EQ(items1[2], *it ++);
	EXPECT_EQ(items1[3], *it ++);
	EXPECT_EQ(items1[4], *it ++);
	EXPECT_EQ(items1[5], *it ++);
	EXPECT_TRUE(it == l2.end());
}

TEST(LazyList, ValidateLazyTail)
{
    size_t count = 0;
	uint64_t items1[6];

    auto l2(MakeTestLazyList(items1, count).tail().toList());

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

TEST(LazyList, Cons)
{
	uint64_t items1[] = {0, 1, 2, 3, 4, 5};

    auto l2(fl::lazy_from(items1).cons(10).toList());

	auto it = l2.begin();
    EXPECT_EQ(10, *it ++);
    EXPECT_EQ(items1[0], *it ++);
	EXPECT_EQ(items1[1], *it ++);
	EXPECT_EQ(items1[2], *it ++);
	EXPECT_EQ(items1[3], *it ++);
	EXPECT_EQ(items1[4], *it ++);
	EXPECT_EQ(items1[5], *it ++);
	EXPECT_TRUE(it == l2.end());
}

TEST(LazyList, ValidateLazyCons)
{
    size_t count = 0;
	uint64_t items1[6];

    auto l2(MakeTestLazyList(items1, count).cons(10).toList());

	auto it = l2.begin();
    EXPECT_EQ(0, count);
	EXPECT_EQ(10, *it ++);
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
	EXPECT_TRUE(it == l2.end());
	EXPECT_EQ(6, count);
}

TEST(LazyList, Filter)
{
	uint64_t items1[] = {0, 1, 2, 3, 4, 5};

    auto l2(fl::lazy_from(items1).filter([](uint64_t val) {return (val % 2) != 0;}).toList());

	auto it = l2.begin();
	EXPECT_EQ(items1[1], *it ++);
	EXPECT_EQ(items1[3], *it ++);
	EXPECT_EQ(items1[5], *it ++);
	EXPECT_TRUE(it == l2.end());
}

TEST(LazyList, Map)
{
	uint64_t items1[] = {0, 1, 2, 3, 4, 5};

    auto l2(fl::lazy_from(items1).map([](uint64_t val) {return std::to_string(val);}).toList());

	auto it = l2.begin();
    EXPECT_EQ(std::string("0"), *it ++);
	EXPECT_EQ(std::string("1"), *it ++);
    EXPECT_EQ(std::string("2"), *it ++);
    EXPECT_EQ(std::string("3"), *it ++);
    EXPECT_EQ(std::string("4"), *it ++);
    EXPECT_EQ(std::string("5"), *it ++);
	EXPECT_TRUE(it == l2.end());
}

TEST(LazyList, Zip)
{
	uint64_t items1[] = {0, 1, 2, 3, 4, 5};

    auto l1(fl::lazy_from(items1).toList());
    auto l2(fl::lazy_from(l1).zip(l1, [](uint64_t v1, uint64_t v2) {return v1 * v2;}).toList());

	auto it = l2.begin();
    EXPECT_EQ(0, *it ++);
	EXPECT_EQ(1, *it ++);
    EXPECT_EQ(4, *it ++);
    EXPECT_EQ(9, *it ++);
    EXPECT_EQ(16, *it ++);
    EXPECT_EQ(25, *it ++);
	EXPECT_TRUE(it == l2.end());
}

TEST(LazyList, ZipLazySequence)
{
    auto l1(fl::lazy_sequence<uint64_t>(6).toList());
    auto l2(fl::lazy_from(l1).zip(l1, [](uint64_t v1, uint64_t v2) {return v1 * v2;}).toList());

	auto it = l2.begin();
    EXPECT_EQ(0, *it ++);
	EXPECT_EQ(1, *it ++);
    EXPECT_EQ(4, *it ++);
    EXPECT_EQ(9, *it ++);
    EXPECT_EQ(16, *it ++);
    EXPECT_EQ(25, *it ++);
	EXPECT_TRUE(it == l2.end());
}

TEST(LazyList, ZipSelfLazySequence)
{
    auto l2(fl::lazy_sequence<uint64_t>(6).zip_self([](uint64_t v1, uint64_t v2) {return v1 * v2;}).toList());

	auto it = l2.begin();
    EXPECT_EQ(0, *it ++);
	EXPECT_EQ(1, *it ++);
    EXPECT_EQ(4, *it ++);
    EXPECT_EQ(9, *it ++);
    EXPECT_EQ(16, *it ++);
    EXPECT_EQ(25, *it ++);
	EXPECT_TRUE(it == l2.end());
}
int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

