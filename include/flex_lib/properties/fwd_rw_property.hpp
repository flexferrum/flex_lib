/*****************************************************************************************
	Created:	04.05.2008	14:32

	Purpose:	
******************************************************************************************
	History:

	20080504 Flex Ferrum:
******************************************************************************************/

#ifndef Sandbox_Source_Files_property_fwd_rw_property_hpp__
#define Sandbox_Source_Files_property_fwd_rw_property_hpp__

#include <functional>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/function.hpp>
#include <boost/utility/result_of.hpp>
#include "properties_fwd.hpp"

# define MAKE_PROPERTY_TYPE_FWDRW(ClassName, PI) MAKE_PROP_TYPE_GENERIC(ClassName, PI, DefaultPropertyBase, BOOST_PP_SEQ_ELEM(6, PI))

# define DEFINE_PROPERTY_FWDRW(ClassName, PI) DEFINE_PROPERTY_GENERIC_X(ClassName, PI, DefaultPropertyBase, BOOST_PP_SEQ_ELEM(6, PI));

# define DEFINE_PROPERTY_IMPL_AS_MEMBER_FWDRW(ClassName, PI) 

# define FWDRW_PROPERTY_CTR_INIT_RW(ClassName, PI) BOOST_PP_SEQ_ELEM(7, PI), BOOST_PP_SEQ_ELEM(8, PI)
# define FWDRW_PROPERTY_CTR_INIT_RO(ClassName, PI) BOOST_PP_SEQ_ELEM(7, PI)

# define DEFINE_PROPERTY_IMPL_AS_STATIC_FWDRW(ClassName, PI) \
	static boost::properties::detail::BOOST_PP_SEQ_ELEM(3, PI)<PROP_TYPE(PI), ClassName> BOOST_PP_SEQ_ELEM(4, PI)(BOOST_PP_CAT(FWDRW_PROPERTY_CTR_INIT_, BOOST_PP_SEQ_ELEM(5, PI))(ClassName, PI));

# define INITIALIZE_PROPERTY_IMPL_FWDRW(ClassName, PI) \
	host->BOOST_PP_SEQ_ELEM(4, PI).Init(host, &BOOST_PP_SEQ_ELEM(4, PI) );

# define COPY_PROPERTY_IMPL_FWDRW(_1, _2) 


# define COPY_PROPERTY_IMPL_FWDRW(_1, _2) 

# define SERIALIZE_PROPERTY_IMPL_FWDRW(ClassName, PI) 

# define INVOKE_GET_PROPERTY_IMPL_FWDRW(ClassName, PI) INVOKE_GET_PROPERTY_IMPL_GENERIC(ClassName, PI)

# define INVOKE_SET_PROPERTY_IMPL_FWDRW(ClassName, PI) INVOKE_SET_PROPERTY_IMPL_GENERIC(ClassName, PI)

# define ENUM_PROPERTY_IMPL_FWDRW(ClassName, PI) ENUM_PROPERTY_IMPL_GENERIC(ClassName, PI)

#define FWD_RW_PROPERTY(Type, Name, Get, Set) ( (FWDRW)((Type))(Property)(FwdRWPropertyImpl)(Name)(RW)(ReadWritePropertyAccessors)(Get)(Set) )
#define MEM_FWD_RW_PROPERTY(Type, Name, MemName, Get, Set) ( (FWDRW)((Type))(Property)(FwdRWPropertyImpl)(Name)(RW)(ReadWritePropertyAccessors)\
				( (boost::bind(&Get, boost::bind(&T::MemName, _1))) )\
				( (boost::bind(&Set, boost::bind(boost::mem_ptr(&T::MemName), _1), _2)) ) )
#define FWD_RO_PROPERTY(Type, Name, Get) ( (FWDRW)((Type))(Property)(FwdROPropertyImpl)(Name)(RO)(ReadPropertyAccessors)(Get) )
#define MEM_FWD_RO_PROPERTY(Type, Name, MemName, Get) ( (FWDRW)((Type))(Property)(FwdROPropertyImpl)(Name)(RO)(ReadPropertyAccessors)\
				( (boost::bind(&Get, boost::bind(&T::MemName, _1))) ) )
#define FWD_WO_PROPERTY(Type, Name, Set) ( (FWDRW)((Type))(Property)(FwdWOPropertyImpl)(Name)(RO)(WritePropertyAccessors)(Set) )
#define MEM_FWD_WO_PROPERTY(Type, Name, MemName, Set) ( (FWDRW)((Type))(Property)(FwdWOPropertyImpl)(Name)(RO)(WritePropertyAccessors)\
				( (boost::bind(&Set, boost::bind(boost::mem_ptr(&T::MemName), _1), _2)) ) )

namespace boost
{
	namespace properties
	{
		namespace detail
		{
			template<typename T, typename H>
			class FwdRWPropertyImpl
			{
			public:
				typedef PropertyTraits<T> traits;
				typedef typename traits::arg_type arg_type;
				typedef boost::function<typename traits::result_type (const H*)> GetterType;
				typedef boost::function<void (H*, typename traits::arg_type)> SetterType;

				FwdRWPropertyImpl(GetterType getter, SetterType setter) : m_Getter(getter), m_Setter(setter) {;}
				typename traits::result_type get(H const* host) const
				{
					return m_Getter(host);
				}
				void set(H* host, typename traits::arg_type val)
				{
					m_Setter(host, val);
				}
			private:
				GetterType m_Getter;
				SetterType m_Setter;
			};

			template<typename T, typename H>
			class FwdROPropertyImpl
			{
			public:
				typedef PropertyTraits<T> traits;
				typedef typename traits::arg_type arg_type;
				typedef boost::function<typename traits::result_type (const H*)> GetterType;

				FwdROPropertyImpl(GetterType getter) : m_Getter(getter) {;}
				typename traits::result_type get(H const* host) const
				{
					return m_Getter(host);
				}
			private:
				GetterType m_Getter;
			};

			template<typename T, typename H>
			class FwdWOPropertyImpl
			{
			public:
				typedef PropertyTraits<T> traits;
				typedef typename traits::arg_type arg_type;
				typedef boost::function<void (H*, typename traits::arg_type)> SetterType;

				FwdWOPropertyImpl(SetterType setter) : m_Setter(setter) {;}
				void set(H* host, typename traits::arg_type val)
				{
					m_Setter(host, val);
				}
			private:
				SetterType m_Setter;
			};
		}
	}

	template<typename R, typename T>
	class mem_ptr_impl : public std::unary_function<T*, R*>
	{
	public:
		mem_ptr_impl(R T::*ptr) : m_Ptr(ptr) {;}
		R* operator()(T* obj)
		{
			return &(obj->*m_Ptr);
		}
	private:
		R T::*m_Ptr;
	};

	template<typename R, typename T, bool IsPtr>
	struct mem_ptr_chooser
	{	
		typedef void type;
	};

	template<typename R, typename T>
	struct mem_ptr_chooser<R, T, true>
	{
		typedef _mfi::dm<R, T> type;
	};

	template<typename R, typename T>
	struct mem_ptr_chooser<R, T, false>
	{
		typedef mem_ptr_impl<R, T> type;
	};

	template<typename R, typename T>
	typename mem_ptr_chooser<R, T, is_pointer<R>::value >::type mem_ptr(R T::*ptr)
	{
		return typename mem_ptr_chooser<R, T, is_pointer<R>::value >::type(ptr);
	}
}

#endif // Sandbox_Source_Files_property_fwd_rw_property_hpp__
