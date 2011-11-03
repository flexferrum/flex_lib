/*****************************************************************************************
	Created:	04.05.2008	14:32

	Purpose:	
******************************************************************************************
	History:

	20080504 Flex Ferrum:
******************************************************************************************/

#ifndef Sandbox_Source_Files_property_index_property_fwd_hpp__
#define Sandbox_Source_Files_property_index_property_fwd_hpp__

// #include <boost/tuple/tuple.hpp>
#include <tuple>
#include "properties_fwd.hpp"

namespace boost
{
	namespace properties
	{
		namespace detail
		{
			// template<typename I> struct IndexesChain;
			// template<typename IDXS, int count> struct ArgMaker;
			template<typename IDXS> struct ArgMaker;
			template<typename I, typename TUP, typename HTUP> class Indexer;
			template<typename I> class IndexPropertyBase;
			template<typename I> class IndexPropertyImpl;
			template<typename I> class AbstractIndexPropertyImpl;
			template<typename I, typename P> class AbstractIndexPropImpl;

			template<typename TP, int AI>
			struct t_arg
			{
				typedef typename std::tuple_element<AI, TP>::type type;
			};

			template<typename T, typename H, typename IDX, template<typename, typename, typename> class ACC>
			struct IndexPropertyInfo
			{
				typedef IndexPropertyInfo<T, H, IDX, ACC> this_type;
				typedef IndexPropertyBase<this_type> PropertyTypeBase;
				typedef IndexPropertyImpl<this_type> PropertyImpl;
				typedef Property<T, H, PropertyTypeBase, ACC> PropertyType;
				BOOST_STATIC_CONSTANT(int, IndexesCount = std::tuple_size<IDX>::value);
				typedef PropertyTraits<T> TypeTraits;
				typedef IDX Indexes; 
				typedef typename ArgMaker<IDX>::arg_type Args;
				typedef typename ArgMaker<IDX>::rest_args_type RestArgs;
				typedef H Host;
				typedef T Type;
			};

			template<typename T, typename H, typename IDX, template<typename, typename, typename> class ACC>
			struct AbstractIndexPropertyInfo
			{
				typedef AbstractIndexPropertyInfo<T, H, IDX, ACC> this_type;
				typedef IndexPropertyBase<this_type> PropertyTypeBase;
				typedef AbstractIndexPropertyImpl<this_type> PropertyImpl;
				typedef Property<T, H, PropertyTypeBase, ACC> PropertyType;
				BOOST_STATIC_CONSTANT(int, IndexesCount = std::tuple_size<IDX>::value);
				typedef PropertyTraits<T> TypeTraits;
				typedef IDX Indexes; 
				typedef typename ArgMaker<IDX>::arg_type Args;
				typedef typename ArgMaker<IDX>::rest_args_type RestArgs;
				typedef H Host;
				typedef T Type;
			};

			template<> struct ArgMaker<std::tuple<>> {};

			template<typename T1>
			struct ArgMaker<std::tuple<T1>>
			{
				typedef std::tuple<typename PropertyTraits<T1>::arg_type> arg_type;
				typedef std::tuple<> rest_args_type;

				template<typename U1, typename U2>
				static arg_type append_arg(U1, U2 val)
				{
					return arg_type(val);
				}
			};

			template<typename T1, typename T2>
			struct ArgMaker<std::tuple<T1, T2>>
			{
				typedef std::tuple<
					typename PropertyTraits<T1>::arg_type,
					typename PropertyTraits<T2>::arg_type
				> arg_type;

				typedef std::tuple<
					typename PropertyTraits<T2>::arg_type
				> rest_args_type;

				template<typename U1, typename U2>
				static arg_type append_arg(U1 a, U2 val)
				{
					return arg_type(get<0>(a), val);
				}
			};

			template<typename T1, typename T2, typename T3>
			struct ArgMaker<std::tuple<T1, T2, T3>>
			{
				typedef std::tuple<
					typename PropertyTraits<T1>::arg_type,
					typename PropertyTraits<T2>::arg_type,
					typename PropertyTraits<T3>::arg_type
				> arg_type;

				typedef std::tuple<
					typename PropertyTraits<T2>::arg_type,
					typename PropertyTraits<T3>::arg_type
				> rest_args_type;

				template<typename U1, typename U2>
				static arg_type append_arg(U1 a, U2 val)
				{
					return arg_type(get<0>(a), get<1>(a), val);
				}
			};

			template<typename T1, typename T2, typename T3, typename T4>
			struct ArgMaker<std::tuple<T1, T2, T3, T4>>
			{
				typedef std::tuple<
					typename PropertyTraits<T1>::arg_type,
					typename PropertyTraits<T2>::arg_type,
					typename PropertyTraits<T3>::arg_type,
					typename PropertyTraits<T4>::arg_type
				> arg_type;

				typedef std::tuple<
					typename PropertyTraits<T2>::arg_type,
					typename PropertyTraits<T3>::arg_type,
					typename PropertyTraits<T4>::arg_type
				> rest_args_type;

				template<typename U1, typename U2>
				static arg_type append_arg(U1 a, U2 val)
				{
					return arg_type(get<0>(a), get<1>(a), get<2>(a), val);
				}
			};

			template<typename TUP, typename T> struct TupleAppender
			{
				static void append(TUP const&, T const&) {;}
			};

			template<typename T> struct TupleAppender<std::tuple<>, T>
			{
				typedef std::tuple<T> type;
				static std::tuple<T> append(std::tuple<> const&, T const& v)
				{
					return std::make_tuple(v);
				}
			};

			template<typename T1, typename T> struct TupleAppender<std::tuple<T1>, T>
			{
				typedef std::tuple<T1, T> type;
				static std::tuple<T1, T> append(std::tuple<T1> const& t, T const& v)
				{
					return std::make_tuple(std::get<0>(t), v);
				}
			};

			template<typename T1, typename T2, typename T> struct TupleAppender<std::tuple<T1, T2>, T>
			{
				typedef std::tuple<T1, T2, T> type;
				static std::tuple<T1, T2, T> append(std::tuple<T1, T2> const& t, T const& v)
				{
					return std::make_tuple(std::get<0>(t), std::get<1>(t), v);
				}
			};

			template<typename T1, typename T2, typename T3, typename T> struct TupleAppender<std::tuple<T1, T2, T3>, T>
			{
				typedef std::tuple<T1, T2, T3, T> type;
				static std::tuple<T1, T2, T3, T> append(std::tuple<T1, T2, T3> const& t, T const& v)
				{
					return std::make_tuple(std::get<0>(t), std::get<1>(t), std::get<2>(t), v);
				}
			};
			template<typename TUP, typename T>
			auto append2tuple(TUP const& t, T const& v) -> decltype(TupleAppender<TUP, T>::append(t, v)) 
			{
				return TupleAppender<TUP, T>::append(t, v);
			}


		}
	}
}

#endif // Sandbox_Source_Files_property_index_property_fwd_hpp__
