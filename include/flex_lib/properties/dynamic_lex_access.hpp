/*****************************************************************************************
	Created:	15.01.2009	12:11

	Purpose:	
******************************************************************************************
	History:

	20090115 Flex Ferrum:
******************************************************************************************/

#ifndef properties_Boost_headers_dynamic_lex_access_hpp__
#define properties_Boost_headers_dynamic_lex_access_hpp__

#include <string>
#include "dynamic_access.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/type_traits/is_fundamental.hpp>

namespace boost
{
	namespace properties
	{
		namespace detail
		{
			template<typename T/*, typename CharT*/>
			struct IsInputStreamable
			{
				BOOST_STATIC_CONSTANT(int, value = boost::is_arithmetic<T>::value);
			};

			template<typename T/*, typename CharT*/>
			struct IsOutputStreamable
			{
				BOOST_STATIC_CONSTANT(int, value = boost::is_arithmetic<T>::value);
			};

			template<typename R>
			struct PropertyLexGetter
			{
				template<typename P>
				static typename boost::enable_if_c<
					boost::is_convertible<typename P::result_type, R>::value &&
					P::has_getter, R>::type
					Get(const char* name, P const& prop)
				{
					return prop.safe_get();
				}
				template<typename P>
				static typename boost::enable_if_c<
					!boost::is_convertible<typename P::result_type, R>::value &&
					IsOutputStreamable<typename P::value_type>::value && 
					P::has_getter, R>::type
					Get(const char* name, P const& prop)
				{
					return  boost::lexical_cast<R>(prop.safe_get());
				}
				template<typename P>
				static typename boost::enable_if_c<
					(!boost::is_convertible<typename P::result_type, R>::value &&
					!IsOutputStreamable<typename P::value_type>::value) || 
					!P::has_getter, R>::type
					Get(const char* name, P const& prop)
				{
					throw PropertyInaccessable(name);
				}
			};

			template<>
			struct PropertyGetter<std::string> : PropertyLexGetter<std::string>
			{
				;
			};
 			template<>
 			struct PropertyGetter<std::wstring> : PropertyLexGetter<std::wstring>
 			{
 				;
 			};

			template<typename V>
			struct PropertyLexSetter
			{
				template<typename P>
				static typename boost::enable_if_c<
					boost::is_convertible<V, typename P::arg_type>::value &&
					P::has_setter, void>::type
					Set(const char* name, P& prop, V val)
				{
					prop.safe_set(val);
				}
				template<typename P>
				static typename boost::enable_if_c<
					!boost::is_convertible<V, typename P::arg_type>::value &&
					IsInputStreamable<typename P::value_type>::value && 
					P::has_setter, void>::type
					Set(const char* name, P& prop, V val)
				{
					prop.safe_set(boost::lexical_cast<typename P::value_type>(val));
				}
				template<typename P>
				static typename boost::enable_if_c<
					(!boost::is_convertible<V, typename P::arg_type>::value &&
					!IsInputStreamable<typename P::value_type>::value) || 
					!P::has_setter, void>::type
					Set(const char* name, P& prop, V val)
				{
					throw PropertyInaccessable(name);
				}
			};

			template<>
			struct PropertySetter<std::string> : PropertyLexSetter<std::string>
			{
				;
			};
			template<>
			struct PropertySetter<std::string&> : PropertyLexSetter<std::string&>
			{
				;
			};
			template<>
			struct PropertySetter<std::string const&> : PropertyLexSetter<std::string const&>
			{
				;
			};
			template<>
			struct PropertySetter<std::wstring> : PropertyLexSetter<std::wstring>
			{
				;
			};
			template<>
			struct PropertySetter<std::wstring&> : PropertyLexSetter<std::wstring&>
			{
				;
			};
			template<>
			struct PropertySetter<std::wstring const&> : PropertyLexSetter<std::wstring const&>
			{
				;
			};
			template<>
			struct PropertySetter<char const*> : PropertyLexSetter<char const*>
			{
				;
			};
			template<>
			struct PropertySetter<char const[]> : PropertyLexSetter<char const[]>
			{
				;
			};
			template<>
			struct PropertySetter<wchar_t const*> : PropertyLexSetter<wchar_t const*>
			{
				;
			};
			template<>
			struct PropertySetter<wchar_t const[]> : PropertyLexSetter<wchar_t const[]>
			{
				;
			};
		}
	}
}

#endif // properties_Boost_headers_dynamic_lex_access_hpp__
