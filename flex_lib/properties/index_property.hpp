/*****************************************************************************************
	Created:	04.05.2008	14:32

	Purpose:	
******************************************************************************************
	History:

	20080504 Flex Ferrum:
******************************************************************************************/

#ifndef Sandbox_Source_Files_property_index_property_hpp__
#define Sandbox_Source_Files_property_index_property_hpp__

#include "properties_fwd.hpp"
#include "index_property_fwd.hpp"
#include "index_property_impl.hpp"

# define INDEX_PROPERTY_INNER_TYPE(ClassName, PI) PROP_TYPE(PI), ClassName, std::tuple<BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_ELEM(7, PI))>, boost::properties::detail::BOOST_PP_SEQ_ELEM(6, PI)
# define MAKE_PROPERTY_TYPE_IDX(ClassName, PI) boost::properties::IndexProperty<boost::properties::detail::IndexPropertyInfo<INDEX_PROPERTY_INNER_TYPE(ClassName, PI)> >
# define MAKE_PROPERTY_TYPE_ABST_IDX(ClassName, PI) boost::properties::IndexProperty<boost::properties::detail::AbstractIndexPropertyInfo<INDEX_PROPERTY_INNER_TYPE(ClassName, PI)> >
# define MAKE_ABST_PROP_IMPL_TYPE_IDX(ClassName, BaseClassName, PI) \
	boost::properties::detail::AbstractIndexPropImpl<BaseClassName::BOOST_PP_CAT(PROP_NAME(PI),_type_)::impl_type, ClassName::BOOST_PP_CAT(PROP_NAME(PI),_type_)>

//boost::properties::detail::AbstractIndexPropImpl<boost::properties::detail::AbstractIndexPropertyInfo<INDEX_PROPERTY_INNER_TYPE(BaseClassName, PI)>, MAKE_PROPERTY_TYPE_IDX(ClassName, PI) >
# define DEFINE_PROPERTY_IDX(ClassName, PI) \
	typedef MAKE_PROPERTY_TYPE_IDX(ClassName, PI) BOOST_PP_CAT(PROP_NAME(PI),_type_);\
	BOOST_PP_CAT(PROP_NAME(PI),_type_) PROP_NAME(PI);
# define DEFINE_PROPERTY_ABST_IDX(ClassName, PI) \
	typedef MAKE_PROPERTY_TYPE_ABST_IDX(ClassName, PI) BOOST_PP_CAT(PROP_NAME(PI),_type_);\
	BOOST_PP_CAT(PROP_NAME(PI),_type_) PROP_NAME(PI);

# define DEFINE_PROPERTY_IMPL_AS_MEMBER_IDX(ClassName, PI) 
# define DEFINE_PROPERTY_IMPL_AS_MEMBER_ABST_IDX(ClassName, PI) 

# define INDEX_RW_PROPERTY_CTR_INIT_RW(ClassName, PI) &ClassName::BOOST_PP_SEQ_ELEM(8, PI), &ClassName::BOOST_PP_SEQ_ELEM(9, PI)
# define INDEX_RW_PROPERTY_CTR_INIT_RO(ClassName, PI) &ClassName::BOOST_PP_SEQ_ELEM(8, PI), NULL

# define DEFINE_PROPERTY_IMPL_AS_STATIC_IDX(ClassName, PI) \
	static boost::properties::detail::IndexPropertyImpl<boost::properties::detail::IndexPropertyInfo<INDEX_PROPERTY_INNER_TYPE(ClassName, PI) > > BOOST_PP_SEQ_ELEM(4, PI)(BOOST_PP_CAT(INDEX_RW_PROPERTY_CTR_INIT_, BOOST_PP_SEQ_ELEM(5, PI))(ClassName, PI));
	// static boost::properties::detail::BOOST_PP_SEQ_ELEM(3, PI)<PROP_TYPE(PI), ClassName> BOOST_PP_SEQ_ELEM(4, PI)(&ClassName::BOOST_PP_SEQ_ELEM(5, PI), &ClassName::BOOST_PP_SEQ_ELEM(6, PI));
# define DEFINE_PROPERTY_IMPL_AS_STATIC_ABST_IDX(ClassName, PI)

# define INITIALIZE_PROPERTY_IMPL_IDX(ClassName, PI) \
	host->BOOST_PP_SEQ_ELEM(4, PI).Init(host, &BOOST_PP_SEQ_ELEM(4, PI) );
# define INITIALIZE_PROPERTY_IMPL_ABST_IDX(ClassName, PI)

# define COPY_PROPERTY_IMPL_IDX(_1, _2) 
# define COPY_PROPERTY_IMPL_ABST_IDX(_1, _2) 

# define SERIALIZE_PROPERTY_IMPL_IDX(ClassName, PI) 
# define SERIALIZE_PROPERTY_IMPL_ABST_IDX(ClassName, PI) 

# define INVOKE_GET_PROPERTY_IMPL_IDX(ClassName, PI) INVOKE_GET_PROPERTY_IMPL_GENERIC(ClassName, PI)
# define INVOKE_GET_PROPERTY_IMPL_ABST_IDX(ClassName, PI) INVOKE_GET_PROPERTY_IMPL_GENERIC(ClassName, PI)

# define INVOKE_SET_PROPERTY_IMPL_IDX(ClassName, PI) INVOKE_SET_PROPERTY_IMPL_GENERIC(ClassName, PI)
# define INVOKE_SET_PROPERTY_IMPL_ABST_IDX(ClassName, PI) INVOKE_SET_PROPERTY_IMPL_GENERIC(ClassName, PI)

# define ENUM_PROPERTY_IMPL_IDX(ClassName, PI) ENUM_PROPERTY_IMPL_GENERIC(ClassName, PI)
# define ENUM_PROPERTY_IMPL_ABST_IDX(ClassName, PI) ENUM_PROPERTY_IMPL_GENERIC(ClassName, PI)

#define INDEX_PROPERTY(Type, Name, IndexesNum, Indexes, Get, Set) ( (IDX)((Type))(IndexProperty)(IndexPropertyImpl)(Name)(RW)(ReadWritePropertyAccessors)(BOOST_PP_TUPLE_TO_SEQ(IndexesNum, Indexes))(Get)(Set) )
#define RO_INDEX_PROPERTY(Type, Name, IndexesNum, Indexes, Get) ( (IDX)((Type))(IndexProperty)(IndexPropertyImpl)(Name)(RO)(ReadPropertyAccessors)(BOOST_PP_TUPLE_TO_SEQ(IndexesNum, Indexes))(Get) )
#define WO_INDEX_PROPERTY(Type, Name, IndexesNum, Indexes, Set) ( (IDX)((Type))(IndexProperty)(IndexPropertyImpl)(Name)(WO)(WritePropertyAccessors)(BOOST_PP_TUPLE_TO_SEQ(IndexesNum, Indexes))(Set) )

#define INDEX_PROPERTY_V(Type, Name, IndexesNum, Indexes, Get, Set) ( (IDX)((boost::properties::by_val<Type>))(IndexProperty)(IndexPropertyImpl)(Name)(RW)(ReadWritePropertyAccessors)(BOOST_PP_TUPLE_TO_SEQ(IndexesNum, Indexes))(Get)(Set) )
#define RO_INDEX_PROPERTY_V(Type, Name, IndexesNum, Indexes, Get) ( (IDX)((boost::properties::by_val<Type>))(IndexProperty)(IndexPropertyImpl)(Name)(RO)(ReadPropertyAccessors)(BOOST_PP_TUPLE_TO_SEQ(IndexesNum, Indexes))(Get) )
#define WO_INDEX_PROPERTY_V(Type, Name, IndexesNum, Indexes, Set) ( (IDX)((boost::properties::by_val<Type>))(IndexProperty)(IndexPropertyImpl)(Name)(WO)(WritePropertyAccessors)(BOOST_PP_TUPLE_TO_SEQ(IndexesNum, Indexes))(Set) )

#define ABSTRACT_INDEX_PROPERTY(Type, Name, IndexesNum, Indexes) ( (ABST_IDX)((Type))(IndexProperty)(IndexPropertyImpl)(Name)(RW)(ReadWritePropertyAccessors)(BOOST_PP_TUPLE_TO_SEQ(IndexesNum, Indexes))(_)(_) )
#define ABSTRACT_RO_INDEX_PROPERTY(Type, Name, IndexesNum, Indexes) ( (ABST_IDX)((Type))(IndexProperty)(IndexPropertyImpl)(Name)(RO)(ReadPropertyAccessors)(BOOST_PP_TUPLE_TO_SEQ(IndexesNum, Indexes))(_) )
#define ABSTRACT_WO_INDEX_PROPERTY(Type, Name, IndexesNum, Indexes) ( (ABST_IDX)((Type))(IndexProperty)(IndexPropertyImpl)(Name)(WO)(WritePropertyAccessors)(BOOST_PP_TUPLE_TO_SEQ(IndexesNum, Indexes))(_) )

#define ABSTRACT_INDEX_PROPERTY_V(Type, Name, IndexesNum, Indexes) ( (ABST_IDX)((boost::properties::by_val<Type>))(IndexProperty)(IndexPropertyImpl)(Name)(RW)(ReadWritePropertyAccessors)(BOOST_PP_TUPLE_TO_SEQ(IndexesNum, Indexes))(_)(_) )
#define ABSTRACT_RO_INDEX_PROPERTY_V(Type, Name, IndexesNum, Indexes) ( (ABST_IDX)((boost::properties::by_val<Type>))(IndexProperty)(IndexPropertyImpl)(Name)(RO)(ReadPropertyAccessors)(BOOST_PP_TUPLE_TO_SEQ(IndexesNum, Indexes))(_) )
#define ABSTRACT_WO_INDEX_PROPERTY_V(Type, Name, IndexesNum, Indexes) ( (ABST_IDX)((boost::properties::by_val<Type>))(IndexProperty)(IndexPropertyImpl)(Name)(WO)(WritePropertyAccessors)(BOOST_PP_TUPLE_TO_SEQ(IndexesNum, Indexes))(_) )

namespace boost
{
	namespace properties
	{
		namespace detail
		{
			template<typename PI, typename T1, typename HTUP> class Indexer<PI, std::tuple<T1>, HTUP>
			{
			public:
				typedef typename PI::PropertyType result_type;
				typedef T1 arg_type;
				typedef T1 const& const_ref_type;
				typedef Indexer<PI, std::tuple<T1>, HTUP> this_type;

				Indexer(typename PI::Host const* host, typename PI::PropertyImpl const* impl, HTUP const&& head) : m_host(host), m_impl(impl), m_head(head) {;}

				result_type operator[](const_ref_type a)
				{
					result_type ret_val;

					ret_val.Init(const_cast<typename PI::Host*>(m_host), const_cast<typename PI::PropertyImpl*>(m_impl), append2tuple(m_head, a));
					return ret_val;
				}

				const result_type operator[](const_ref_type a) const
				{
					result_type ret_val;

					ret_val.Init(const_cast<typename PI::Host*>(m_host), const_cast<PI::PropertyImpl*>(m_impl), append2tuple(m_head, a));
					return ret_val;
				}
			private:
				HTUP m_head;
				typename PI::Host const* m_host;
				typename PI::PropertyImpl const* m_impl;			
			};

			template<typename PI, typename TUP, typename HTUP> class Indexer
			{
			public:
				typedef typename t_arg<TUP, 0>::type arg_type;
				typedef typename Indexer<PI, typename ArgMaker<TUP>::rest_args_type, typename TupleAppender<HTUP, arg_type>::type> result_type;
				typedef Indexer<PI, TUP, HTUP> this_type;

				Indexer(typename PI::Host const* host, typename PI::PropertyImpl const* impl, HTUP const&& head) : m_host(host), m_impl(impl), m_head(head) {;}

				result_type operator[](const arg_type& a)
				{
					return result_type(m_host, m_impl, append2tuple(m_head, a));
				}

				const result_type operator[](const arg_type& a) const
				{
					return result_type(m_host, m_impl, append2tuple(m_head, a));
				}
			private:
				HTUP m_head;
				typename PI::Host const* m_host;
				typename PI::PropertyImpl const* m_impl;			
			};

			template<typename Info>
			class IndexPropertyBase
			{
			public:
				typedef IndexPropertyBase<Info> this_type;
				typedef typename Info::TypeTraits traits;
				typedef typename Info::PropertyImpl ImplType;
				typedef typename Info::PropertyType PropertyType;
				typedef typename traits::arg_type arg_type;

				IndexPropertyBase() : m_Impl(NULL), m_Host(NULL) {;}

				typename traits::result_type get() const
				{
					static_assert(PropertyType::has_getter, "Property don't have getter");
					BOOST_ASSERT(m_Impl != NULL);
					return m_Impl->get(m_Host, m_Args);
				}
				void set(typename traits::arg_type value)
				{
					static_assert(PropertyType::has_setter, "Property don't have getter");
					BOOST_ASSERT(m_Impl != NULL);
					m_Impl->set(m_Host, m_Args, value);
				}

				void Init(typename Info::Host* host, ImplType* impl, const typename Info::Indexes& args)
				{
					m_Host = host;
					m_Impl = impl;
					m_Args = args;
				}

			private:
				typename Info::Host* m_Host;
				ImplType* m_Impl;
				typename Info::Indexes m_Args;
			};
		} // detail

		template<typename PI> /*T, typename H, typename Indexes, template<typename, typename, typename> class ACC*/
		class IndexProperty // : public detail::SimpleIndex<T, H, Indexes>
		{
		public:
			typedef IndexProperty<PI> this_type;
			// typedef IndexProperty<T, H, Indexes, ACC> this_type;
			// typedef Base base_type;
			typedef typename PI::Type value_type;
			typedef detail::PropertyTraits<value_type> traits;
			typedef typename PI::Host host_type;
			typedef typename traits::result_type result_type;
			typedef typename traits::arg_type arg_type;
			typedef typename PI::PropertyImpl impl_type;
			// typedef typename detail::IndexerTraits<PI, typename PI::Indexes> idx_traits;
			// typedef typename std::tuple_element<0, typename PI::Args>::type idx_arg_type;
			typedef typename PI::Args idx_args;
			typedef std::tuple<> empty_tuple;
			typedef detail::Indexer<PI, idx_args, empty_tuple> idx_result_type;
			typedef typename idx_result_type::arg_type idx_arg_type;
			typedef typename idx_result_type::result_type idx_res_type;
			BOOST_STATIC_CONSTANT(bool, has_getter = 1);
			BOOST_STATIC_CONSTANT(bool, has_setter = 1);
			// typedef detail::IndexPropertyInfo<T, H, Indexes, ACC>  PropertyInfo;
			typedef PI PropertyInfo;

			IndexProperty() : m_Host(NULL), m_Impl(NULL) {;}


			typename idx_res_type operator[](const idx_arg_type& a)
			{
				return idx_result_type(m_Host, m_Impl, std::tuple<>())[a];
			}
			const idx_res_type operator[](const idx_arg_type& a) const
			{
				return idx_result_type(m_Host, m_Impl, std::tuple<>())[a];
			}

			template<typename T1>
			typename idx_res_type operator[](T1 a)
			{
				return idx_result_type(m_Host, m_Impl, std::tuple<>())[a];
			}
			template<typename T1>
			const idx_res_type operator[](T1 a) const
			{
				return idx_result_type(m_Host, m_Impl, std::tuple<>())[a];
			}

			typename traits::result_type safe_get() const
			{
				return traits::result_type();
			}

			void safe_set(typename traits::arg_type val)
			{
			}

			typename traits::result_type get(const typename PI::Indexes& args) const
			{
				return m_Impl->get(m_Host, args);
			}

			void set(const typename PI::Indexes& args, typename traits::arg_type value) const
			{
				m_Impl->set(m_Host, args, value);
			}

			void Init(host_type* host, typename PropertyInfo::PropertyImpl* impl)
			{
				m_Host = host;
				m_Impl = impl;
			}
		private:
			host_type* m_Host;
			typename PropertyInfo::PropertyImpl* m_Impl;
		};
	}
}

#endif // Sandbox_Source_Files_property_index_property_hpp__
