/*****************************************************************************************
	Created:	04.05.2008	14:32

	Purpose:	
******************************************************************************************
	History:

	20080504 Flex Ferrum:
******************************************************************************************/

#ifndef Sandbox_Source_Files_property_index_property_impl_hpp__
#define Sandbox_Source_Files_property_index_property_impl_hpp__

#include "properties_fwd.hpp"
#include "index_property_fwd.hpp"

namespace boost
{
	namespace properties
	{
		namespace detail
		{
			template<typename I, int Args> class IndexPropertyImplBase {};

			template<typename I>
			class IndexPropertyImplBase<I, 1>
			{
			public:
				typedef typename ArgMaker<typename I::Indexes>::arg_type args;
				typedef typename ArgMaker<typename I::Indexes>::rest_args_type rest_args;
				typedef typename I::TypeTraits traits;
				typedef typename I::Host H;
				typedef typename traits::result_type (H::*GetterType)(typename t_arg<args, 0>::type) const;
				typedef void (H::*SetterType)(typename t_arg<args, 0>::type i0, typename traits::arg_type val);

				IndexPropertyImplBase(GetterType getter, SetterType setter) : m_Getter(getter), m_Setter(setter) {;}
				typename traits::result_type get(H const* host, const args& idx) const
				{
					return (host->*m_Getter)(std::get<0>(idx));
				}
				void set(H* host, const args& idx, typename traits::arg_type val)
				{
					(host->*m_Setter)(std::get<0>(idx), val);
				}
			private:
				GetterType m_Getter;
				SetterType m_Setter;
			};

			template<typename I>
			class IndexPropertyImplBase<I, 2>
			{
			public:
				typedef typename ArgMaker<typename I::Indexes>::arg_type args;
				typedef typename ArgMaker<typename I::Indexes>::rest_args_type rest_args;
				typedef typename I::TypeTraits traits;
				typedef typename I::Host H;
				typedef typename traits::result_type (H::*GetterType)(typename t_arg<args, 0>::type, typename t_arg<args, 1>::type) const;
				typedef void (H::*SetterType)(typename t_arg<args, 0>::type, typename t_arg<args, 1>::type, typename traits::arg_type val);

				IndexPropertyImplBase(GetterType getter, SetterType setter) : m_Getter(getter), m_Setter(setter) {;}
				typename traits::result_type get(H const* host, const args& idx) const
				{
					return (host->*m_Getter)(std::get<0>(idx), std::get<1>(idx));
				}
				void set(H* host, const args& idx, typename traits::arg_type val)
				{
					(host->*m_Setter)(std::get<0>(idx), std::get<1>(idx), val);
				}
			private:
				GetterType m_Getter;
				SetterType m_Setter;
			};

			template<typename I>
			class IndexPropertyImplBase<I, 3>
			{
			public:
				typedef typename ArgMaker<typename I::Indexes>::arg_type args;
				typedef typename ArgMaker<typename I::Indexes>::rest_args_type rest_args;
				typedef typename I::TypeTraits traits;
				typedef typename I::Host H;
				typedef typename traits::result_type (H::*GetterType)(typename t_arg<args, 0>::type, typename t_arg<args, 1>::type, typename t_arg<args, 2>::type) const;
				typedef void (H::*SetterType)(typename t_arg<args, 0>::type, typename t_arg<args, 1>::type, typename t_arg<args, 2>::type, typename traits::arg_type val);

				IndexPropertyImplBase(GetterType getter, SetterType setter) : m_Getter(getter), m_Setter(setter) {;}
				typename traits::result_type get(H const* host, const args& idx) const
				{
					return (host->*m_Getter)(std::get<0>(idx), std::get<1>(idx), std::get<2>(idx));
				}
				void set(H* host, const args& idx, typename traits::arg_type val)
				{
					(host->*m_Setter)(std::get<0>(idx), std::get<1>(idx), std::get<2>(idx), val);
				}
			private:
				GetterType m_Getter;
				SetterType m_Setter;
			};

			template<typename I>
			class IndexPropertyImpl : public IndexPropertyImplBase<I, I::IndexesCount>
			{
			public:
				typedef IndexPropertyImplBase<I, I::IndexesCount> base_type;

				IndexPropertyImpl(typename base_type::GetterType g, typename base_type::SetterType s) : base_type(g, s) {;}
			};

			template<typename I>
			class AbstractIndexPropertyImpl
			{
			public:
				typedef typename I::TypeTraits traits;
				typedef I PropertyInfo;

				virtual typename traits::result_type get(const typename I::Host*, const typename I::Indexes& args) const = 0;
				virtual void set(const typename I::Host*, const typename I::Indexes& args, typename traits::arg_type value) = 0;
			};

			template<typename B, typename P>
			class AbstractIndexPropImpl : public B//public AbstractIndexPropertyImpl<I>
			{
			public:
				typedef typename B::traits traits;
				typedef typename B::PropertyInfo I;
				typename traits::result_type get(const typename I::Host* h, const typename I::Indexes& args) const
				{
					return m_Prop->get(args);
				}
				void set(const typename I::Host* h, const typename I::Indexes& args, typename traits::arg_type value)
				{
					m_Prop->set(args, value);
				}
				void Init(P* prop)
				{
					m_Prop = prop;
				}
			private:
				P*   m_Prop;
			};

		}
	}
}

#endif // Sandbox_Source_Files_property_index_property_impl_hpp__
