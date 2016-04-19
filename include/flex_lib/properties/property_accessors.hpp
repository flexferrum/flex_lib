/*****************************************************************************************
	Created:	09.05.2008	1:31

	Purpose:	
******************************************************************************************
	History:

	20080509 Flex Ferrum:
******************************************************************************************/

#ifndef Sandbox_Source_Files_property_write_property_accessors_hpp__
#define Sandbox_Source_Files_property_write_property_accessors_hpp__

#include "property_traits.hpp"
#include <boost/utility/enable_if.hpp>

namespace boost
{
	namespace properties
	{
		namespace detail
		{
			template<typename T1 = void, typename T2 = void, typename T3 = void >
			struct EmptyBase {};


/*
			template<typename P, typename T, bool IsRef>
			class ArithmeticOperators
			{
			private:
				P& derived() {return *static_cast<P*>(this);}
				P const& derived() const {return *static_cast<P const*>(this);}
			public:
				typedef T value_type;
				typedef PropertyTraits<T> traits;

#define PROPERTY_BINARY_OPERATOR_COMMUTATIVE(OP )                             \
				template<typename T1>											  \
				friend typename traits::clear_type								\
				operator OP( const P& lhs, T1 rhs )									\
				{																  \
					typename traits::clear_type result( lhs.get() );			  \
					result OP##= rhs;											  \
					return result;												  \
				}																  \
				template<typename T1>											  \
				friend typename traits::clear_type								\
				operator OP( T1 lhs, const P& rhs ) \
				{																  \
					typename traits::clear_type result( lhs );					  \
					result OP##= rhs.get();										  \
					return result;												  \
				}																  \
				template<typename P1>											  \
				friend typename traits::clear_type								\
				operator OP( const P1& lhs, const P1& rhs ) \
				{																  \
					typename traits::clear_type result( lhs.get() );			  \
					result OP##= lhs.get();										  \
					return result;												  \
				}																  

				PROPERTY_BINARY_OPERATOR_COMMUTATIVE(+)
				PROPERTY_BINARY_OPERATOR_COMMUTATIVE(-)
				PROPERTY_BINARY_OPERATOR_COMMUTATIVE(*)
				PROPERTY_BINARY_OPERATOR_COMMUTATIVE(/)
				PROPERTY_BINARY_OPERATOR_COMMUTATIVE(%)
				PROPERTY_BINARY_OPERATOR_COMMUTATIVE(>>)
				PROPERTY_BINARY_OPERATOR_COMMUTATIVE(<<)
				PROPERTY_BINARY_OPERATOR_COMMUTATIVE(&)
				PROPERTY_BINARY_OPERATOR_COMMUTATIVE(|)
				PROPERTY_BINARY_OPERATOR_COMMUTATIVE(^)
#undef PROPERTY_BINARY_OPERATOR_COMMUTATIVE

			};
*/

//			template<typename P, typename T>
//			class ArithmeticOperators<P, T, false> {;};

			template<typename P, typename T, typename B = EmptyBase<> >
			class ReadPropertyAccessors : public B
			{
			private:
				P& derived() {return *static_cast<P*>(this);}
				P const& derived() const {return *static_cast<P const*>(this);}
			public:
				typedef T value_type;
				typedef PropertyTraits<T> traits;
				BOOST_STATIC_CONSTANT(bool, has_getter = 1);
				BOOST_STATIC_CONSTANT(bool, has_setter = 0);

				typename traits::result_type safe_get() const {return derived().get();}
				void safe_set(typename traits::arg_type v) {;}

				template<typename T1>
				operator T1() const
				{
					return derived().get();
				}

				operator typename traits::result_type() const
				{
					return derived().get();
				}

#if 0
# define PROPERTY_LOGICALOPER_SET(OPER)											\
				template<typename T1>											\
				bool operator OPER (T1 other) const								\
				{																\
					return derived().get() OPER T(other);	\
				}																\
																				\
				bool operator OPER (typename traits::arg_type other) const		\
				{																\
					return derived().get() OPER T(other);	\
				}																\
																				\
				bool operator OPER (const P& other) const						\
				{																\
					return derived().get() OPER other.get();	\
				}																
				
				PROPERTY_LOGICALOPER_SET(==)
				PROPERTY_LOGICALOPER_SET(<)
				PROPERTY_LOGICALOPER_SET(>)				
# undef PROPERTY_LOGICALOPER_SET
#endif

				bool operator !() const											
				{																
					return !derived().get();												
				}	

				friend std::ostream& operator << (std::ostream& os, P const& prop)
				{
					os << prop.get();
					return os;
				}
			};

			template<typename T, bool IsRef>
			class SafeGetter
			{
			public:
				typedef PropertyTraits<T> traits;
				typename traits::result_type safe_get() const {return typename traits::result_type();}
			};

			template<typename T>
			class SafeGetter<T, true>
			{
			public:
				typedef PropertyTraits<T> traits;
				typename traits::result_type safe_get() const {return *(typename traits::clear_type*)NULL;}
			};

			template<typename P, typename T, typename B = EmptyBase<> >
			class WritePropertyAccessors : public B, public SafeGetter<T, PropertyTraits<T>::is_reference && !PropertyTraits<T>::is_scalar>
			{
			private:
				P& derived() {return *static_cast<P*>(this);}
				P const& derived() const {return *static_cast<P const*>(this);}
			public:
				typedef T value_type;
				typedef PropertyTraits<T> traits;
				BOOST_STATIC_CONSTANT(bool, has_getter = 0);
				BOOST_STATIC_CONSTANT(bool, has_setter = 1);
				void safe_set(typename traits::arg_type v) {derived().set(v);}

				P& operator ++()
				{
					typename traits::clear_type new_val(derived().get());

					++ new_val;
					derived().set(new_val);

					return derived();
				}

				P& operator --()
				{
					typename traits::clear_type new_val(derived().get());

					-- new_val;
					derived().set(new_val);

					return derived();
				}

				P& operator =(typename traits::arg_type value)
				{
					derived().set(value);
					return derived();
				}

				friend std::istream& operator >> (std::istream& is, P& prop)
				{
					T val;
					is >> val;
					prop.set(val);
					return is;
				}
			};

			template<typename P, typename T, typename EB = EmptyBase<> >
			class ReadWritePropertyAccessors : public WritePropertyAccessors<P, T, ReadPropertyAccessors<P, T> >
			{
			private:
				P& derived() {return *static_cast<P*>(this);}
				P const& derived() const {return *static_cast<P const*>(this);}
			public:
				typedef T value_type;
				typedef PropertyTraits<T> traits;
				BOOST_STATIC_CONSTANT(bool, has_getter = 1);
				BOOST_STATIC_CONSTANT(bool, has_setter = 1);
				typename traits::result_type safe_get() const {return derived().get();}
				void safe_set(typename traits::arg_type v) {derived().set(v);}

#if 1
# define PROPERTY_ASSIGNOPER_SET(OPER)											\
																				\
				template<typename T1>											\
				P& operator OPER (T1 other)										\
				{																\
					typename traits::clear_type new_val(derived().get());		\
																				\
					new_val OPER T(other);										\
					derived().set(new_val);										\
					return derived();											\
				}																\
																				\
				P& operator OPER (typename traits::arg_type other)				\
				{																\
					typename traits::clear_type new_val(derived().get());		\
																				\
					new_val OPER other;											\
					derived().set(new_val);										\
					return derived();											\
				}																

				PROPERTY_ASSIGNOPER_SET(+=);
				PROPERTY_ASSIGNOPER_SET(-=);
				PROPERTY_ASSIGNOPER_SET(*=);
				PROPERTY_ASSIGNOPER_SET(/=);
				PROPERTY_ASSIGNOPER_SET(%=);
				PROPERTY_ASSIGNOPER_SET(&=);
				PROPERTY_ASSIGNOPER_SET(|=);
				PROPERTY_ASSIGNOPER_SET(^=);
				PROPERTY_ASSIGNOPER_SET(>>=);
				PROPERTY_ASSIGNOPER_SET(<<=);

# undef PROPERTY_ASSIGNOPER_SET
#endif

			};
		}
	}
}

#endif // Sandbox_Source_Files_property_write_property_accessors_hpp__
