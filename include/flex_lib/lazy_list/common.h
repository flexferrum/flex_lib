#ifndef FLEX_LIB_LAZY_LIST_COMMON_H
#define FLEX_LIB_LAZY_LIST_COMMON_H

#include <list>

#if defined(Yield)
#undef Yield
#endif

namespace flex_lib 
{
template<typename VT>
struct LazyListConstructorBase
{
    typedef LazyListConstructorBase<VT> this_type;
    template<typename U>
    struct ListNode
    {
        mutable U m_nodeValue;

        mutable bool m_isConstructed = false;
        mutable bool m_isFinalNode = false;

        U const& Dereference() const
        {
            return m_nodeValue;
        }

        void Construct(this_type& ctor) const
        {
            if (m_isConstructed)
                return;

            m_isFinalNode = ctor.Yield(*this);
            m_isConstructed = true;
        }
        
        ListNode(bool isFinal = false)
            : m_isFinalNode(isFinal)
        {
            ;
        }
    };

    typedef ListNode<VT> ListNodeType;
    typedef std::list<ListNodeType> ListType;
    mutable ListType m_items;
    
    LazyListConstructorBase()
    {
        m_items.emplace_back(false);
    }
    
    bool Yield(const ListNodeType &node)
    {
        bool isEos = false;        
        auto val = GetNextItem(isEos);
        if (!isEos)
        {
            node.m_nodeValue = std::move(val);
            m_items.emplace_back(false);
        }
        
        return isEos;
    }
    
private:
    virtual VT GetNextItem(bool &isEos) = 0;
};

template<typename VT, typename Gen>
class LazyListConstructor : public LazyListConstructorBase<VT>
{
public:
    LazyListConstructor(Gen &&gen)
        : m_gen(std::move(gen))
    {
        ;
    }
    
    VT GetNextItem(bool &isEos)
    {
        return m_gen(isEos);
    }
    
private:
    mutable Gen m_gen;
};

#if 0
template<typename VT, bool isLazy>
struct lazy_list_constructor;

template<typename T>
using node_producer = std::function<T ()>;

template<typename VT>
struct lazy_list_constructor<VT, false>
{
    typedef VT value_type;
    typedef std::function<VT (bool &isFinish)> NodeIniter;
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
    typedef node_producer<VT> NodeEvaluator;
    typedef std::function<NodeEvaluator (bool &isFinish)> NodeIniter;
    NodeIniter node_initer;
    
    template<typename L>
    void Init(L& list)
    {
        list.InitLazyList(std::move(node_initer));
    }
};
#endif
}

#endif // FLEX_LIB_LAZY_LIST_COMMON_H
