/*****************************************************************************************
	Created:	04.05.2008	14:3

	Purpose:	
******************************************************************************************
	History:

	20080504 Flex Ferrum:
******************************************************************************************/

#ifndef Sandbox_Source_Files_property_simple_property_hpp__
#define Sandbox_Source_Files_property_simple_property_hpp__

#include "properties_fwd.hpp"

# define MAKE_PROPERTY_TYPE_SIMPLE(ClassName, PI) \
	boost::properties::BOOST_PP_SEQ_ELEM(2, PI)<PROP_TYPE(PI), ClassName, boost::properties::LightWieghtProperty<PROP_TYPE(PI), ClassName> >
# define DEFINE_PROPERTY_SIMPLE(ClassName, PI) \
	MAKE_PROPERTY_TYPE_SIMPLE(ClassName, PI) BOOST_PP_SEQ_ELEM(4, PI);
	

# define DEFINE_PROPERTY_IMPL_AS_MEMBER_SIMPLE(ClassName, PI) 
# define DEFINE_PROPERTY_IMPL_AS_STATIC_SIMPLE(_1, _2)

#define INIT_SIMPLE_PROPERTY_NOINIT(ClassName, PI)
#define INIT_SIMPLE_PROPERTY_INIT(ClassName, PI) m_Host->BOOST_PP_SEQ_ELEM(4, PI).init_value BOOST_PP_SEQ_ELEM(6, PI);

# define INITIALIZE_PROPERTY_IMPL_SIMPLE(ClassName, PI) BOOST_PP_CAT(INIT_SIMPLE_PROPERTY_, BOOST_PP_SEQ_ELEM(5, PI))(ClassName, PI)
# define COPY_PROPERTY_IMPL_SIMPLE(ClassName, PI) m_Host->BOOST_PP_SEQ_ELEM(4, PI).copy_from(other.m_Host->BOOST_PP_SEQ_ELEM(4, PI) );
# define SERIALIZE_PROPERTY_IMPL_SIMPLE(ClassName, PI) ar & m_Host->BOOST_PP_SEQ_ELEM(4, PI);
# define INVOKE_GET_PROPERTY_IMPL_SIMPLE(ClassName, PI) INVOKE_GET_PROPERTY_IMPL_GENERIC(ClassName, PI)
# define INVOKE_SET_PROPERTY_IMPL_SIMPLE(ClassName, PI) INVOKE_SET_PROPERTY_IMPL_GENERIC(ClassName, PI)
# define HAS_PROPERTY_PROPERTY_IMPL_SIMPLE(ClassName, PI) HAS_PROPERTY_PROPERTY_IMPL_GENERIC(ClassName, PI)
# define ENUM_PROPERTY_IMPL_SIMPLE(ClassName, PI) ENUM_PROPERTY_IMPL_GENERIC(ClassName, PI)

# define PROPERTY(Type, Name) ( (SIMPLE)((Type))(Property)(SimplePropertyImpl)(Name)(NOINIT) )
# define PROPERTY_I(Type, Name, InitVal) ( (SIMPLE)((Type))(Property)(SimplePropertyImpl)(Name)(INIT)((InitVal)) )
# define PROPERTY_CT(N, Type, Name) ( (SIMPLE)(BOOST_PP_TUPLE_TO_SEQ(N, Type))(Property)(SimplePropertyImpl)(Name)(NOINIT) )
# define PROPERTY_CT_I(N, Type, Name, InitVal) ( (SIMPLE)(BOOST_PP_TUPLE_TO_SEQ(N, Type))(Property)(SimplePropertyImpl)(Name)(INIT)(InitVal) )

namespace boost
{
	namespace properties
	{
		template<typename T, typename H>
		class LightWieghtProperty
		{
		public:
			typedef LightWieghtProperty<T, H> this_type;
			typedef detail::PropertyTraits<T> traits;
			typedef typename traits::arg_type arg_type;

			typename traits::const_result_type get() const
			{
				return m_Value;
			}
			typename traits::nconst_result_type get()
			{
				return m_Value;
			}
			void set(typename traits::arg_type value)
			{
				m_Value = value;
			}
			void Init(H* host, void* impl)
			{
			}
			void copy_from(const this_type& other)
			{
				m_Value = other.m_Value;
			}
			void init_value(typename traits::arg_type value)
			{
				m_Value = value;
			}
		private:
			typename traits::clear_type m_Value;
		};
	}
}

#endif // Sandbox_Source_Files_property_simple_property_hpp__
