
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

// Preprocessed code, do not edit manually !

namespace boost { namespace function_types { namespace detail {
template<typename T>
struct signature_impl
  : conditioned_transmuted_signature_base
    < signature_impl<T*>, T
    , function_pointer, plain_function
    , nullary_signature_base >
{
  typedef function_type_signature<T> type;
};
template<typename T>
struct signature_impl< T& >
  : conditioned_transmuted_signature_base
    < signature_impl<T*>, T&
    , function_pointer, function_reference
    , nullary_signature_base >
{
  typedef function_type_signature<T&> type;
};
template<typename T>
struct signature_impl< T* >
  : nullary_signature_base
{
  typedef function_type_signature<T*> type;
};
template< typename R >
struct signature_impl
< R ( * ) ( ) >
{
  typedef R ( * representee ) ( );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector1
    < R > types;
};
template< typename R >
struct signature_impl
< R ( * ) ( ... ) >
{
  typedef R ( * representee ) ( ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector1
    < R > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( * ) ( T0 ) >
{
  typedef R ( * representee ) ( T0 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( * ) ( T0 ... ) >
{
  typedef R ( * representee ) ( T0 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ) >
{
  typedef R ( T0::* representee ) ( );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ... ) >
{
  typedef R ( T0::* representee ) ( ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ) const >
{
  typedef R ( T0::* representee ) ( ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ... ) const >
{
  typedef R ( T0::* representee ) ( ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ) volatile >
{
  typedef R ( T0::* representee ) ( ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ... ) volatile >
{
  typedef R ( T0::* representee ) ( ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ) const volatile >
{
  typedef R ( T0::* representee ) ( ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ... ) const volatile >
{
  typedef R ( T0::* representee ) ( ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( * ) ( T0 , T1 ) >
{
  typedef R ( * representee ) ( T0 , T1 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( * ) ( T0 , T1 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ) >
{
  typedef R ( T0::* representee ) ( T1 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ... ) >
{
  typedef R ( T0::* representee ) ( T1 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ) const >
{
  typedef R ( T0::* representee ) ( T1 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 11 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 11 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
} } }

// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

// Preprocessed code, do not edit manually !

namespace boost { namespace function_types { namespace detail {
template<typename T>
struct signature_impl
  : conditioned_transmuted_signature_base
    < signature_impl<T*>, T
    , function_pointer, plain_function
    , nullary_signature_base >
{
  typedef function_type_signature<T> type;
};
template<typename T>
struct signature_impl< T& >
  : conditioned_transmuted_signature_base
    < signature_impl<T*>, T&
    , function_pointer, function_reference
    , nullary_signature_base >
{
  typedef function_type_signature<T&> type;
};
template<typename T>
struct signature_impl< T* >
  : nullary_signature_base
{
  typedef function_type_signature<T*> type;
};
template< typename R >
struct signature_impl
< R ( * ) ( ) >
{
  typedef R ( * representee ) ( );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector1
    < R > types;
};
template< typename R >
struct signature_impl
< R ( * ) ( ... ) >
{
  typedef R ( * representee ) ( ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector1
    < R > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( * ) ( T0 ) >
{
  typedef R ( * representee ) ( T0 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( * ) ( T0 ... ) >
{
  typedef R ( * representee ) ( T0 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ) >
{
  typedef R ( T0::* representee ) ( );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ... ) >
{
  typedef R ( T0::* representee ) ( ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ) const >
{
  typedef R ( T0::* representee ) ( ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ... ) const >
{
  typedef R ( T0::* representee ) ( ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ) volatile >
{
  typedef R ( T0::* representee ) ( ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ... ) volatile >
{
  typedef R ( T0::* representee ) ( ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ) const volatile >
{
  typedef R ( T0::* representee ) ( ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 >
struct signature_impl
< R ( T0::* ) ( ... ) const volatile >
{
  typedef R ( T0::* representee ) ( ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 0 > arity;
  typedef mpl::vector2
    < R , T0 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( * ) ( T0 , T1 ) >
{
  typedef R ( * representee ) ( T0 , T1 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( * ) ( T0 , T1 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ) >
{
  typedef R ( T0::* representee ) ( T1 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ... ) >
{
  typedef R ( T0::* representee ) ( T1 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ) const >
{
  typedef R ( T0::* representee ) ( T1 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 >
struct signature_impl
< R ( T0::* ) ( T1 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 1 > arity;
  typedef mpl::vector3
    < R , T0 , T1 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 2 > arity;
  typedef mpl::vector4
    < R , T0 , T1 , T2 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 3 > arity;
  typedef mpl::vector5
    < R , T0 , T1 , T2 , T3 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 4 > arity;
  typedef mpl::vector6
    < R , T0 , T1 , T2 , T3 , T4 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 5 > arity;
  typedef mpl::vector7
    < R , T0 , T1 , T2 , T3 , T4 , T5 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 6 > arity;
  typedef mpl::vector8
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 7 > arity;
  typedef mpl::vector9
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 8 > arity;
  typedef mpl::vector10
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 9 > arity;
  typedef mpl::vector11
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 );
  typedef non_variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 11 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( * ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) >
{
  typedef R ( * representee ) ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... );
  typedef variadic_defaultcall_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 11 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 );
  typedef non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... );
  typedef variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) const;
  typedef const_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) const >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) const;
  typedef const_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) volatile;
  typedef volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) volatile;
  typedef volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) const volatile;
  typedef const_volatile_non_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
template< typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10 >
struct signature_impl
< R ( T0::* ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) const volatile >
{
  typedef R ( T0::* representee ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) const volatile;
  typedef const_volatile_variadic_defaultcall_member_function_pointer kind;
  typedef function_type_signature< representee > type;
  typedef signature_mpl_sequence_tag tag;
  typedef mpl::size_t< 10 > arity;
  typedef mpl::vector12
    < R , T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 > types;
};
} } }
