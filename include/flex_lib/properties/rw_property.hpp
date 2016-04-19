/*****************************************************************************************
	Created:	04.05.2008	14:32

	Purpose:	
******************************************************************************************
	History:

	20080504 Flex Ferrum:
******************************************************************************************/

#ifndef Sandbox_Source_Files_property_rw_property_hpp__
#define Sandbox_Source_Files_property_rw_property_hpp__

#include "properties_fwd.hpp"

# define MAKE_PROPERTY_TYPE_RW(ClassName, PI) MAKE_PROP_TYPE_GENERIC(ClassName, PI, DefaultPropertyBase, BOOST_PP_SEQ_ELEM(6, PI))

# define DEFINE_PROPERTY_RW(ClassName, PI) DEFINE_PROPERTY_GENERIC_X(ClassName, PI, DefaultPropertyBase, BOOST_PP_SEQ_ELEM(6, PI));

# define DEFINE_PROPERTY_IMPL_AS_MEMBER_RW(ClassName, PI) 

# define RW_PROPERTY_CTR_INIT_RW(ClassName, PI) &ClassName::BOOST_PP_SEQ_ELEM(7, PI), &ClassName::BOOST_PP_SEQ_ELEM(8, PI)
# define RW_PROPERTY_CTR_INIT_RO(ClassName, PI) &ClassName::BOOST_PP_SEQ_ELEM(7, PI)

# define DEFINE_PROPERTY_IMPL_AS_STATIC_RW(ClassName, PI) \
	static boost::properties::detail::BOOST_PP_SEQ_ELEM(3, PI)<PROP_TYPE(PI), ClassName> BOOST_PP_SEQ_ELEM(4, PI)(BOOST_PP_CAT(RW_PROPERTY_CTR_INIT_, BOOST_PP_SEQ_ELEM(5, PI))(ClassName, PI));

# define INITIALIZE_PROPERTY_IMPL_RW(ClassName, PI) \
	host->BOOST_PP_SEQ_ELEM(4, PI).Init(host, &BOOST_PP_SEQ_ELEM(4, PI) );

# define COPY_PROPERTY_IMPL_RW(_1, _2) 

# define SERIALIZE_PROPERTY_IMPL_RW(ClassName, PI) 

# define INVOKE_GET_PROPERTY_IMPL_RW(ClassName, PI) INVOKE_GET_PROPERTY_IMPL_GENERIC(ClassName, PI)

# define INVOKE_SET_PROPERTY_IMPL_RW(ClassName, PI) INVOKE_SET_PROPERTY_IMPL_GENERIC(ClassName, PI)

# define HAS_PROPERTY_PROPERTY_IMPL_RW(ClassName, PI) HAS_PROPERTY_PROPERTY_IMPL_GENERIC(ClassName, PI)


# define ENUM_PROPERTY_IMPL_RW(ClassName, PI) ENUM_PROPERTY_IMPL_GENERIC(ClassName, PI)

#define RW_PROPERTY(Type, Name, Get, Set) ( (RW)((Type))(Property)(SetterGetterPropertyImpl)(Name)(RW)(ReadWritePropertyAccessors)(Get)(Set) )
#define RO_PROPERTY(Type, Name, Get) ( (RW)((Type))(Property)(GetterPropertyImpl)(Name)(RO)(ReadPropertyAccessors)(Get)(_) )
#define WO_PROPERTY(Type, Name, Set) ( (RW)((Type))(Property)(SetterPropertyImpl)(Name)(RO)(WritePropertyAccessors)(Set)(_) )

#define RW_PROPERTY_V(Type, Name, Get, Set) ( (RW)((boost::properties::by_val<Type>))(Property)(SetterGetterPropertyImpl)(Name)(RW)(ReadWritePropertyAccessors)(Get)(Set) )
#define RO_PROPERTY_V(Type, Name, Get) ( (RW)((boost::properties::by_val<Type>))(Property)(GetterPropertyImpl)(Name)(RO)(ReadPropertyAccessors)(Get)(_) )
#define WO_PROPERTY_V(Type, Name, Set) ( (RW)((boost::properties::by_val<Type>))(Property)(SetterPropertyImpl)(Name)(RO)(WritePropertyAccessors)(Set)(_) )

namespace boost
{
	namespace properties
	{
		namespace detail
		{
			template<typename H, typename PTR, bool IsScalar, bool IsRef, bool IsConst>
			struct SetterGetterTraitsImpl
			{
				typedef typename PTR::result_type result_type;
				typedef typename PTR::arg_type arg_type;
				typedef result_type (H::*GetterType)() const;
				typedef void (H::*SetterType)(arg_type val);
				typedef H* const Host;
				BOOST_STATIC_CONSTANT(bool, ConstGetter = true);
			};

			template<typename H, typename PTR>
			struct SetterGetterTraitsImpl<H, PTR, false, true, false>
			{
				typedef typename PTR::clear_type& result_type;
				typedef typename PTR::clear_type& arg_type;
				typedef result_type (H::*GetterType)();
				typedef void (H::*SetterType)(arg_type val);
				typedef H* Host;
				BOOST_STATIC_CONSTANT(bool, ConstGetter = false);
			};

			template<typename H, typename PTR>
			struct SetterGetterTraits : SetterGetterTraitsImpl<H, PTR, PTR::is_scalar, PTR::is_reference, PTR::is_const>
			{
			};

			template<typename T, typename H>
			class SetterGetterPropertyImpl
			{
			public:
				typedef PropertyTraits<T> traits;
				typedef SetterGetterTraits<H, traits> sg_traits;
				typedef typename sg_traits::GetterType GetterType;
				typedef typename sg_traits::SetterType SetterType;
				typedef typename sg_traits::arg_type arg_type;

				SetterGetterPropertyImpl(GetterType getter, SetterType setter) : m_Getter(getter), m_Setter(setter) {;}
				typename sg_traits::result_type get(typename sg_traits::Host host) const
				{
					return (host->*m_Getter)();
				}
				void set(typename sg_traits::Host host, typename sg_traits::arg_type val)
				{
					(host->*m_Setter)(val);
				}
			private:
				GetterType m_Getter;
				SetterType m_Setter;
			};

			template<typename T, typename H>
			class GetterPropertyImpl
			{
			public:
				typedef PropertyTraits<T> traits;
				typedef SetterGetterTraits<H, traits> sg_traits;
				typedef typename sg_traits::GetterType GetterType;
				typedef typename traits::arg_type arg_type;

				GetterPropertyImpl(GetterType getter) : m_Getter(getter) {;}
				typename sg_traits::result_type get(typename sg_traits::Host host) const
				{
					return (host->*m_Getter)();
				}
			private:
				GetterType m_Getter;
			};

			template<typename T, typename H>
			class SetterPropertyImpl
			{
			public:
				typedef PropertyTraits<T> traits;
				typedef SetterGetterTraits<H, traits> sg_traits;
				typedef typename sg_traits::SetterType SetterType;
				typedef typename sg_traits::arg_type arg_type;

				SetterPropertyImpl(SetterType setter) : m_Setter(setter) {;}
				void set(typename sg_traits::Host host, typename sg_traits::arg_type val)
				{
					(host->*m_Setter)(val);
				}
			private:
				SetterType m_Setter;
			};
		}
	}
}

#endif // Sandbox_Source_Files_property_rw_property_hpp__
