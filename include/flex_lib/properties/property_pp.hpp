/*****************************************************************************************
	Created:	04.05.2008	14:32

	Purpose:	
******************************************************************************************
	History:

	20080504 Flex Ferrum:
******************************************************************************************/

#ifndef Sandbox_Source_Files_property_property_pp_hpp__
#define Sandbox_Source_Files_property_property_pp_hpp__

#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
// #include <boost/bind.hpp>
#include "dynamic_access.hpp"

#/* 
# * Returns kind of property
# */
# define PROP_KIND(PI) BOOST_PP_SEQ_HEAD(PI)
#
#/* 
# * Returns type of property
# */
# define PROP_TYPE_(TI) BOOST_PP_EXPAND(BOOST_PP_SEQ_ENUM(TI))
# define PROP_TYPE(PI) BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_ELEM(1, PI))
# define PROP_NAME(PI) BOOST_PP_SEQ_ELEM(4, PI)
#
#
# /* Host class properties definition */
#
#
# define MAKE_PROP_TYPE_GENERIC(ClassName, PI, ImplType, AccessorType)	\
	boost::properties::BOOST_PP_SEQ_ELEM(2, PI)<PROP_TYPE(PI), ClassName, boost::properties::ImplType<PROP_TYPE(PI), ClassName, boost::properties::detail::BOOST_PP_SEQ_ELEM(3, PI)>, boost::properties::detail::AccessorType >

# define DEFINE_PROPERTY_GENERIC(ClassName, PI) MAKE_PROP_TYPE_GENERIC(ClassName, PI, DefaultPropertyBase, ReadWritePropertyAccessors) PROP_NAME(PI);
# define DEFINE_PROPERTY_GENERIC_X(ClassName, PI, ImplType, AccessorType) MAKE_PROP_TYPE_GENERIC(ClassName, PI, ImplType, AccessorType) PROP_NAME(PI);
#
#/*
# * Class member property declaration macro. Declare property in the property-owner class 
# * scope.
# * ClassName: Property-owner class name 
# * PI: Property info set
# */

# define DEFINE_PROPERTY(_, ClassName, PI) \
	BOOST_PP_CAT(DEFINE_PROPERTY_, PROP_KIND(PI))(ClassName, PI)
#
#
#
#  /* Property-holder class properties definition as datamembers*/
#
#
#
#/*
# * Dispatches trivial properties initialization
# * ClassName: Property-owner class name
# * PI: Property info set
# */
# define DEFINE_PROPERTY_IMPL_AS_MEMBER(_, ClassName, PI) \
	BOOST_PP_CAT(DEFINE_PROPERTY_IMPL_AS_MEMBER_, PROP_KIND(PI))(ClassName, PI)
#
#/*
# * Generates trivial properties initialization list
# * ClassName: Property-owner class name
# * S: Properties info set
# */
# define PROPERTIES__DEFINE_PROPS_IMPL_AS_MEMBER(ClassName, S) \
	BOOST_PP_SEQ_FOR_EACH(DEFINE_PROPERTY_IMPL_AS_MEMBER, ClassName, S)
#
#
#
#  /* Property-holder class properties definition as static members*/
#
#
#
#/*
# * Dispatchs non-trivial property implementation generation
# * ClassName: Property-owner class name
# * PI: Property info set
# */
# define DEFINE_PROPERTY_IMPL_AS_STATIC(_, ClassName, PI) \
	BOOST_PP_CAT(DEFINE_PROPERTY_IMPL_AS_STATIC_, PROP_KIND(PI))(ClassName, PI)
#
#/*
# * Generates non-trivial properties implementation list
# * ClassName: Property-owner class name
# * S: Properties info set
# */
# define PROPERTIES__DEFINE_PROPS_AS_STATIC(ClassName, S) \
	BOOST_PP_SEQ_FOR_EACH(DEFINE_PROPERTY_IMPL_AS_STATIC, ClassName, S)
#
#
#
#  /* Property-holder class properties initialization*/
#
#
#
#/*
# * Dispatches property initialization code generation
# * ClassName: Property-owner class name
# * PI: Property info set
# */
# define INITIALIZE_PROPERTY(_, ClassName, PI)\
	BOOST_PP_CAT(INITIALIZE_PROPERTY_IMPL_, PROP_KIND(PI))(ClassName, PI)
#
#/*
# * Generates properties initialization code
# * ClassName: Property-owner class name
# * S: Properties info set
# */
# define PROPERTIES__DO_INITIALIZE(ClassName, S) \
	BOOST_PP_SEQ_FOR_EACH(INITIALIZE_PROPERTY, ClassName, S)
#
#
#
#  /* Property-holder class property copying */
#
#
#
# define COPY_PROPERTY(_, ClassName, PI)\
	BOOST_PP_CAT(COPY_PROPERTY_IMPL_, PROP_KIND(PI))(ClassName, PI)
# define PROPERTIES__DO_COPY(ClassName, S) \
	BOOST_PP_SEQ_FOR_EACH(COPY_PROPERTY, ClassName, S)
#
#
#
#  /* Property-holder class property serialization */
#
#
#
# define SERIALIZE_PROPERTY(_, ClassName, PI)\
	BOOST_PP_CAT(SERIALIZE_PROPERTY_IMPL_, PROP_KIND(PI))(ClassName, PI)
# define PROPERTIES__DO_SERIALIZE(ClassName, S) \
	BOOST_PP_SEQ_FOR_EACH(SERIALIZE_PROPERTY, ClassName, S)
#
#
#
#  /* Property-holder class property-get invoker  */
#
#
#
# define INVOKE_GET_PROPERTY_IMPL_GENERIC(ClassName, PI) \
	else if (name == BOOST_PP_STRINGIZE(PROP_NAME(PI))) {return boost::properties::detail::PropertyGetter<R>::Get(BOOST_PP_STRINGIZE(PROP_NAME(PI)), m_Host->PROP_NAME(PI));}
# define INVOKE_GET_PROPERTY(_, ClassName, PI)\
	BOOST_PP_CAT(INVOKE_GET_PROPERTY_IMPL_, PROP_KIND(PI))(ClassName, PI)
# define PROPERTIES__DO_INVOKE_GET(ClassName, S) \
	BOOST_PP_SEQ_FOR_EACH(INVOKE_GET_PROPERTY, ClassName, S)
# define MAKE_BASE_INVOKE_GET_CALL(ClassName, BaseClassName) \
	return boost::properties::get_property<R>(*static_cast<BaseClassName const*>(m_Host), name);
# define MAKE_PROPERTY_NOT_DEF_THROW(ClassName) \
	throw boost::properties::PropertyNotDefined(name.c_str(), BOOST_PP_STRINGIZE(ClassName));
#
#
#
#  /* Property-holder class property-set invoker */
#
#
#
# define INVOKE_SET_PROPERTY_IMPL_GENERIC(ClassName, PI) \
	else if (name == BOOST_PP_STRINGIZE(PROP_NAME(PI))) {return boost::properties::detail::PropertySetter<V>::Set(BOOST_PP_STRINGIZE(PROP_NAME(PI)), m_Host->PROP_NAME(PI), val);}
# define INVOKE_SET_PROPERTY(_, ClassName, PI)\
	BOOST_PP_CAT(INVOKE_SET_PROPERTY_IMPL_, PROP_KIND(PI))(ClassName, PI)
# define PROPERTIES__DO_INVOKE_SET(ClassName, S) \
	BOOST_PP_SEQ_FOR_EACH(INVOKE_SET_PROPERTY, ClassName, S)
# define MAKE_BASE_INVOKE_SET_CALL(ClassName, BaseClassName) \
	return boost::properties::set_property(*static_cast<BaseClassName*>(m_Host), name, val);
#
#
#
#  /* Property-holder class property presence checker */
#
#
#
# define HAS_PROPERTY_PROPERTY_IMPL_GENERIC(ClassName, PI) \
	else if (name == BOOST_PP_STRINGIZE(PROP_NAME(PI))) {return true;}
# define HAS_PROPERTY_PROPERTY(_, ClassName, PI)\
	BOOST_PP_CAT(HAS_PROPERTY_PROPERTY_IMPL_, PROP_KIND(PI))(ClassName, PI)
# define PROPERTIES__DO_HAS_PROPERTY(ClassName, S) \
	BOOST_PP_SEQ_FOR_EACH(HAS_PROPERTY_PROPERTY, ClassName, S)
# define MAKE_BASE_HAS_PROPERTY_CALL(ClassName, BaseClassName) \
	return static_cast<BaseClassName const*>(m_Host)->properties__.has_property(name);
#
#
#
#  /* Property-holder class properties enumerator */
#
#
#
# define ENUM_PROPERTY_IMPL_GENERIC(ClassName, PI) \
	v(BOOST_PP_STRINGIZE(PROP_NAME(PI)), &T::PROP_NAME(PI));
# define ENUM_PROPERTY(_, ClassName, PI)\
	BOOST_PP_CAT(ENUM_PROPERTY_IMPL_, PROP_KIND(PI))(ClassName, PI)
# define PROPERTIES__DO_ENUM(ClassName, S) \
	BOOST_PP_SEQ_FOR_EACH(ENUM_PROPERTY, ClassName, S)
# /*
#  * Generates properties initialization and implementation code
#  * ClassName: Name of class contained properties
#  * S: Properties sequence
#  */
# define MAKE_PROPERTY_INITER(ClassName, HasBase, BaseClassName, S) \
	template<typename T>										\
	struct Properties__											\
	{															\
		/* Simples properties definition */						\
		PROPERTIES__DEFINE_PROPS_IMPL_AS_MEMBER(ClassName, S)	\
																\
		Properties__(T* host) : m_Host(host) {init(host);}		\
		void init(T* host)										\
		{	/* Non-trivial properties (such as					\
			 * read-writer properties definition */ 			\
			 if (host == NULL) return;							\
			PROPERTIES__DEFINE_PROPS_AS_STATIC(ClassName, S)	\
			/* Properties initialization code */				\
			PROPERTIES__DO_INITIALIZE(ClassName, S)				\
		}														\
		private:												\
		Properties__(const Properties__&) : m_Host(NULL)		\
		{														\
			BOOST_ASSERT(false);								\
		}														\
		public:													\
		Properties__& operator ==(const Properties__& other)	\
		{														\
			if (this == &other) return *this;					\
																\
			copy_from(other);									\
			return *this;										\
		}														\
		/* Copies property values from another object */		\
		void copy_from(const Properties__<T>& other)			\
		{														\
			PROPERTIES__DO_COPY(ClassName, S);					\
		}														\
		/* Serialize property into/from archive */				\
		template<typename Ar>									\
		void serialize(Ar & ar, const unsigned int version = 0)	\
		{														\
			PROPERTIES__DO_SERIALIZE(ClassName, S);				\
		}														\
		/* Invokes property-get by name */						\
		template<typename R>									\
		R invoke_get(const std::string& name) const	\
		{														\
			if (false) {;}										\
			PROPERTIES__DO_INVOKE_GET(ClassName, S);			\
			BOOST_PP_IIF(HasBase, MAKE_BASE_INVOKE_GET_CALL(ClassName, BaseClassName), MAKE_PROPERTY_NOT_DEF_THROW(ClassName))\
		}														\
		/* Invokes property-set by name */						\
		template<typename V>									\
		void invoke_set(const std::string& name, V val)			\
		{														\
			if (false) {;}										\
			PROPERTIES__DO_INVOKE_SET(ClassName, S);			\
			BOOST_PP_IIF(HasBase, MAKE_BASE_INVOKE_SET_CALL(ClassName, BaseClassName), MAKE_PROPERTY_NOT_DEF_THROW(ClassName))\
		}														\
		/* Test property presence */							\
		bool has_property(const std::string& name)				\
		{														\
			if (false) {;}										\
			PROPERTIES__DO_HAS_PROPERTY(ClassName, S);			\
			BOOST_PP_IIF(HasBase, MAKE_BASE_HAS_PROPERTY_CALL(ClassName, BaseClassName), return false;)\
		}														\
		/* Enumerates properties */								\
		template<typename V>									\
		static void do_enum_s(const V& v)						\
		{														\
			PROPERTIES__DO_ENUM(ClassName, S);					\
		}														\
		template<typename V>									\
		void do_enum(const V& v)								\
		{														\
			Properties__::do_enum_s(boost::bind(v, m_Host, _1, _2));	\
		}														\
		template<typename V>									\
		void do_enum(const V& v) const							\
		{														\
			Properties__::do_enum_s(boost::bind(v, m_Host, _1, _2));\
		}														\
		T* m_Host;												\
	};															\
	/* Declare properties holder class as friend for 
	 * properties owner class                        */			\
	friend struct Properties__<ClassName>;						\
	/* Declare properties holder as data member. It helps 
	 * us to make single call (in constructor) 
	 * for all properties initialization */						\
	Properties__<ClassName> properties__;						\

# /*
#  * Root macro for properties initialization code.
#  * ClassName: Name of class contained properties
#  * S: Properties sequence
#  */
#define PROPERTIES(ClassName, S) \
	BOOST_PP_SEQ_FOR_EACH(DEFINE_PROPERTY, ClassName, S);\
	MAKE_PROPERTY_INITER(ClassName, 0, BOOST_PP_EMPTY, S) 

#define PROPERTIES_INH(ClassName, BaseClassName, S) \
	BOOST_PP_SEQ_FOR_EACH(DEFINE_PROPERTY, ClassName, S);\
	MAKE_PROPERTY_INITER(ClassName, 1, BaseClassName, S) 

#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif


#endif // Sandbox_Source_Files_property_property_pp_hpp__
