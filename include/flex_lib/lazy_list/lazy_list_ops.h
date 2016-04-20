#ifndef FLEX_LIB_LAZY_LIST_OPS_H
#define FLEX_LIB_LAZY_LIST_OPS_H

#include "lazy_list_impl.h"
#include "lazy_list_factory.h"

namespace flex_lib
{

template<typename NT>
lazy_list<NT> tail(const lazy_list<NT> &ll)
{
    return fl::lazy_from(ll).tail().done();
}

template<typename NT, typename V>
lazy_list<NT> cons(const lazy_list<NT> &ll, V &&val)
{
    return fl::lazy_from(ll).cons(val).done();
}

template<typename NT, typename Fn>
auto filter(const lazy_list<NT> &ll, Fn &&val)
{
    return fl::lazy_from(ll).filter(std::forward<Fn>(val)).done();
}

template<typename NT, typename Fn>
auto map(const lazy_list<NT> &ll, Fn &&val)
{
    return fl::lazy_from(ll).map(std::forward<Fn>(val)).done();
}

template<typename NT1, typename NT2, typename Fn>
auto zip(const lazy_list<NT1> &l1, const lazy_list<NT2> &l2, Fn &&val)
{
    return fl::lazy_from(l1).zip(l2, std::forward<Fn>(val)).done();
}

template<typename NT, typename Fn>
auto zip_self(const lazy_list<NT> &ll, Fn &&val)
{
    return fl::lazy_from(ll).zip_self(std::forward<Fn>(val)).done();
}
}

#endif // FLEX_LIB_LAZY_LIST_OPS_H
