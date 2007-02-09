
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

// Preprocessed code, do not edit manually !

namespace boost { namespace function_types { namespace detail {
template<typename S, std::size_t N>
struct function_type_impl<no_function,S, N>
{
  typedef S type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 1>
{
  template<typename R >
  struct synthesize
  {
    typedef R type ( );
  };
        typedef typename mpl::begin<S> :: type itrR;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 1>
{
  template<typename R >
  struct synthesize
  {
    typedef R type ( ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R type ( T0 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R type ( T0 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R type ( T0 , T1 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R type ( T0 , T1 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
} } }

// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

// Preprocessed code, do not edit manually !

namespace boost { namespace function_types { namespace detail {
template<typename S, std::size_t N>
struct function_type_impl<no_function,S, N>
{
  typedef S type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 1>
{
  template<typename R >
  struct synthesize
  {
    typedef R type ( );
  };
        typedef typename mpl::begin<S> :: type itrR;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 1>
{
  template<typename R >
  struct synthesize
  {
    typedef R type ( ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R type ( T0 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R type ( T0 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 2>
{
  template<typename R , typename T0>
  struct synthesize
  {
    typedef R ( T0::* type ) ( ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R type ( T0 , T1 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R type ( T0 , T1 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 3>
{
  template<typename R , typename T0 , typename T1>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 4>
{
  template<typename R , typename T0 , typename T1 , typename T2>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 5>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 6>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 7>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 8>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 9>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 10>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 11>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_non_variadic_defaultcall_function, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<plain_variadic_defaultcall_function, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R type ( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<non_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... );
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_non_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) const;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_non_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<volatile_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_non_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
template<typename S>
struct function_type_impl<const_volatile_variadic_defaultcall_member_function_pointer, S, 12>
{
  template<typename R , typename T0 , typename T1 , typename T2 , typename T3 , typename T4 , typename T5 , typename T6 , typename T7 , typename T8 , typename T9 , typename T10>
  struct synthesize
  {
    typedef R ( T0::* type ) ( T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ... ) const volatile;
  };
        typedef typename mpl::begin<S> :: type itrR;
        typedef typename mpl::next<itrR> :: type itr0;
        typedef typename mpl::next<itr0> :: type itr1;
        typedef typename mpl::next<itr1> :: type itr2;
        typedef typename mpl::next<itr2> :: type itr3;
        typedef typename mpl::next<itr3> :: type itr4;
        typedef typename mpl::next<itr4> :: type itr5;
        typedef typename mpl::next<itr5> :: type itr6;
        typedef typename mpl::next<itr6> :: type itr7;
        typedef typename mpl::next<itr7> :: type itr8;
        typedef typename mpl::next<itr8> :: type itr9;
        typedef typename mpl::next<itr9> :: type itr10;
  typedef typename
    synthesize
        < typename mpl::deref< itrR >::type
        , typename mpl::deref< itr0 >::type
        , typename mpl::deref< itr1 >::type
        , typename mpl::deref< itr2 >::type
        , typename mpl::deref< itr3 >::type
        , typename mpl::deref< itr4 >::type
        , typename mpl::deref< itr5 >::type
        , typename mpl::deref< itr6 >::type
        , typename mpl::deref< itr7 >::type
        , typename mpl::deref< itr8 >::type
        , typename mpl::deref< itr9 >::type
        , typename mpl::deref< itr10 >::type
    >::type
  type;
};
} } }
