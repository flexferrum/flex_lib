#ifndef FLEX_LIB_VARIANT_IMPL_H
#define FLEX_LIB_VARIANT_IMPL_H

#include <type_traits>
#include <iostream>
#include <cassert>

namespace flex_lib
{
namespace detail
{
template<int I, typename ... T>
union InnerUnion;

template<int I, typename T, typename ... Tail>
union InnerUnion<I, T, Tail...>
{
    typedef InnerUnion<I, T, Tail ...> this_type;
    typedef InnerUnion<I + 1, Tail ...> next_type;
    
    enum {Index = I};
    char m_Dummy;
    T m_Data;    
    next_type m_Tail;
    
    InnerUnion()
        : m_Dummy()
    {
        ;
    }
    
    ~InnerUnion()
    {
        ;
    }
    
    InnerUnion(const this_type &other) = delete;
    InnerUnion(this_type &&other) = delete;
    
    this_type& operator = (const this_type&) = delete;
    
    void CopyFrom(const this_type& other, int dataType)
    {
        if (dataType == I)    
            new (&m_Data) T(other.m_Data);
        else
            m_Tail.CopyFrom(other.m_Tail, dataType);
    }
    
    void MoveFrom(this_type& other, int dataType)
    {
        if (dataType == I)    
            new (&m_Data) T(std::move(other.m_Data));
        else
            m_Tail.MoveFrom(other.m_Tail, dataType);
    }
    
    void SwapWith(this_type& other, int dataType)
    {
        if (dataType == I)
            std::swap(m_Data, other.m_Data);
        else
            m_Tail.SwapWith(other.m_Tail, dataType);
    }
    
    void Destroy(int dataType)
    {
        if (dataType == I)
            m_Data.~T();
        else
            m_Tail.Destroy(dataType);
    }
    
    template<int Idx, typename U>
    void InitWithCopy(const U& data, typename std::enable_if<Idx == I, void>::type* = nullptr)
    {
        new(&m_Dummy) T(data);
    }
    
    template<int Idx, typename U>
    void InitWithCopy(const U& data, typename std::enable_if<Idx != I, void>::type* = nullptr)
    {
        m_Tail.InitWithCopy<Idx>(data);
    }
    
    template<int Idx, typename U>
    void InitWithMove(U&& data, typename std::enable_if<Idx == I, void>::type* = nullptr)
    {
        new(&m_Dummy) T(std::move(data));
    }
    
    template<int Idx, typename U>
    void InitWithMove(U&& data, typename std::enable_if<Idx != I, void>::type* = nullptr)
    {
        m_Tail.InitWithMove<Idx>(data);
    }        
};

template<int I>
union InnerUnion<I>
{
    enum {Index = I};
    
    void Destroy(int)
    {
        assert(false);
    }
    
    void CopyFrom(const InnerUnion<I>&, int)
    {
        assert(false);
    }
    
    void MoveFrom(InnerUnion<I>&, int)
    {
        assert(false);
    }
    
    void SwapWith(InnerUnion<I>&, int)
    {
        assert(false);
    }
};

enum
{
    EmptyVariant = -1
};

template<typename ... Types>
struct VariantTypesEnumerator;

template<typename ... Types>
struct VariantDataHolder
{
public:
    typedef VariantDataHolder<Types ...> this_type;
    
    VariantDataHolder()
        : m_Data()
    {
        ;
    }
    
    VariantDataHolder(const this_type &other)
        : m_DataType(EmptyVariant)
        , m_Data()
    {
        CopyData(other, other.m_DataType);
    }
    
    VariantDataHolder(this_type &&other)
        : m_DataType(EmptyVariant)
        , m_Data()
    {
        MoveData(other, other.m_DataType);
    }
    
    template<typename U>
    VariantDataHolder(const U& data)
        : m_Data()
    {
        constexpr int type = VariantTypesEnumerator<Types ...>::template MatchType<U>::value;
        m_DataType = type;
        if (m_DataType == -1)
            return;        
        InitializeDataCopy<type>(data);
    }
    
    template<typename U>
    VariantDataHolder(U&& data, int &dataType)
        : m_Data()
    {
        constexpr int type = VariantTypesEnumerator<Types ...>::template MatchType<U>::value;
        m_DataType = type;
        if (m_DataType == -1)
            return;
        InitializeDataMove<type>(data);
    }            
    
    void DestroyData()
    {
        if (m_DataType != EmptyVariant)
            m_Data.Destroy(m_DataType);
    }
    
    void Swap(this_type &other)
    {
        if (m_DataType == other.m_DataType)
        {
            if (m_DataType == -1)
                return;
            
            SwapData(other, m_DataType);
            return;
        }

        if (m_DataType == -1)
            MoveData(other, other.m_DataType);
        else if (other.m_DataType == -1)
            other.MoveData(*this, m_DataType);
        else
        {
            this_type tmp;
            tmp.MoveData(other, other.m_DataType);
            other.MoveData(*this, m_DataType);
            MoveData(tmp, tmp.m_DataType);
        }
    }
    
    template<typename R>
    R GetValue() const
    {
        typedef typename VariantTypesEnumerator<Types ...>::template RealTypeGetter<R>::type result;
        
        return *GetPointer<result>();
    }
    
    template<typename R>
    auto GetPointer() const -> const typename VariantTypesEnumerator<Types ...>::template RealTypeGetter<R>::type *
    {
        typedef typename VariantTypesEnumerator<Types ...>::template RealTypeGetter<R>::type result;
        constexpr int type = VariantTypesEnumerator<Types ...>::template MatchType<result>::value;
        if (type != m_DataType)
            return nullptr;        
        
        return reinterpret_cast<const result *>(&m_Data);
    }
    
    template<typename R>
    auto GetPointer() -> typename VariantTypesEnumerator<Types ...>::template RealTypeGetter<R>::type *
    {
        typedef typename VariantTypesEnumerator<Types ...>::template RealTypeGetter<R>::type result;
        constexpr int type = VariantTypesEnumerator<Types ...>::template MatchType<result>::value;
        if (type != m_DataType)
            return nullptr;        
        
        return reinterpret_cast<result *>(&m_Data);
    }
    
    void * GetRawDataPointer()
    {
        return &m_Data;
    }
    
    int GetDataType() const
    {
        return m_DataType;
    }
    
private:
    void CopyData(const this_type &from, int dataType)
    {
        if (dataType != EmptyVariant)
            m_Data.CopyFrom(from.m_Data, dataType);
        m_DataType = dataType;
    }
    
    void MoveData(this_type &from, int dataType)
    {
        if (dataType != EmptyVariant)
            m_Data.MoveFrom(from.m_Data, dataType);
        m_DataType = dataType;
    }
    
    void SwapData(this_type &from, int dataType)
    {
        if (dataType != EmptyVariant)
            m_Data.SwapWith(from.m_Data, dataType);
        m_DataType = dataType;
    }
    
    template<int I, typename U>
    void InitializeDataCopy(const U &data)
    {
        m_Data.template InitWithCopy<I>(data);
    }
    
    template<int I, typename U>
    void InitializeDataMove(U &&data)
    {
        m_Data.template InitWithMove<I>(data);
    }
      
private:
    int m_DataType;
    InnerUnion<0, Types...> m_Data;
};

template<typename ... Types>
struct VariantTypesEnumerator
{        
    template<typename U>
    struct MatchType
    {
        template<int I, typename ... Tail>
        struct IndexMatcher
        {
            static constexpr int GetMatchedIndex() {return -1;}
            static constexpr int GetExactMatchedIndex() {return -1;}
        };
        
        template<int I, typename T, typename ... Tail>
        struct IndexMatcher<I, T, Tail...>
        {
            static constexpr int GetMatchedIndex()
            {
                return std::is_convertible<U, T>::value ? I : IndexMatcher<I + 1, Tail ...>::GetMatchedIndex();
            }
            
            static constexpr int GetExactMatchedIndex()
            {
                typedef typename std::remove_reference<typename std::remove_cv<U>::type>::type type;
                return std::is_same<type, T>::value ? I : IndexMatcher<I + 1, Tail ...>::GetExactMatchedIndex();
            }
        };
        
        static constexpr int GetMatchedIndex()
        {
            return IndexMatcher<0, Types ...>::GetExactMatchedIndex() != -1 ? IndexMatcher<0, Types ...>::GetExactMatchedIndex() : IndexMatcher<0, Types ...>::GetMatchedIndex();
        }
        
        static constexpr int GetExactMatchedIndex()
        {
            return IndexMatcher<0, Types ...>::GetExactMatchedIndex();
        }
        
        enum {value = GetMatchedIndex()};
        enum {exact_type_value = GetMatchedIndex()};
    };
    
    template<int I, typename ... Tail>
    struct IndexToType;

    template<int I, typename T, typename ... Tail>
    struct IndexToType<I, T, Tail ...>
    {
        typedef typename IndexToType<I - 1, Tail ...>::type type;
    };            
    
    template<typename T, typename ... Tail>
    struct IndexToType<0, T, Tail ...>
    {
        typedef T type;
    };            
    
    template<typename ... Tail>
    struct IndexToType<-1, Tail ...>
    {
    };            
    
    template<typename R>
    struct RealTypeGetter
    {
        typedef typename IndexToType<MatchType<R>::exact_type_value, Types ...>::type type;
    };
};

}

template<typename ... Types>
class Variant
{
public:
    typedef Variant<Types...> this_type;
    
    enum 
    {
        Empty = -1,
        MaxVariantId = sizeof ... (Types)
    };
    
    Variant()
    {
        ;
    }
    
    Variant(const this_type &other)
        : m_Data(other.m_Data)
    {
        ;
    }
    
    Variant(this_type &&other)
        : m_Data(std::move(other.m_Data))
    {
    }
       
    template<typename T>
    Variant(const T& other)
        : m_Data(other)
    {
        
    }
    
    ~Variant()
    {
        m_Data.DestroyData();
    }
    
    void swap(this_type &other)
    {
        m_Data.Swap(other.m_Data);
    }
    
    this_type& operator = (const this_type &other)
    {
        this_type temp(other);
        swap(temp);
        return *this;
    }
    
    this_type& operator = (this_type &&other)
    {
        this_type temp(std::move(other));
        swap(temp);
        return *this;
    }
    
    template<typename U>
    this_type& operator = (const U& val)
    {
        this_type temp(val);
        swap(temp);
        return *this;
    }
    
    bool IsEmpty() const
    {
        return m_Data.GetDataType() == detail::EmptyVariant;
    }
    
    int Which() const 
    {
        return m_Data.GetDataType();        
    }
    
#if FL_VARIANT_WITH_TEST == 1
    void *GetDataPointer()
    {
        return m_Data.GetRawDataPointer();
    }

#endif
    
private:    
    detail::VariantDataHolder<Types...> m_Data;
    
private:    
    template<typename R, typename ... T>
    friend R get(const Variant<T...>& var);
    
    template<typename R, typename ... T>
    friend const R* get(const Variant<T...>* var);
    
    template<typename R, typename ... T>
    friend R* get(Variant<T...>* var);
};

template<typename R, typename ... T>
R get(const Variant<T...>& var)
{
    return var.m_Data.template GetValue<R>();
}

template<typename R, typename ... T>
const R* get(const Variant<T...>* var)
{
    return var->m_Data.template GetPointer<R>();
}

template<typename R, typename ... T>
R* get(Variant<T...>* var)
{
    return var->m_Data.template GetPointer<R>();
}

}

#endif // FLEX_LIB_VARIANT_IMPL_H
