#ifndef FLEX_LIB_LAZY_LIST_IMPL_H
#define FLEX_LIB_LAZY_LIST_IMPL_H

#include "common.h"

#include <list>
#include <functional>
#include "../variant.h"

namespace flex_lib 
{
template<typename LL>
class lazy_list_iterator;

template<typename NT, template <typename> class A = std::allocator> class lazy_list
{
public:
    typedef typename lazy_list_constructor<NT, true>::NodeEvaluator NodeEvaluator;

private:
    typedef typename lazy_list_constructor<NT, true>::NodeIniter LazyNodeIniter;
    typedef typename lazy_list_constructor<NT, false>::NodeIniter ValueNodeIniter;
    typedef lazy_list<NT, A> this_type;

    template<typename U>
    struct lazy_list_node
    {
        typedef std::function<bool (this_type*, lazy_list_node&)> NodeConstructor;
        mutable U node_value;

        NodeConstructor node_ctor;
        NodeEvaluator node_evtor;
        mutable bool is_evaluated;
        mutable bool is_constructed;
        mutable bool is_final_node;

        U const& dereference() const
        {
            if (!is_evaluated)
            {
                node_value = node_evtor();
                is_evaluated = true;
            }

            return node_value;
        }

        void construct(this_type const& l) const
        {
            if (is_constructed)
                return;

            is_final_node = !node_ctor(const_cast<this_type*>(&l), const_cast<lazy_list_node<U>&>(*this));
            is_constructed = true;
        }

        lazy_list_node() : is_evaluated(false), is_constructed(false), is_final_node(false) {;}
        lazy_list_node(NodeConstructor const& ctor) : is_evaluated(false), is_constructed(false), is_final_node(false), node_ctor(ctor) {;}
    };


public:
    typedef NT value_type;
    typedef const value_type const_value_type;
    typedef value_type& reference_type;
    typedef value_type const& const_reference_type;
    typedef NT* pointer_type;
    typedef NT const* const_pointer_type;
    typedef lazy_list_node<NT> node_type_t;
    typedef std::list<node_type_t, A<node_type_t> > inner_list_t;
    typedef lazy_list_iterator<this_type> const_iterator;
    typedef const_iterator iterator;

    lazy_list() 
    {
        InitValueList([](NT &) {return false;});
    }

    template<typename VT, bool isLazy>
    lazy_list(lazy_list_constructor<VT, isLazy> &&ctor)
    {
        ctor.Init(*this);
    }
    
    const_iterator begin() const {return const_iterator(*this, inner_list.begin());}
    const_iterator end() const {return const_iterator(*this);}
    const_iterator constructed_end() const {return const_iterator(*this, -- inner_list.end());}

    const_iterator cbegin() const {return begin();}
    const_iterator cend() const {return end();}

    const_reference_type front() const {return *begin();}

    const_reference_type head() const {return front();}

    // this_type tail() const;

    template<typename F, typename A1, typename A2>
    struct result_of
    {
        static F f;
        typedef decltype(f(A1(), A2())) type;
    };

    bool empty() const
    {
        return begin() == end();
    }

    size_t size() const
    {
        return std::distance(begin(), end());
    }

private:
    mutable inner_list_t inner_list;
    friend class lazy_list_iterator<this_type>;
    
    struct NodeIniterHolder
    {
        Variant<LazyNodeIniter, ValueNodeIniter> m_initer;
        
        bool IsLazy() const {return m_initer.Which() == 0;}        
        LazyNodeIniter *GetLazyIniter() {return get<LazyNodeIniter>(&m_initer);}
        ValueNodeIniter *GetValueIniter() {return get<ValueNodeIniter>(&m_initer);}
        
        template<typename T>
        void SetIniter(T &&initer)
        {
            m_initer = std::move(initer);
        }
    } m_nodeIniter;

    bool NodeConstructor(this_type* list, node_type_t& node)
    {
        // std::cout << "Construct next node" << std::endl;

        bool is_final_node;
        if (m_nodeIniter.IsLazy())
            is_final_node = !(*m_nodeIniter.GetLazyIniter())(node.node_evtor);
        else
        {
            is_final_node = !(*m_nodeIniter.GetValueIniter())(node.node_value);
            node.is_evaluated = true;
        }
        inner_list.emplace_back(node_type_t([](this_type* list, node_type_t& node) -> bool
        {
            return list->NodeConstructor(list, node);
        }));

        return !is_final_node;
    }

    void InitLazyList(LazyNodeIniter &&initFn)
    {
        m_nodeIniter.SetIniter(std::move(initFn));
        inner_list.emplace_back(node_type_t([](this_type* list, node_type_t& node) mutable -> bool
        {
            return list->NodeConstructor(list, node);
        }));
    }
    
    void InitValueList(ValueNodeIniter &&initFn)
    {
        m_nodeIniter.SetIniter(std::move(initFn));
        inner_list.push_back(node_type_t([](this_type* list, node_type_t& node) mutable -> bool
        {
            return list->NodeConstructor(list, node);
        }));
    }

    template<typename VT, bool>
    friend class lazy_list_constructor;
};

}

#endif // FLEX_LIB_LAZY_LIST_IMPL_H
