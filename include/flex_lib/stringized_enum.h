#ifndef FLEX_LIB_STRINGIZED_ENUM_H__
#define FLEX_LIB_STRINGIZED_ENUM_H__

#include <utility>
#include <stdexcept>
#include <algorithm>
#include <sstream>

namespace flex_lib
{

class bad_enum_name : public std::logic_error
{
public:
    bad_enum_name(std::string description) : std::logic_error(std::move(description))  {}
    
    static void Throw(const char* itemName, const char* enumName)
    {
        std::ostringstream os;
        os << "Can't convert string \"" << itemName << "\" into value of enum '" << enumName << "'";
        throw flex_lib::bad_enum_name(os.str());        
    }
};

namespace detail 
{
template<typename EnumType, size_t N>
bool String2Enum(const char* itemName, std::pair<const char*, EnumType> (&items)[N], EnumType& value)
{
    auto p = std::lower_bound(begin(items), end(items), itemName,
                      [](auto&& i, auto&& v) {return strcmp(i.first, v) < 0;});
    
    if (p == end(items) || strcmp(p->first, itemName) != 0)
        return false;
    
    value = p->second;
    return true;    
}
} // detail

template<typename E>
const char* Enum2String(E e);

template<typename E>
E String2Enum(const char* name);

} // flex_lib

#endif // FLEX_LIB_STRINGIZED_ENUM_H__
