#ifndef FLEX_LIB_LAZY_LIST_BUILDER_H
#define FLEX_LIB_LAZY_LIST_BUILDER_H

#include "common.h"
#include "lazy_list_impl.h"

#include <memory>

namespace flex_lib {
namespace detail {

//template<typename VT, typename Gen>
//auto MakeLazyListBuilder(Gen &&gen);

template<typename VT, typename Gen>
class LazyListBuilder
{
public:
    typedef VT value_type;
    typedef Gen generator_type;
    typedef LazyListBuilder<VT, Gen> this_type;
    
    explicit LazyListBuilder(Gen &&gen)
        : m_gen(std::move(gen))
    {
        ;
    }
    
    explicit LazyListBuilder(const Gen &gen)
        : m_gen(gen)
    {
        ;
    }
    
    LazyListBuilder(this_type &&other)
        : m_gen(std::move(other.m_gen))
    {
        ;
    }
    
    this_type& operator = (this_type &&other)
    {
        m_gen = std::move(other.m_gen);
        return *this;
    }
    
    Gen &getGenerator()
    {
        return m_gen;
    }
    
    lazy_list<VT> toList()
    {
        typedef LazyListConstructorBase<VT> ConstructorBase;
        typedef LazyListConstructor<VT, Gen> Constructor;
        auto ctor = std::make_shared<Constructor>(std::move(m_gen));
        
        return lazy_list<VT>(std::static_pointer_cast<ConstructorBase>(ctor));
    }
    
    auto tail() &&
    {
        auto newGen = [is_first_time = true, gen = std::move(m_gen)](bool &isEos) mutable
        {
            if (is_first_time)
            {
                is_first_time = false;
                gen(isEos);
                if (isEos)
                    return VT();
            }
            
            return gen(isEos);
        };
        
        return MakeLazyListBuilder<VT>(std::move(newGen));
    }
    
    template<typename U>
    auto cons(const U& val) &&
    {
        auto newGen = [is_first_time = true, gen = std::move(m_gen), val](bool &isEos) mutable
        {
            if (is_first_time)
            {
                is_first_time = false;
                return VT(val);
            }
            
            return gen(isEos);
        };
        
        return MakeLazyListBuilder<VT>(std::move(newGen));
    }

private:
    Gen m_gen;
};

template<typename VT, typename Gen>
auto MakeLazyListBuilder(Gen &&gen)
{
    typedef std::decay_t<Gen> GenType;
    return LazyListBuilder<VT, Gen>(std::forward<Gen>(gen));
}
}

}

#endif // FLEX_LIB_LAZY_LIST_BUILDER_H
