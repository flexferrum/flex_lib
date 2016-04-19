#ifndef FLEX_LIB_LAZY_LIST_COMMON_H
#define FLEX_LIB_LAZY_LIST_COMMON_H

#include <functional>

namespace flex_lib 
{
template<typename VT, bool isLazy>
struct lazy_list_constructor;

template<typename VT>
struct lazy_list_constructor<VT, false>
{
    typedef VT value_type;
    typedef std::function<bool (VT &value)> NodeIniter;    
    NodeIniter node_initer;
    
    template<typename L>
    void Init(L& list)
    {
        list.InitValueList(std::move(node_initer));
    }
};

template<typename VT>
struct lazy_list_constructor<VT, true>
{
    typedef VT value_type;
    typedef std::function<VT ()> NodeEvaluator;
    typedef std::function<bool (NodeEvaluator &eval)> NodeIniter;    
    NodeIniter node_initer;
    
    template<typename L>
    void Init(L& list)
    {
        list.InitLazyList(std::move(node_initer));
    }
};
}

#endif // FLEX_LIB_LAZY_LIST_COMMON_H
