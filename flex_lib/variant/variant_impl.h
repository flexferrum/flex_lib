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

template<typename ... Types>
struct VariantDataManipulator;

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
    
    VariantDataHolder(const this_type &other, int dataType)
        : m_Data()
    {
        VariantDataManipulator<Types ...>::CopyData(other.m_Data, dataType, m_Data);
    }
    
    VariantDataHolder(this_type &&other, int dataType)
        : m_Data()
    {
        VariantDataManipulator<Types ...>::MoveData(other.m_Data, dataType, m_Data);
    }
    
    template<typename U>
    VariantDataHolder(const U& data, int &dataType)
        : m_Data()
    {
        constexpr int type = VariantTypesEnumerator<Types ...>::template MatchType<U>::value;
        dataType = type;
        if (dataType == -1)
            return;        
        VariantDataManipulator<Types ...>::template InitializeDataCopy<type>(data, m_Data);
    }
    
    template<typename U>
    VariantDataHolder(U&& data, int &dataType)
        : m_Data()
    {
        constexpr int type = VariantTypesEnumerator<Types ...>::template MatchType<U>::value;
        dataType = type;
        if (dataType == -1)
            return;
        VariantDataManipulator<Types ...>::template InitializeDataMove<type>(data, m_Data);
    }            
    
    void DestroyData(int dataType)
    {
        VariantDataManipulator<Types ...>::DestroyData(m_Data, dataType);
    }
    
    void Swap(int dataType, this_type &other, int otherDataType)
    {
        if (dataType == otherDataType)
        {
            if (dataType == -1)
                return;
            
            VariantDataManipulator<Types ...>::SwapData(other.m_Data, dataType, m_Data);
            return;
        }

        if (dataType == -1)
            VariantDataManipulator<Types ...>::MoveData(other.m_Data, otherDataType, m_Data);
        else if (otherDataType == -1)
            VariantDataManipulator<Types ...>::MoveData(m_Data, dataType, other.m_Data);
        else
        {
            this_type tmp;
            VariantDataManipulator<Types ...>::MoveData(other.m_Data, otherDataType, tmp.m_Data);
            VariantDataManipulator<Types ...>::MoveData(m_Data, dataType, other.m_Data);
            VariantDataManipulator<Types ...>::MoveData(tmp.m_Data, otherDataType, m_Data);
        }
    }
    
    template<typename R>
    R GetValue(int dataType) const
    {
        typedef typename VariantTypesEnumerator<Types ...>::template RealTypeGetter<R>::type result;
        
        return *GetPointer<result>(dataType);
    }
    
    template<typename R>
    auto GetPointer(int dataType) const -> const typename VariantTypesEnumerator<Types ...>::template RealTypeGetter<R>::type *
    {
        typedef typename VariantTypesEnumerator<Types ...>::template RealTypeGetter<R>::type result;
        constexpr int type = VariantTypesEnumerator<Types ...>::template MatchType<result>::value;
        if (type != dataType)
            return nullptr;        
        
        return reinterpret_cast<const result *>(&m_Data);
    }
    
    template<typename R>
    auto GetPointer(int dataType) -> typename VariantTypesEnumerator<Types ...>::template RealTypeGetter<R>::type *
    {
        typedef typename VariantTypesEnumerator<Types ...>::template RealTypeGetter<R>::type result;
        constexpr int type = VariantTypesEnumerator<Types ...>::template MatchType<result>::value;
        if (type != dataType)
            return nullptr;        
        
        return reinterpret_cast<result *>(&m_Data);
    }
    
    void * GetRawDataPointer()
    {
        return &m_Data;
    }
    
private:
    InnerUnion<0, Types...> m_Data;
};

template<typename ... Types>
struct VariantDataManipulator
{
    typedef InnerUnion<0, Types ...> Holder;
    
    static void CopyData(const Holder &from, int dataType, Holder &to)
    {
        if (dataType != -1)
            to.CopyFrom(from, dataType);
    }
    
    static void MoveData(Holder &from, int dataType, Holder &to)
    {
        if (dataType != -1)
            to.MoveFrom(from, dataType);
    }
    
    static void SwapData(Holder &from, int dataType, Holder &to)
    {
        if (dataType != -1)
            to.SwapWith(from, dataType);
    }
    
    template<int I, typename U>
    static void InitializeDataCopy(const U &data, Holder &to)
    {
        to.template InitWithCopy<I>(data);
    }
    
    template<int I, typename U>
    static void InitializeDataMove(U &&data, Holder &to)
    {
        to.template InitWithMove<I>(data);
    }
    
    static void DestroyData(Holder &data, int dataType)
    {
        data.Destroy(dataType);
    }
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
        : m_DataType(Empty)
    {
        ;
    }
    
    Variant(const this_type &other)
        : m_DataType(other.m_DataType)
        , m_Data(other.m_Data, m_DataType)
    {
        ;
    }
    
    Variant(this_type &&other)
        : m_DataType(other.m_DataType)
        , m_Data(std::move(other.m_Data), m_DataType)
    {
        other.m_DataType = Empty;
    }
       
    template<typename T>
    Variant(const T& other)
        : m_DataType(Empty)
        , m_Data(other, m_DataType)
    {
        
    }
    
    ~Variant()
    {
        if (m_DataType != Empty)
            m_Data.DestroyData(m_DataType);
    }
    
    void swap(this_type &other)
    {
        m_Data.Swap(m_DataType, other.m_Data, other.m_DataType);
        std::swap(m_DataType, other.m_DataType);
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
    
    bool IsEmpty() const
    {
        return m_DataType == Empty;
    }
    
    int Which() const 
    {
        return m_DataType;        
    }
    
#if FL_VARIANT_WITH_TEST == 1
    void *GetDataPointer()
    {
        return m_Data.GetRawDataPointer();
    }

#endif
    
private:    
    int m_DataType;
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
    return var.m_Data.template GetValue<R>(var.m_DataType);
}

template<typename R, typename ... T>
const R* get(const Variant<T...>* var)
{
    return var->m_Data.template GetPointer<R>(var->m_DataType);
}

template<typename R, typename ... T>
R* get(Variant<T...>* var)
{
    return var->m_Data.template GetPointer<R>(var->m_DataType);
}

}

#endif // FLEX_LIB_VARIANT_IMPL_H
