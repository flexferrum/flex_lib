#ifndef FLEX_LIB_LAZY_LIST_FACTORY_H
#define FLEX_LIB_LAZY_LIST_FACTORY_H

#include "common.h"
#include "lazy_list_builder.h"
#include <functional>
#include <type_traits>
#include <iterator>

namespace flex_lib
{
#if 0
namespace detail
{
template<typename VT, typename It>
auto make_rangebased_llist(It from, It to) -> lazy_list<VT>
{
    auto ctor = lazy_list_constructor<VT, false>{
        [from, to](bool &isFinal) mutable
        {
            if (from == to)
            {
                isFinal = true;
                return VT();
            }
 
            return *(from ++);
        }
     };
    return lazy_list<VT>(std::move(ctor));
}


template<typename VT, typename FT, bool isLazy>
struct IsNodeEvaluator
{
    template<typename U>
    static auto GetResult(U *fn, bool *b) -> decltype((*fn)(*b));
    static void GetResult(...);

    typedef typename lazy_list_constructor<VT, isLazy>::NodeIniter NodeIniter;
    typedef decltype(GetResult((FT*)nullptr, nullptr)) FTResult;
    typedef decltype(GetResult((NodeIniter*)nullptr, nullptr)) NIResult;

    enum {value = std::is_same_v<NIResult, FTResult>/*::value*/};
};

template<typename VT, typename FT>
struct FunctionTypeTest
{
    enum {value = IsNodeEvaluator<VT, FT, false>::value || IsNodeEvaluator<VT, FT, true>::value};
};

}

template<typename T>
auto make_llist(T* from, T* to) -> decltype(detail::make_rangebased_llist<T, T*>(from, to))
{
    return detail::make_rangebased_llist<T>(from, to);
}

template<typename T, size_t N>
auto make_llist(T (&arr)[N]) -> decltype(detail::make_rangebased_llist<T, T*>(arr, arr + N))
{
    return detail::make_rangebased_llist<T>(arr, arr + N);
}

template<typename It>
auto make_llist(It from, It to) -> decltype(detail::make_rangebased_llist<typename It::value_type, It>(from, to))
{
    return detail::make_rangebased_llist<typename It::value_type, It>(from, to);
}

template<typename C>
auto make_llist(C &cont) -> decltype(detail::make_rangebased_llist<typename C::value_type, typename C::iterator>(cont.begin(), cont.end()))
{
    return detail::make_rangebased_llist<typename C::value_type, typename C::iterator>(begin(cont), end(cont));
}

template<typename C>
auto make_llist(const C &cont) -> decltype(detail::make_rangebased_llist<typename C::value_type, typename C::const_iterator>(cont.begin(), cont.end()))
{
    return detail::make_rangebased_llist<typename C::value_type, typename C::const_iterator>(begin(cont), end(cont));
}

template<typename VT, typename FT>
std::enable_if_t<detail::FunctionTypeTest<VT, FT>::value, lazy_list<VT>> make_llist(FT && fn)
{
    auto ctor = lazy_list_constructor<VT, detail::IsNodeEvaluator<VT, FT, true>::value>{
        std::forward<FT>(fn)
     };
    return lazy_list<VT>(std::move(ctor));    
}
#endif


template<typename It>
auto lazy_from(It b, It e)
{
    typedef typename std::iterator_traits<It>::value_type VT;
    // typedef typename It::value_type VT;
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

template<typename Gen>
auto lazy_generate(Gen &&gen)
{
    typedef std::decay_t<decltype(gen(*(bool*)nullptr))> VT;
    return detail::MakeLazyListBuilder<VT>(std::forward<Gen>(gen));
}
}

#endif // FLEX_LIB_LAZY_LIST_FACTORY_H
