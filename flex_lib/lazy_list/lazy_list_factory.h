#ifndef FLEX_LIB_LAZY_LIST_FACTORY_H
#define FLEX_LIB_LAZY_LIST_FACTORY_H

#include "common.h"
#include <functional>
#include <type_traits>

namespace flex_lib
{
namespace detail
{
template<typename VT, typename It>
auto make_rangebased_llist(It from, It to) -> lazy_list<VT>
{
    auto ctor = lazy_list_constructor<VT, false>{
        [from, to](VT& val) mutable -> bool
        {
            if (from == to)
                return false;
 
            val = *from;
            ++ from;
            return true;
        }
     };
    return lazy_list<VT>(std::move(ctor));
}


template<typename VT, typename FT>
struct IsLazyEvaluator
{
    typedef typename lazy_list_constructor<VT, true>::NodeEvaluator NodeEvaluator;
    
    template<typename U>
    static auto Test(U *fn) -> decltype((*fn)(*(NodeEvaluator*)nullptr));
    static void Test(...);
    
    enum {value = !std::is_same<decltype(Test((FT*)nullptr)), void>::value};
};


template<typename VT, typename FT>
struct IsValueEvaluator
{
    template<typename U>
    static auto Test(U *fn) -> decltype((*fn)(*(VT*)nullptr));
    static void Test(...);
    
    enum {value = !std::is_same<decltype(Test((FT*)nullptr)), void>::value};
};

template<typename VT, typename FT>
struct FunctionTypeTest
{
    enum {value = IsLazyEvaluator<VT, FT>::value || IsValueEvaluator<VT, FT>::value};
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
typename std::enable_if<detail::FunctionTypeTest<VT, FT>::value, lazy_list<VT>>::type  make_llist(FT && fn)
{
    auto ctor = lazy_list_constructor<VT, detail::IsLazyEvaluator<VT, FT>::value>{
        std::move(fn)
     };
    return lazy_list<VT>(std::move(ctor));    
}
}

#endif // FLEX_LIB_LAZY_LIST_FACTORY_H
