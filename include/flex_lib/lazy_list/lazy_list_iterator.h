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

    // TODO: Copy and default constructors

    lazy_list_iterator(LL const& list) : iterating_list(&list), is_end(true) {;}
    lazy_list_iterator(const this_type &other) = default;
    lazy_list_iterator(this_type &&other)
        : iterating_list(other.iterating_list)
        , current(std::move(other.current))
        , is_end(other.is_end)
    {
        other.iterating_list = nullptr;
        other.is_end = true;
    }

    lazy_list_iterator(LL const& list, inner_iterator it) : iterating_list(&list), current(it), is_end(false) {;}

    reference operator*() const
    {
        current->Construct(*iterating_list->m_constructor.get());
        return current->Dereference();
    }

    pointer operator->() const
    {
        current->construct(*iterating_list);
        return &current->dereference();
    }

    this_type& operator ++()
    {
        if (current->m_isFinalNode)
            return *this;

        inner_iterator it = current ++;
        if (current == iterating_list->m_constructor->m_items.end() || !current->m_isConstructed)
        {
            if (current != iterating_list->m_constructor->m_items.end())
            {
                current->Construct(*iterating_list->m_constructor.get());
            }
            else
            {
                it->Construct(*iterating_list->m_constructor.get());
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

            if (!other.current->m_isConstructed)
                other.current->Construct(*iterating_list->m_constructor.get());

            return other.current->m_isFinalNode;
        }

        if (!current->m_isConstructed)
            current->Construct(*iterating_list->m_constructor.get());

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


}

#endif // FLEX_LIB_LAZY_LIST_ITERATOR_H
