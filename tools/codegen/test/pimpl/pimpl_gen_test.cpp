#include <gtest/gtest.h>

#include "test_pimpl.h"

#include <array>
#include <iterator>

class TestPimplImpl
{
public:
    TestPimplImpl(uint32_t number)
        : m_intNumber(number)
    {        
    }

    TestPimplImpl(std::string number)
        : m_strNumber(number)
    {
    }
    
    TestPimplImpl(TestMoveable&& moveable)
        : m_moveableObj(std::move(moveable))
    {
    }
    
    const std::string GetString() const
    {
        return m_strNumber;
    }
    const uint32_t GetNumber() const
    {
        return m_intNumber;
    }
    void SetGeneratedValues(unsigned values[10])
    {
        std::copy(values, values + 10, begin(m_values));
    }
    const std::array<unsigned, 10> GetGeneratedValues() const
    {
        return m_values;
    }
    void ResetValues(int num, std::string str)
    {
        m_intNumber = num;
        m_strNumber = str;
    }

    PimplMode GetCurrentMode() const
    {
        return m_curMode;
    }
    const int* GetMoveablePtr() const
    {
        return m_moveableObj.GetObject();
    }
    
private:
    uint32_t m_intNumber;
    std::string m_strNumber;
    std::array<unsigned, 10> m_values;
    PimplMode m_curMode = AbnormalMode;
    TestMoveable m_moveableObj = TestMoveable(5);
};

#include <generated/pimpl_gen.h>

TEST(PimplGen, StrConstructor_Successfull)
{
    TestPimpl pimpl("1234");
    
    EXPECT_EQ("1234", pimpl.GetString());
}

TEST(PimplGen, IntConstructor_Successfull)
{
    TestPimpl pimpl(1234);
    
    EXPECT_EQ(1234, pimpl.GetNumber());
}

TEST(PimplGen, ArraySet_Successfull)
{
    unsigned numbers[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    TestPimpl pimpl(1234);
    
    pimpl.SetGeneratedValues(numbers);
    
    auto vals = pimpl.GetGeneratedValues();
    auto srcBeg = numbers;
    auto srcEnd = numbers + 10;
    auto dstBeg = vals.begin();
    auto dstEnd = vals.end();
    
    auto itPair = std::mismatch(srcBeg, srcEnd, dstBeg, dstEnd);
    EXPECT_TRUE(itPair.first == srcEnd);
    EXPECT_TRUE(itPair.second == dstEnd);
}

TEST(PimplGen, MoveOperations_Successfull)
{
    TestMoveable moveable;
    auto srcObjPtr = moveable.GetObject();
    
    TestPimpl pimpl(std::move(moveable));
    
    auto destObjPtr = pimpl.GetMoveablePtr();
    
    EXPECT_EQ(*srcObjPtr, *destObjPtr);
    EXPECT_EQ(srcObjPtr, destObjPtr);
}
