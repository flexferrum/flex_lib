#ifndef FLEX_LIB_LAZY_LIST_ITERATOR_H
#define FLEX_LIB_LAZY_LIST_ITERATOR_H

namespace flex_lib
{
template<typename LL>
class lazy_list_iterator
{
    typedef typename LL::inner_list_t::const_iterator	inner_iterator;
public:
    typedef lazy_list_iterator<LL> 				this_type;
    typedef typename LL::node_type_t          	node_type_t;

    typedef typename inner_iterator::difference_type	difference_type;
    typedef std::forward_iterator_tag    				iterator_category;
    typedef typename LL::value_type             		value_type;
    typedef typename LL::value_type const*              pointer;
    typedef typename LL::value_type const&              reference;

    lazy_list_iterator(LL const& list) : iterating_list(&list), is_end(true) {;}
    lazy_list_iterator(LL const& list, inner_iterator it) : iterating_list(&list), current(it), is_end(false) {;}

    reference operator*() const
    {
        current->construct(*iterating_list);
        return current->dereference();
    }

    pointer operator->() const
    {
        current->construct(*iterating_list);
        return &current->dereference();
    }

    this_type& operator ++()
    {
        if (current->is_final_node)
            return *this;

        inner_iterator it = current ++;
        if (current == iterating_list->inner_list.end() || !current->is_constructed)
        {
            if (current != iterating_list->inner_list.end())
            {
                current->construct(*iterating_list);
            }
            else
            {
                it->construct(*iterating_list);
                current = ++ it;
            }
        }

        return *this;
    }

    this_type operator ++(int)
    {
        this_type tmp(*this);
        ++(*this);
        return tmp;
    }

    bool operator == (this_type const& other) const
    {
        if (is_end)
        {
            if (other.is_end)
                return true;

            if (other.iterating_list != iterating_list)
                return false;

            if (!other.current->is_constructed)
                other.current->construct(*iterating_list);

            return other.current->is_final_node;
        }

        if (!current->is_constructed)
            current->construct(*iterating_list);

        if (other.is_end)
            return other == *this;

        return current == other.current;
    }

    bool operator != (this_type const& other) const
    {
        return !(*this == other);
    }

    this_type& operator = (this_type const& other)
    {
        if (this == &other)
            return *this;

        iterating_list = other.iterating_list;
        current = other.current;
        is_end = other.is_end;
        return *this;
    }

    void swap(this_type& other)
    {
        std::swap(iterating_list, other.iterating_list);
        current.swap(other.current);
        std::swap(is_end, other.is_end);
    }

private:
    LL const* iterating_list;
    inner_iterator current;
    bool is_end;
};

// #include "lazy_list.inl.h"

}

#endif // FLEX_LIB_LAZY_LIST_ITERATOR_H
