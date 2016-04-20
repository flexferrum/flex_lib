#ifndef FLEX_LIB_LAZY_LIST_FACTORY_H
#define FLEX_LIB_LAZY_LIST_FACTORY_H

#include "common.h"
#include "lazy_list_builder.h"
#include <functional>
#include <type_traits>
#include <iterator>

namespace flex_lib
{
template<typename It>
auto lazy_from(It b, It e)
{
    typedef typename std::iterator_traits<It>::value_type VT;
    auto gen = [b, e](bool &isEos) mutable
    {
        if (b == e)
        {
            isEos = true;
            return VT();
        }
        
        return *b ++;
    };
    
    return detail::MakeLazyListBuilder<VT>(std::move(gen));
}

template<typename VT, size_t SZ>
auto lazy_from(VT (&items)[SZ])
{
    auto first = &items[0];
    return lazy_from(first, first + SZ);
}

template<typename C>
auto lazy_from(C &cont) -> decltype(lazy_from<typename C::iterator>(cont.begin(), cont.end()))
{
    return lazy_from(cont.begin(), cont.end());
}

template<typename C>
auto lazy_from(const C &cont) -> decltype(lazy_from<typename C::const_iterator>(cont.begin(), cont.end()))
{
    return lazy_from(cont.begin(), cont.end());
}

template<typename Gen>
auto lazy_generate(Gen &&gen)
{
    typedef std::decay_t<decltype(gen(*(bool*)nullptr))> VT;
    return detail::MakeLazyListBuilder<VT>(std::forward<Gen>(gen));
}

template<typename T>
auto lazy_sequence(size_t n, T start = 0, T step = 1)
{
    auto gen = [start, step, n = 0, max = n](bool &isEos) mutable
    {
        if (n == max)
        {
            isEos = true;
            return T();
        }
        n ++;
        auto cur = start;
        start += step;
        
        return cur;
    };
    
    return detail::MakeLazyListBuilder<T>(std::move(gen));
}
}

#endif // FLEX_LIB_LAZY_LIST_FACTORY_H
