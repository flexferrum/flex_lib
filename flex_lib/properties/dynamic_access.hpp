/*****************************************************************************************
	Created:	14.01.2009	23:51

	Purpose:	
******************************************************************************************
	History:

	20090114 Flex Ferrum:
******************************************************************************************/

#ifndef properties_Boost_headers_dynamic_access_hpp__
#define properties_Boost_headers_dynamic_access_hpp__

#include <string>
#include <exception>
#include <type_traits>

namespace boost
{
	namespace properties
	{
		class PropertyInaccessable : public std::exception
		{
		public:
			PropertyInaccessable(const char* name)
			{
				m_Description = std::string("Property '") + name + std::string("' is inaccessable");
			}
			~PropertyInaccessable() throw() {;}
			const char* what() const throw() {return m_Description.c_str();}
		private:
			std::string m_Description;
		};

		class PropertyNotDefined : public std::exception
		{
		public:
			PropertyNotDefined(const char* name, const char* className)
			{
				m_Description = std::string("Property '") + name + std::string("' is not defined for the class '") + className + std::string("'");
			}
			~PropertyNotDefined() throw() {;}
			const char* what() const throw() {return m_Description.c_str();}
		private:
			std::string m_Description;
		};
		
		namespace detail
		{
			template<typename R>
			struct PropertyGetter
			{
				template<typename P>
				static typename boost::enable_if_c<
					std::is_convertible<typename P::result_type, R>::value &&
					P::has_getter, R>::type
					Get(const char* name, P const& prop)
				{
					return prop.safe_get();
				}
				template<typename P>
				static typename std::enable_if<
					!std::is_convertible<typename P::result_type, R>::value ||
					!P::has_getter, R>::type
					Get(const char* name, P const& prop)
				{
					throw PropertyInaccessable(name);
				}
			};		

			template<typename V>
			struct PropertySetter
			{
				template<typename P>
				static typename std::enable_if<
					std::is_convertible<V, typename P::arg_type>::value &&
					P::has_setter, void>::type
					Set(const char* name, P& prop, V val)
				{
					prop.safe_set(val);
				}
				template<typename P>
				static typename std::enable_if<
					!std::is_convertible<V, typename P::arg_type>::value ||
					!P::has_setter, void>::type
					Set(const char* name, P& prop, V val)
				{
					throw PropertyInaccessable(name);
				}
			};

			template<typename T, int C>
			struct PropertyAccessorBase
			{
				template<class R>
				static R GetProperty(T const& obj, std::string const& name)
				{
					return obj.properties__.template invoke_get<R>(name);
				}
				template<typename V>
				static void SetProperty(T& obj, std::string const& name, V val)
				{
					obj.properties__.invoke_set(name, val);
				}
			};

			template<typename T>
			struct PropertyAccessorBase<T, true>
			{
				template<typename R>
				static R GetProperty(T const& obj, std::string const& name)
				{
					return obj.template get_property<R>(name);
				}
				template<typename V>
				static void SetProperty(T& obj, std::string const& name, V val)
				{
					obj.set_property(name, val);
				}
			};

			template<typename T>
			struct HasSpecialAccessor
			{
				struct RightType
				{
					int dummy;
				};

				struct WrongType
				{
					int dummy[2];
				};

				template<typename U, int (U::*fn)(std::string const&) const> struct CheckFn
				{
					typedef RightType result;
				};

#ifdef _MSC_VER
				template<class U> static typename CheckFn<U, (int (U::*)(std::string const&) const)&U::template get_property<int> >::result HasFn(U*);
#else				
				template<typename U> static typename CheckFn<U, &U::get_property >::result HasFn(U*);
#endif				
				static WrongType HasFn(...);

				BOOST_STATIC_CONSTANT(int, value = sizeof(HasFn((T*)NULL)) == sizeof(RightType) ? 1 : 0);
			};

			template<typename T>
			struct PropertyAccessor : PropertyAccessorBase<T, HasSpecialAccessor<T>::value>
			{
				;
			};
		}

		template<typename R, typename T>
		R get_property(T const& obj, std::string const& propName)
		{
			return detail::PropertyAccessor<T>::template GetProperty<R>(obj, propName);
		}

		template<typename V, typename T>
		void set_property(T& obj, std::string const& propName, V val)
		{
			detail::PropertyAccessor<T>::SetProperty(obj, propName, val);
		}
	}
}

#endif // properties_Boost_headers_dynamic_access_hpp__
