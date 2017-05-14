#ifndef FLEX_LIB_LAZY_LIST_IMPL_H
#define FLEX_LIB_LAZY_LIST_IMPL_H

#include "common.h"

#include <list>
#include <functional>
#include <memory>
#include "../variant.h"

namespace flex_lib 
{
template<typename LL>
class lazy_list_iterator;

template<typename NT> class lazy_list
{
public:
    typedef LazyListConstructorBase<NT> ListConstructor;
    typedef std::shared_ptr<ListConstructor> ListConstructorPtr;

private:
    typedef lazy_list<NT> this_type;

public:
    typedef NT value_type;
    typedef const value_type const_value_type;
    typedef value_type& reference_type;
    typedef value_type const& const_reference_type;
    typedef NT* pointer_type;
    typedef NT const* const_pointer_type;
    typedef typename ListConstructor::ListNodeType node_type_t;
    typedef typename ListConstructor::ListType inner_list_t;
    typedef lazy_list_iterator<this_type> const_iterator;
    typedef const_iterator iterator;

    lazy_list() 
    {
    }

    lazy_list(ListConstructorPtr ctor)
        : m_constructor(ctor)
    {
    }
    
    const_iterator begin() const {return !m_constructor ? const_iterator(*this) : const_iterator(*this, m_constructor->m_items.begin());}
    const_iterator end() const {return const_iterator(*this);}
    const_iterator constructed_end() const {return !m_constructor ? const_iterator(*this) : const_iterator(*this, -- m_constructor->m_items.end());}

    const_iterator cbegin() const {return begin();}
    const_iterator cend() const {return end();}

    const_reference_type front() const {return *begin();}

    const_reference_type head() const {return front();}

    bool empty() const
    {
        return !m_constructor || begin() == end();
    }

    size_t size() const
    {
        return std::distance(begin(), end());
    }

private:
    mutable ListConstructorPtr m_constructor;
    friend class lazy_list_iterator<this_type>;

};
}

#endif // FLEX_LIB_LAZY_LIST_IMPL_H
