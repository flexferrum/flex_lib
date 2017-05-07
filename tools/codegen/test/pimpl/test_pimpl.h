#ifndef TEST_ENUMS_H
#define TEST_ENUMS_H

#include <flex_lib/pimpl.h>

#include <string>
#include <utility>

class TestPimplImpl;

enum PimplMode
{
    NormalMode,
    AbnormalMode
};

class TestPimpl : flex_lib::pimpl<TestPimplImpl>
{
public:
    explicit TestPimpl(uint32_t number = 0);
    explicit TestPimpl(std::string&& number);
    ~TestPimpl();
    
    const std::string GetString() const;
    const uint32_t GetNumber() const;
    void SetGeneratedValues(unsigned values[10]);
    const std::array<unsigned, 10> GetGeneratedValues() const;
    void ResetValues(int num, std::string str);
    PimplMode GetCurrentMode() const;
    
};

#endif // TEST_ENUMS_H
