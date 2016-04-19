/*****************************************************************************************
	Created:	04.05.2008	18:34

	Purpose:	
******************************************************************************************
	History:

	20080504 Flex Ferrum:
******************************************************************************************/

#ifndef Sandbox_Source_Files_property_property_traits_hpp__
#define Sandbox_Source_Files_property_property_traits_hpp__

#include <type_traits>
// #include <boost/type_traits/is_scalar.hpp>
// #include <boost/type_traits/is_pointer.hpp>
// #include <boost/type_traits/is_const.hpp>
// #include <boost/type_traits/remove_all_extents.hpp>
// #include <boost/type_traits/remove_reference.hpp>
// #include <boost/type_traits/remove_pointer.hpp>
// #include <boost/type_traits/add_pointer.hpp>
// #include <boost/type_traits/add_const.hpp>
// #include <boost/mpl/if.hpp>

namespace boost
{
	namespace properties
	{

		template<typename T>
		struct by_val
		{
			typedef T value_type;
		};

		namespace detail
		{
			template<typename T, bool IF>
			struct PropertyTraitsBase {;};

			template<typename T, bool>
			struct MakePtrTypeBase
			{
				typedef typename std::add_pointer<T>::type type;
			};

			template<typename T>
			struct MakePtrTypeBase<T, true>
			{
				typedef T type;
			};

			template<typename T>
			struct MakePtrType : public MakePtrTypeBase<T, std::is_pointer<T>::value >
			{
				;
			};

			template<typename T>
			struct PropertyTraitsBase<T, true>
			{
				typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type clear_type;
				typedef clear_type arg_type;
				typedef clear_type result_type;
				typedef result_type nconst_result_type;
				typedef const result_type const_result_type;
				typedef typename MakePtrType<T>::type ptr_type;
				typedef typename MakePtrType<T const>::type const_ptr_type;
				BOOST_STATIC_CONSTANT(bool, is_scalar = true);
				BOOST_STATIC_CONSTANT(bool, is_const = false);
				BOOST_STATIC_CONSTANT(bool, is_reference = std::is_reference<T>::value);
				BOOST_STATIC_CONSTANT(bool, is_pointer = std::is_pointer<T>::value);
			};

			template<typename T, bool IsRef> 
			struct NonScalarTypeTraits
			{
				typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type clear_type;
				typedef const clear_type& arg_type;
				typedef arg_type result_type;
				typedef arg_type const_result_type;
				typedef typename std::remove_cv<result_type>::type nconst_result_type;
				BOOST_STATIC_CONSTANT(bool, is_const = std::is_const<typename std::remove_pointer<T>::type>::value);
			};

			template<typename T> 
			struct NonScalarTypeTraits<T, true>
			{
				typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type clear_type;
				typedef clear_type& arg_type;
				typedef T result_type;
				typedef clear_type const& const_result_type;
				typedef typename std::remove_cv<result_type>::type nconst_result_type;
				BOOST_STATIC_CONSTANT(bool, is_const = std::is_const<typename std::remove_reference<T>::type>::value);
			};

			template<typename T>
			struct PropertyTraitsBase<T, false> : NonScalarTypeTraits<T, std::is_reference<T>::value>
			{
				typedef typename MakePtrType<T>::type ptr_type;
				typedef typename MakePtrType<typename std::add_const<T>::type>::type const_ptr_type;
				BOOST_STATIC_CONSTANT(bool, is_scalar = false);
				BOOST_STATIC_CONSTANT(bool, is_reference = std::is_reference<T>::value);
				// typedef typename remove_all_extents<T>::type result_type;
				BOOST_STATIC_CONSTANT(bool, is_pointer = std::is_pointer<T>::value);
			};

			template<typename T>
			struct PropertyTraitsBase<by_val<T>, false>
			{
				typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type clear_type;
				typedef clear_type arg_type;
				typedef clear_type result_type;
				typedef const clear_type const_result_type;
				typedef clear_type nconst_result_type;
				typedef typename MakePtrType<T>::type ptr_type;
				typedef typename MakePtrType<typename std::add_const<T>::type>::type const_ptr_type;
				BOOST_STATIC_CONSTANT(bool, is_scalar = false);
				BOOST_STATIC_CONSTANT(bool, is_const = false);
				BOOST_STATIC_CONSTANT(bool, is_reference = std::is_reference<T>::value);
				BOOST_STATIC_CONSTANT(bool, is_pointer = std::is_pointer<T>::value);
			};

			template<typename T>
			struct PropertyTraits : public PropertyTraitsBase<T, std::is_scalar<T>::value>
			{
				;
			};
		}
	}
}

#endif // Sandbox_Source_Files_property_property_traits_hpp__
