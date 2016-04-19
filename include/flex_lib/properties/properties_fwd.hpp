/*****************************************************************************************
	Created:	04.05.2008	14:5

	Purpose:	
******************************************************************************************
	History:

	20080504 Flex Ferrum:
******************************************************************************************/

#ifndef Sandbox_Source_Files_property_properties_fwd_hpp__
#define Sandbox_Source_Files_property_properties_fwd_hpp__

#include <iostream>
// #include <boost/static_assert.hpp>
// #include <boost/utility/enable_if.hpp>
// #include <boost/type_traits/is_convertible.hpp>
#include <boost/operators.hpp>
#include <boost/assert.hpp>
#include "property_traits.hpp"
// #include "default_property_base.hpp"
#include "property_accessors.hpp"

namespace boost
{
	namespace properties
	{
		namespace detail
		{
			template<typename T, typename H> class PropertyImpl;

			template<typename Traits, int ValKind>
			class PropertyResWrapperBase {;};

			template<typename Traits>
			class PropertyResWrapperBase<Traits, 0> // pointer
			{
			public:
				PropertyResWrapperBase(typename Traits::ptr_type val) : m_Value(val) {;}
				// PropertyResWrapperBase(typename Traits::arg_type val) : m_Value(&val) {;}

				typename Traits::ptr_type operator ->() const {return m_Value;}
				typename Traits::ptr_type operator ->() {return m_Value;}
			private:
				typename Traits::ptr_type m_Value;
			};

			template<typename Traits>
			class PropertyResWrapperBase<Traits, 1> // reference
			{
			public:
				// PropertyResWrapperBase(typename Traits::ptr_type val) : m_Value(val) {;}
				PropertyResWrapperBase(typename Traits::result_type val) : m_Value(&val) {;}

				typename Traits::ptr_type operator ->() const {return m_Value;}
				typename Traits::ptr_type operator ->() {return m_Value;}
			private:
				typename Traits::ptr_type m_Value;
			};

			template<typename Traits>
			class PropertyResWrapperBase<Traits, 2> // value
			{
			public:
				PropertyResWrapperBase(typename Traits::result_type val) : m_Value(val) {;}

				typename Traits::const_ptr_type operator ->() const {return &m_Value;}
				typename Traits::ptr_type operator ->() {return const_cast<Traits::ptr_type>(&m_Value);} // !!!! Удивительное дело. По всем признакам, m_Value не должно быть константным типом. И тем не менее, без const_cast не работает.

			private:
				typename Traits::result_type m_Value;
			};

			template<typename Traits>
			class PropertyResWrapper : public PropertyResWrapperBase<Traits, Traits::is_pointer ? 0 : (Traits::is_reference ? 1 : 2) >
			{
			public:
				typedef PropertyResWrapperBase<Traits, Traits::is_pointer ? 0 : (Traits::is_reference ? 1 : 2)> base_type;
				PropertyResWrapper(typename Traits::result_type res) : base_type(res) {;}
			};
		}

		template<typename T, typename H, template<typename, typename> class IT>
		class DefaultPropertyBase
		{
		public:
			typedef IT<T, H> ImplType;
			typedef DefaultPropertyBase<T, H, IT> this_type;
			typedef detail::PropertyTraits<T> traits;
			typedef typename ImplType::arg_type arg_type;

			DefaultPropertyBase() : m_Impl(NULL), m_Host(NULL) {;}

			typename traits::result_type get() const
			{
				BOOST_ASSERT(m_Impl != NULL);
				return m_Impl->get(m_Host);
			}
			void set(typename traits::arg_type value)
			{
				BOOST_ASSERT(m_Impl != NULL);
				m_Impl->set(m_Host, value);
			}

			void Init(H* host, ImplType* impl)
			{
				m_Host = host;
				m_Impl = impl;
			}

		private:
			ImplType* m_Impl;
			H* m_Host;
		};

		template<typename T, typename H, class Base, template<typename, typename, typename> class ACC = detail::ReadWritePropertyAccessors>
		class Property : public Base,
			public ACC<Property<T, H, Base, ACC>, T, detail::EmptyBase<> >
		{
		public:
			typedef Property<T, H, Base, ACC> this_type;
			typedef Base base_type;
			typedef T value_type;
			typedef detail::PropertyTraits<T> traits;
			typedef H host_type;
			typedef ACC<Property<T, H, Base, ACC>, T, detail::EmptyBase<> > Accessor;
			typedef typename traits::result_type result_type;
			typedef typename traits::const_result_type const_result_type;
			typedef typename traits::nconst_result_type nconst_result_type;
			// typedef typename traits::arg_type arg_type;
			typedef typename Base::arg_type arg_type;

			result_type operator()() const
			{
				static_assert(Accessor::has_getter != 0, "Property doesn't have get accessor");
				return base_type::get();
			}
			nconst_result_type operator()()
			{
				static_assert(Accessor::has_getter != 0, "Property doesn't have get accessor");
				return base_type::get();
			}
			const_result_type operator*() const
			{
				static_assert(Accessor::has_getter != 0, "Property doesn't have get accessor");
				return base_type::get();
			}
			nconst_result_type operator*()
			{
				static_assert(Accessor::has_getter != 0, "Property doesn't have get accessor");
				return base_type::get();
			}
			template<typename T1>
			this_type operator()( T1 value)
			{
				static_assert(Accessor::has_setter != 0, "Property doesn't have set accessor");
				base_type::set(value);
				return *this;
			}

			typename detail::PropertyResWrapper<traits> operator ->()
			{
				return detail::PropertyResWrapper<traits>(base_type::get());
			}

			typename detail::PropertyResWrapper<traits> operator ->() const
			{
				return detail::PropertyResWrapper<traits>(base_type::get());
			}

			// template<typename T1>
			// this_type& operator = (T1 value)
			this_type& operator = (arg_type value)
			{
				static_assert(Accessor::has_setter != 0, "Property doesn't have getter");
				base_type::set(value);
				return *this;
			}

			using Accessor::operator =;
		};

#define PROPERTY_TPL_DECL template<typename T, typename H, class Base, template<typename, typename, typename> class>

	}
}

#endif // Sandbox_Source_Files_property_properties_fwd_hpp__
