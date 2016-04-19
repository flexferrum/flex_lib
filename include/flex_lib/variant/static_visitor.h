#ifndef FLEX_LIB_VARIANT_STATIC_VISITOR_H
#define FLEX_LIB_VARIANT_STATIC_VISITOR_H

#include <type_traits>
#include "variant_impl.h"

namespace flex_lib
{
// visitor(*(const typename detail::VariantTypesEnumerator<Types...>::FirstType::type *)nullptr)()
template<typename VFn, typename ... Types>
auto apply_visitor(Variant<Types...>& v, const VFn& visitor) -> typename std::result_of<VFn&(typename detail::VariantTypesEnumerator<Types...>::FirstType::type)>::type
{
    typedef typename std::result_of<VFn&(typename detail::VariantTypesEnumerator<Types...>::FirstType::type)>::type R;
    
    return v.template ApplyVisitor<R>(visitor);
}

template<typename VFn, typename ... Types>
auto apply_visitor(const Variant<Types...>& v, const VFn& visitor) -> typename std::result_of<VFn&(typename detail::VariantTypesEnumerator<Types...>::FirstType::type)>::type
{
    typedef typename std::result_of<VFn&(typename detail::VariantTypesEnumerator<Types...>::FirstType::type)>::type R;
    
    return v.template ApplyVisitor<R>(visitor);
}
}

#endif // FLEX_LIB_VARIANT_STATIC_VISITOR_H
