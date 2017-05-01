#ifndef TEST_ENUMS_H
#define TEST_ENUMS_H

#include <flex_lib/pimpl.h>

#include <string>
#include <utility>

class TestPimplImpl;

class TestPimpl : flex_lib::pimpl<TestPimplImpl>
{
public:
    explicit TestPimpl(int number = 0);
    explicit TestPimpl(const std::string& number);
    ~TestPimpl();
    
    const std::string GetString() const;
    const int GetNumber() const;
    void ResetValues(int num, std::string str);
};

#endif // TEST_ENUMS_H
