#include <gtest/gtest.h>
#include <gtest/gtest-all.cc>

#include <string>

#define FL_VARIANT_WITH_TEST 1

#include <flex_lib/variant.h>

TEST(Variant, EmptyInitialization)
{
    typedef flex_lib::Variant<int, double, char, std::string> Variant;
    
    Variant v;
    EXPECT_TRUE(v.IsEmpty());
}

TEST(Variant, ValueInitialization)
{
    typedef flex_lib::Variant<int, double, char, std::string> Variant;
    
    {
        Variant v(10);
        EXPECT_FALSE(v.IsEmpty());
        EXPECT_EQ(0, v.Which());
        EXPECT_EQ(10, *(int*)v.GetDataPointer());
    }
    
    {
        Variant v(20.0);
        EXPECT_FALSE(v.IsEmpty());
        EXPECT_EQ(1, v.Which());
        EXPECT_EQ(20.0, *(double*)v.GetDataPointer());
    }
    
    {
        Variant v('0');
        EXPECT_FALSE(v.IsEmpty());
        EXPECT_EQ(2, v.Which());
        EXPECT_EQ('0', *(char*)v.GetDataPointer());
    }
    
    {
        Variant v("abcd");
        EXPECT_FALSE(v.IsEmpty());
        EXPECT_EQ(3, v.Which());
        EXPECT_EQ(std::string("abcd"), *(std::string*)v.GetDataPointer());
    }
    
    {
        std::string str("bcde");
        Variant v(const_cast<const std::string&>(str));
        EXPECT_FALSE(v.IsEmpty());
        EXPECT_EQ(3, v.Which());
        EXPECT_EQ(str, *(std::string*)v.GetDataPointer());
    }
}

TEST(Variant, CopyConstruction)
{
    typedef flex_lib::Variant<int, double, char, std::string> Variant;
    
    {
        Variant v1(10);
        EXPECT_FALSE(v1.IsEmpty());
        EXPECT_EQ(0, v1.Which());
        EXPECT_EQ(10, *(int*)v1.GetDataPointer());
        
        Variant v2 = v1;
        EXPECT_FALSE(v2.IsEmpty());
        EXPECT_EQ(0, v2.Which());
        EXPECT_EQ(10, *(int*)v2.GetDataPointer());
    }
    
    {
        std::string str("bcde");
        Variant v1(const_cast<const std::string&>(str));
        EXPECT_FALSE(v1.IsEmpty());
        EXPECT_EQ(3, v1.Which());
        EXPECT_EQ(str, *(std::string*)v1.GetDataPointer());
        
        Variant v2 = v1;
        EXPECT_FALSE(v2.IsEmpty());
        EXPECT_EQ(3, v2.Which());
        EXPECT_EQ(str, *(std::string*)v2.GetDataPointer());
    }
    
}

TEST(Variant, MoveConstruction)
{
    typedef flex_lib::Variant<int, double, char, std::string> Variant;
    
    {
        Variant v1(10);
        EXPECT_FALSE(v1.IsEmpty());
        EXPECT_EQ(0, v1.Which());
        EXPECT_EQ(10, *(int*)v1.GetDataPointer());
        
        Variant v2(std::move(v1));
        EXPECT_FALSE(v2.IsEmpty());
        EXPECT_EQ(0, v2.Which());
        EXPECT_EQ(10, *(int*)v2.GetDataPointer());
        EXPECT_TRUE(v1.IsEmpty());
    }
    
    {
        std::string str("bcde");
        Variant v1(const_cast<const std::string&>(str));
        EXPECT_FALSE(v1.IsEmpty());
        EXPECT_EQ(3, v1.Which());
        EXPECT_EQ(str, *(std::string*)v1.GetDataPointer());
        
        Variant v2(std::move(v1));
        EXPECT_FALSE(v2.IsEmpty());
        EXPECT_EQ(3, v2.Which());
        EXPECT_EQ(str, *(std::string*)v2.GetDataPointer());
        EXPECT_TRUE(v1.IsEmpty());
    }
    
}

TEST(Variant, Destruction)
{
    int destroyCount = 0;
   
    struct TestStruct
    {
        TestStruct(int *destroyFlag) : m_destroyed(destroyFlag) {;}
        TestStruct(const TestStruct &other) : m_destroyed(other.m_destroyed)
        {
        }
        TestStruct(TestStruct &&other) : m_destroyed(other.m_destroyed)
        {
            other.m_destroyed = nullptr;
        }

        ~TestStruct() 
        {
            if (m_destroyed != nullptr)
                (*m_destroyed) ++;
        }
        
        TestStruct& operator = (const TestStruct &other)
        {
            TestStruct temp(other);
            std::swap(temp.m_destroyed, m_destroyed);
            return *this;
        }
        
        TestStruct& operator = (TestStruct &&other)
        {
            TestStruct temp(std::move(other));
            std::swap(temp.m_destroyed, m_destroyed);
            return *this;
        }
        
        int *m_destroyed;
    };
    
    typedef flex_lib::Variant<TestStruct> Variant;
    
    {
        Variant v{TestStruct(&destroyCount)};
        EXPECT_FALSE(v.IsEmpty());
        EXPECT_EQ(0, v.Which());
        EXPECT_EQ(1, destroyCount);
    }
    EXPECT_EQ(2, destroyCount);
    
    destroyCount = 0;
    
    {
        Variant v1{TestStruct(&destroyCount)};
        EXPECT_FALSE(v1.IsEmpty());
        EXPECT_EQ(0, v1.Which());
        EXPECT_EQ(1, destroyCount);
        
        Variant v2(std::move(v1));
        EXPECT_TRUE(v1.IsEmpty());
        EXPECT_FALSE(v2.IsEmpty());
        EXPECT_EQ(0, v2.Which());
        EXPECT_EQ(1, destroyCount);
    }
    EXPECT_EQ(2, destroyCount);
    
    destroyCount = 0;
    
    {
        Variant v1{TestStruct(&destroyCount)};
        EXPECT_FALSE(v1.IsEmpty());
        EXPECT_EQ(0, v1.Which());
        EXPECT_EQ(1, destroyCount);
        
        Variant v2;
        v2 = std::move(v1);
        EXPECT_TRUE(v1.IsEmpty());
        EXPECT_FALSE(v2.IsEmpty());
        EXPECT_EQ(0, v2.Which());
        EXPECT_EQ(1, destroyCount);
    }
    EXPECT_EQ(2, destroyCount);
}

TEST(Variant, CopyAssignment)
{
    typedef flex_lib::Variant<int, double, char, std::string> Variant;
    
    {
        Variant v1(10);
        EXPECT_FALSE(v1.IsEmpty());
        EXPECT_EQ(0, v1.Which());
        EXPECT_EQ(10, *(int*)v1.GetDataPointer());
        
        Variant v2;
        EXPECT_TRUE(v2.IsEmpty());
        
        v2 = v1;
        EXPECT_FALSE(v2.IsEmpty());
        EXPECT_EQ(0, v2.Which());
        EXPECT_EQ(10, *(int*)v2.GetDataPointer());
        EXPECT_FALSE(v1.IsEmpty());
    }
    
    {
        std::string str("bcde");
        Variant v1(const_cast<const std::string&>(str));
        EXPECT_FALSE(v1.IsEmpty());
        EXPECT_EQ(3, v1.Which());
        EXPECT_EQ(str, *(std::string*)v1.GetDataPointer());
        
        Variant v2;
        EXPECT_TRUE(v2.IsEmpty());
        
        v2 = v1;
        EXPECT_FALSE(v2.IsEmpty());
        EXPECT_EQ(3, v2.Which());
        EXPECT_EQ(str, *(std::string*)v2.GetDataPointer());
    }
    
}

TEST(Variant, MoveAssignment)
{
    typedef flex_lib::Variant<int, double, char, std::string> Variant;
    
    {
        Variant v1(10);
        EXPECT_FALSE(v1.IsEmpty());
        EXPECT_EQ(0, v1.Which());
        EXPECT_EQ(10, *(int*)v1.GetDataPointer());
        
        Variant v2;
        EXPECT_TRUE(v2.IsEmpty());
        
        v2 = std::move(v1);
        EXPECT_FALSE(v2.IsEmpty());
        EXPECT_EQ(0, v2.Which());
        EXPECT_EQ(10, *(int*)v2.GetDataPointer());
        EXPECT_TRUE(v1.IsEmpty());
    }
    
    {
        std::string str("bcde");
        Variant v1(const_cast<const std::string&>(str));
        EXPECT_FALSE(v1.IsEmpty());
        EXPECT_EQ(3, v1.Which());
        EXPECT_EQ(str, *(std::string*)v1.GetDataPointer());
        
        Variant v2;
        EXPECT_TRUE(v2.IsEmpty());
        
        v2 = std::move(v1);
        EXPECT_FALSE(v2.IsEmpty());
        EXPECT_EQ(3, v2.Which());
        EXPECT_EQ(str, *(std::string*)v2.GetDataPointer());
        EXPECT_TRUE(v1.IsEmpty());
    }
    
}

TEST(Variant, ValueGetter)
{
    typedef flex_lib::Variant<int, double, char, std::string> Variant;
    
    {
        Variant v(10);
        EXPECT_FALSE(v.IsEmpty());
        EXPECT_EQ(0, v.Which());

        auto val = flex_lib::get<int>(v);
        EXPECT_TRUE(std::is_same(decltype(val), int));
        EXPECT_EQ(10, val);
    }
    
    /*
    {
        Variant v(20.0);
        EXPECT_FALSE(v.IsEmpty());
        EXPECT_EQ(1, v.Which());
        EXPECT_EQ(20.0, *(double*)v.GetDataPointer());
    }
    
    {
        Variant v('0');
        EXPECT_FALSE(v.IsEmpty());
        EXPECT_EQ(2, v.Which());
        EXPECT_EQ('0', *(char*)v.GetDataPointer());
    }
    
    {
        Variant v("abcd");
        EXPECT_FALSE(v.IsEmpty());
        EXPECT_EQ(3, v.Which());
        EXPECT_EQ(std::string("abcd"), *(std::string*)v.GetDataPointer());
    }
    
    {
        std::string str("bcde");
        Variant v(const_cast<const std::string&>(str));
        EXPECT_FALSE(v.IsEmpty());
        EXPECT_EQ(3, v.Which());
        EXPECT_EQ(str, *(std::string*)v.GetDataPointer());
    }
    */
}


int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
