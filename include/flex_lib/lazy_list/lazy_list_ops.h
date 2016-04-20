#ifndef FLEX_LIB_LAZY_LIST_OPS_H
#define FLEX_LIB_LAZY_LIST_OPS_H

#include "lazy_list_impl.h"
#include "lazy_list_factory.h"

namespace flex_lib
{

template<typename NT>
lazy_list<NT> tail(const lazy_list<NT> &ll)
{
    typedef node_producer<NT> NodeEvaluator;
    return make_llist<NT>([is_first_time = true, b = ll.begin(), e = ll.end()](bool &isEos) mutable -> NodeEvaluator
    {
        if (is_first_time)
        {
            is_first_time = false;
            if (b != e)
                ++ b;
        }
        else
            ++ b;
        
        if (b == e)
        {
            isEos = true;
            return NodeEvaluator();
        }
        
        return [b](){return *b;};
    });
}

template<typename NT, typename V>
lazy_list<NT> cons(const lazy_list<NT> &ll, V &&val)
{
    typedef node_producer<NT> NodeEvaluator;

    return make_llist<NT>([val = std::forward<V>(val), is_first_time = 2, b = ll.begin(), e = ll.end()](bool &isEos) mutable -> NodeEvaluator
    {
        if (is_first_time == 2)
        {
            is_first_time --;
            return [val = std::forward<V>(val)](){return val;};
        }
        else if (is_first_time == 0)
            ++ b;
        else
            is_first_time --;
            
        if (b == e)
        {
            isEos = true;
            return NodeEvaluator();
        }
        
        return [&b](){return *b;};
    });
}

}

#endif // FLEX_LIB_LAZY_LIST_OPS_H
