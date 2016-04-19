/*****************************************************************************************
	Created:	04.05.2008	14:32

	Purpose:	
******************************************************************************************
	History:

	20080504 Flex Ferrum:
******************************************************************************************/

#ifndef Sandbox_Source_Files_property_abstract_property_hpp__
#define Sandbox_Source_Files_property_abstract_property_hpp__

#include "properties_fwd.hpp"

# define MAKE_ABST_PROP_IMPL_TYPE_SIMPLE(ClassName, BaseClassName, PI) boost::properties::detail::AbstractPropImpl<BaseClassName, BOOST_PP_CAT(MAKE_PROPERTY_TYPE_, PROP_KIND(PI))(ClassName, PI) >
# define MAKE_ABST_PROP_IMPL_TYPE_RW(ClassName, BaseClassName, PI) MAKE_ABST_PROP_IMPL_TYPE_SIMPLE(ClassName, BaseClassName, PI)
# define MAKE_ABST_PROP_IMPL_TYPE_FWD_RW(ClassName, BaseClassName, PI) MAKE_ABST_PROP_IMPL_TYPE_SIMPLE(ClassName, BaseClassName, PI)

# define DEFINE_PROPERTY_ABST(ClassName, PI) DEFINE_PROPERTY_GENERIC_X(ClassName, PI, DefaultPropertyBase, BOOST_PP_SEQ_ELEM(5, PI))

# define DEFINE_PROPERTY_ABST_IMPL(ClassName, PI) \
	BOOST_PP_CAT(DEFINE_PROPERTY_, PROP_KIND(BOOST_PP_SEQ_ELEM(6, PI))) (ClassName, BOOST_PP_SEQ_ELEM(6, PI)) \
	BOOST_PP_CAT(MAKE_ABST_PROP_IMPL_TYPE_, PROP_KIND(BOOST_PP_SEQ_ELEM(6, PI))) (ClassName, BOOST_PP_SEQ_ELEM(5, PI), BOOST_PP_SEQ_ELEM(6, PI)) BOOST_PP_CAT(PROP_NAME(BOOST_PP_SEQ_ELEM(6, PI)),_aimpl_);

# define DEFINE_PROPERTY_IMPL_AS_MEMBER_ABST(ClassName, PI) 
# define DEFINE_PROPERTY_IMPL_AS_MEMBER_ABST_IMPL(ClassName, PI) \
		BOOST_PP_CAT(DEFINE_PROPERTY_IMPL_AS_MEMBER_, PROP_KIND(BOOST_PP_SEQ_ELEM(6, PI)))(ClassName, BOOST_PP_SEQ_ELEM(6, PI)) \
		DEFINE_PROPERTY_IMPL_AS_MEMBER_ABST(ClassName, PI)
																							  
# define DEFINE_PROPERTY_IMPL_AS_STATIC_ABST(ClassName, PI) 
# define DEFINE_PROPERTY_IMPL_AS_STATIC_ABST_IMPL(ClassName, PI) \
		BOOST_PP_CAT(DEFINE_PROPERTY_IMPL_AS_STATIC_, PROP_KIND(BOOST_PP_SEQ_ELEM(6, PI)))(ClassName, BOOST_PP_SEQ_ELEM(6, PI)) \
		DEFINE_PROPERTY_IMPL_AS_STATIC_ABST(ClassName, PI) 

# define INITIALIZE_PROPERTY_IMPL_ABST(ClassName, PI) 
# define INITIALIZE_PROPERTY_IMPL_ABST_IMPL(ClassName, PI) \
		BOOST_PP_CAT(INITIALIZE_PROPERTY_IMPL_, PROP_KIND(BOOST_PP_SEQ_ELEM(6, PI)))(ClassName, BOOST_PP_SEQ_ELEM(6, PI)) \
		host->BOOST_PP_SEQ_ELEM(5, PI)::PROP_NAME(BOOST_PP_SEQ_ELEM(6, PI)).Init(host, &host->BOOST_PP_CAT(PROP_NAME(BOOST_PP_SEQ_ELEM(6, PI)),_aimpl_)); \
		host->BOOST_PP_CAT(PROP_NAME(BOOST_PP_SEQ_ELEM(6, PI)),_aimpl_).Init(&host->PROP_NAME(BOOST_PP_SEQ_ELEM(6, PI)));

# define COPY_PROPERTY_IMPL_ABST(_1, _2) 
# define COPY_PROPERTY_IMPL_ABST_IMPL(ClassName, PI) \
	BOOST_PP_CAT(COPY_PROPERTY_IMPL_, PROP_KIND(BOOST_PP_SEQ_ELEM(6, PI)))(ClassName, BOOST_PP_SEQ_ELEM(6, PI)) \

# define SERIALIZE_PROPERTY_IMPL_ABST(ClassName, PI) 
# define SERIALIZE_PROPERTY_IMPL_ABST_IMPL(ClassName, PI) \
	BOOST_PP_CAT(SERIALIZE_PROPERTY_IMPL_, PROP_KIND(BOOST_PP_SEQ_ELEM(6, PI)))(ClassName, BOOST_PP_SEQ_ELEM(6, PI)) \

# define INVOKE_GET_PROPERTY_IMPL_ABST(ClassName, PI) INVOKE_GET_PROPERTY_IMPL_GENERIC(ClassName, PI)
# define INVOKE_GET_PROPERTY_IMPL_ABST_IMPL(ClassName, PI) \
	BOOST_PP_CAT(INVOKE_GET_PROPERTY_IMPL_, PROP_KIND(BOOST_PP_SEQ_ELEM(6, PI)))(ClassName, BOOST_PP_SEQ_ELEM(6, PI)) \

# define INVOKE_SET_PROPERTY_IMPL_ABST(ClassName, PI) INVOKE_SET_PROPERTY_IMPL_GENERIC(ClassName, PI)
# define INVOKE_SET_PROPERTY_IMPL_ABST_IMPL(ClassName, PI) \
	BOOST_PP_CAT(INVOKE_SET_PROPERTY_IMPL_, PROP_KIND(BOOST_PP_SEQ_ELEM(6, PI)))(ClassName, BOOST_PP_SEQ_ELEM(6, PI)) \

# define ENUM_PROPERTY_IMPL_ABST(ClassName, PI) ENUM_PROPERTY_IMPL_GENERIC(ClassName, PI)
# define ENUM_PROPERTY_IMPL_ABST_IMPL(ClassName, PI) \
	BOOST_PP_CAT(ENUM_PROPERTY_IMPL_, PROP_KIND(BOOST_PP_SEQ_ELEM(6, PI)))(ClassName, BOOST_PP_SEQ_ELEM(6, PI)) \

#define ABSTRACT_RW_PROPERTY(Type, Name) ( (ABST)((Type))(Property)(AbstractPropertyImpl)(Name)(ReadWritePropertyAccessors) )
#define ABSTRACT_RW_PROPERTY_V(Type, Name) ( (ABST)((boost::properties::by_val<Type>))(Property)(AbstractPropertyImpl)(Name)(ReadWritePropertyAccessors) )
#define ABSTRACT_RO_PROPERTY(Type, Name) ( (ABST)((Type))(Property)(AbstractPropertyImpl)(Name)(ReadPropertyAccessors) )
#define ABSTRACT_RO_PROPERTY_V(Type, Name) ( (ABST)((boost::properties::by_val<Type>))(Property)(AbstractPropertyImpl)(Name)(ReadPropertyAccessors) )
#define ABSTRACT_WO_PROPERTY(Type, Name) ( (ABST)((Type))(Property)(AbstractPropertyImpl)(Name)(WritePropertyAccessors) )
#define ABSTRACT_WO_PROPERTY_V(Type, Name) ( (ABST)((boost::properties::by_val<Type>))(Property)(AbstractPropertyImpl)(Name)(WritePropertyAccessors) )

#define IMPLEMENT_PROPERTY(BaseClass, ImplProp) ( (ABST_IMPL)((_))(PropertyImpl)(_)(_)(BaseClass)ImplProp )
#define IMPLEMENT_PROPERTY_INH(BaseClass, BaseType, ImplProp) ( (ABST_IMPL_INH)((_))(PropertyImpl)(_)(_)(BaseClass)(BaseType)ImplProp )

namespace boost
{
	namespace properties
	{
		namespace detail
		{
			template<typename T, typename H>
			class AbstractPropertyImpl
			{
			public:
				typedef PropertyTraits<T> traits;
				typedef typename traits::arg_type arg_type;

				virtual typename traits::result_type get(H const* host) const = 0;
				virtual typename traits::result_type get(H* host) = 0;
				virtual void set(H* host, typename traits::arg_type val) = 0;
			};

			template<typename BT, typename P>
			class AbstractPropImpl : public AbstractPropertyImpl<typename P::value_type, BT>
			{
			public:
				typedef typename AbstractPropertyImpl<typename P::value_type, BT> base_type;
				typedef typename base_type::traits::arg_type arg_type;

				typename base_type::traits::result_type get(BT const* host) const
				{
					return m_Prop->safe_get();
				}

				typename base_type::traits::result_type get(BT* host)
				{
					return m_Prop->safe_get();
				}

				void set(BT* host, typename base_type::traits::arg_type val)
				{
					m_Prop->safe_set(val);
				}

				void Init(P* prop)
				{
					m_Prop = prop;
				}
			private:
				P* m_Prop;
			};

		}
	}
}

#endif // Sandbox_Source_Files_property_abstract_property_hpp__
