#ifndef FLEX_LIB_LAZY_LIST_BUILDER_H
#define FLEX_LIB_LAZY_LIST_BUILDER_H

#include "common.h"
#include "lazy_list_impl.h"

#include <memory>

namespace flex_lib {
namespace detail {

template<typename VT, typename Gen>
auto MakeLazyListBuilder(Gen &&gen);

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
    
    lazy_list<VT> done()
    {
        typedef LazyListConstructorBase<VT> ConstructorBase;
        typedef LazyListConstructor<VT, Gen> Constructor;
        auto ctor = std::make_shared<Constructor>(std::move(m_gen));
        
        return lazy_list<VT>(std::static_pointer_cast<ConstructorBase>(ctor));
    }
    
    operator lazy_list<VT>()
    {
        return done();
    }
    
    template<typename VT1, typename = std::enable_if_t<!std::is_same_v<VT, VT1>>>
    operator lazy_list<VT1>()
    {
        return map([](const VT &v) {return VT1(v);}).done();
    }
    
    auto tail()
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
    auto cons(const U& val)
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
    
    template<typename Fn>
    auto filter(Fn &&filter)
    {
        auto newGen = [filter = std::forward<Fn>(filter), gen = std::move(m_gen)](bool &isEos) mutable
        {
            do
            {
                auto val = gen(isEos);
                if (!isEos && filter(val))
                    return val;
                
            } while (!isEos);
            
            return VT();
        };
        
        return MakeLazyListBuilder<VT>(std::move(newGen));
    }
    
    template<typename Fn>
    auto map(Fn &&mapper)
    {
        typedef std::decay_t<decltype(mapper(VT()))> MVT;
        
        auto newGen = [mapper = std::forward<Fn>(mapper), gen = std::move(m_gen)](bool &isEos) mutable
        {
            auto val = gen(isEos);
            if (!isEos)
                return mapper(val);
            
            return MVT();
        };
        
        return MakeLazyListBuilder<MVT>(std::move(newGen));
    }
    
    template<typename VT2, typename Fn>
    auto zip(const lazy_list<VT2> &l2, Fn &&zipper)
    {
        typedef std::decay_t<decltype(zipper(VT(), VT2()))> ZVT;
        
        auto newGen = [zipper = std::forward<Fn>(zipper), b = l2.begin(), e = l2.end(), gen = std::move(m_gen)](bool &isEos) mutable
        {
            if (b != e)
            {
                auto v2 = *b ++;
                auto v1 = gen(isEos);
                if (!isEos)
                    return zipper(v1, v2);
            }
            else
                isEos = true;
            
            return ZVT();
        };
        
        return MakeLazyListBuilder<ZVT>(std::move(newGen));
    }    
    
    template<typename Fn>
    auto zip_self(Fn &&zipper)
    {
        typedef std::decay_t<decltype(zipper(VT(), VT()))> ZVT;
        
        auto newGen = [zipper = std::forward<Fn>(zipper), gen = std::move(m_gen)](bool &isEos) mutable
        {
            auto v = gen(isEos);
            if (!isEos)
                return zipper(v, v);
            
            return ZVT();
        };
        
        return MakeLazyListBuilder<ZVT>(std::move(newGen));
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
