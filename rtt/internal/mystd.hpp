/***************************************************************************
  tag: Peter Soetens  Wed Jul 28 09:08:56 CEST 2004  mystd.hpp

                        mystd.hpp -  description
                           -------------------
    begin                : Wed July 28 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef ORO_MYSTD_HPP
#define ORO_MYSTD_HPP

#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/utility.hpp>
#include <functional>
#include <algorithm>
#include <vector>

// here we define some generally useful template stuff that is missing
// from the STL..
namespace RTT { namespace internal {

    // combines remove_reference and remove_const
    template<typename T>
    struct remove_cr
    {
      typedef typename boost::remove_const<
        typename boost::remove_reference<T>::type>::type type;
    };

    template<typename T>
    struct is_pure_reference
    :public boost::mpl::false_
    {};

    template<typename T>
    struct is_pure_reference<T&>
    :public boost::mpl::true_
    {};

    template<typename T>
    struct is_pure_reference<T const&>
    :public boost::mpl::false_
    {};

  template<typename iter>
  static void delete_all( iter a, iter b )
  {
    for ( ; a < b; a++ )
      delete *a;
  }


    // SGI extension, does not seem present in current GNU STL
    // implementation...

    template<typename T>
    struct select1st
      : public std::unary_function<T, typename T::first_type>
    {
        typename T::first_type operator()( const T& p ) {
            return p.first;
        }
    };

    template<typename T>
    struct select2nd
      : public std::unary_function<T, typename T::second_type>
    {
        typename T::second_type operator()( const T& p ) {
            return p.second;
        }
    };

#if 0
    // Alternative implementations, return const ref.
    template<typename PairT>
    class select1st
      : public std::unary_function<PairT, typename PairT::first_type>
    {
      typedef typename PairT::first_type ResultT;
    public:
      const ResultT& operator()( const PairT& p )
        {
          return p.first;
        };
    };

    template<typename PairT>
    class select2nd
      : public std::unary_function<PairT, typename PairT::second_type>
    {
      typedef typename PairT::second_type ResultT;
    public:
      const ResultT& operator()( const PairT& p )
        {
          return p.second;
        };
    };
#endif


    // my own handy little extension..
    template<typename MapT>
    std::vector<typename MapT::mapped_type> values( const MapT& map )
    {
        std::vector<typename MapT::mapped_type> ret;
        ret.reserve( map.size() );
        std::transform( map.begin(), map.end(),
                        std::back_inserter( ret ),
                        select2nd<typename MapT::value_type>() );
        return ret;
    }

    template<typename MapT>
    std::vector<typename MapT::key_type> keys( const MapT& map )
    {
        std::vector<typename MapT::key_type> ret;
        ret.reserve( map.size() );
        std::transform( map.begin(), map.end(),
                        std::back_inserter( ret ),
                        select1st<typename MapT::value_type>() );
        return ret;
    }
}
}

namespace std
{
    // must be in std namespace.
    // STL specialisations for const references : Add others if necessary.
    template <class _Tp>
    struct equal_to< const _Tp& >
        : public binary_function<const _Tp&, const _Tp& ,bool>
    {
        bool operator()(const _Tp& __x, const _Tp& __y) const { return __x == __y; }
    };

    /// One of the @link s20_3_3_comparisons comparison functors@endlink.
    template <class _Tp>
    struct not_equal_to<const _Tp&>
        : public binary_function<const _Tp&, const _Tp&, bool>
    {
        bool operator()(const _Tp& __x, const _Tp& __y) const { return __x != __y; }
    };

    /// One of the @link s20_3_3_comparisons comparison functors@endlink.
    template <class _Tp>
    struct greater<const _Tp&>
        : public binary_function<const _Tp&,const _Tp&,bool>
    {
        bool operator()(const _Tp& __x, const _Tp& __y) const { return __x > __y; }
    };

    /// One of the @link s20_3_3_comparisons comparison functors@endlink.
    template <class _Tp>
    struct less<const _Tp&>
        : public binary_function<const _Tp&,const _Tp&,bool>
    {
        bool operator()(const _Tp& __x, const _Tp& __y) const { return __x < __y; }
    };

    // Ternary functions.
    template<class Arg1T, class Arg2T, class Arg3T, class ResultT >
    struct ternary_function
    {
        typedef ResultT result_type;
        typedef Arg1T first_argument_type;
        typedef Arg2T second_argument_type;
        typedef Arg3T third_argument_type;
    };
}

// STL extensions, some are SGI extensions, others are my own..
namespace RTT
{namespace internal {

  template<typename T>
  struct identity
    : public std::unary_function<T, T>
  {
    const T& operator()( const T& t ) const
      {
        return t;
      }
  };

  // ternary
  template<typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T>
  struct pointer_to_ternary_function
  {
      typedef ResultT (Signature)( Arg1T, Arg2T, Arg3T );

      ResultT (*fun)( Arg1T, Arg2T, Arg3T );

    typedef ResultT result_type;
    typedef Arg1T first_argument_type;
    typedef Arg2T second_argument_type;
    typedef Arg3T third_argument_type;
    pointer_to_ternary_function( ResultT (*f)(Arg1T, Arg2T, Arg3T ) )
      : fun( f )
      {
      }
    ResultT operator()( Arg1T a, Arg2T b, Arg3T c ) const
      {
        return (*fun)( a, b, c );
      }
  };

  template<typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T>
  pointer_to_ternary_function<ResultT, Arg1T, Arg2T, Arg3T>
  ptr_fun( ResultT (*fun)( Arg1T, Arg2T, Arg3T ) )
  {
    return pointer_to_ternary_function<ResultT, Arg1T, Arg2T, Arg3T>( fun );
  }


  // sixary
  template<typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T,
           typename Arg4T, typename Arg5T, typename Arg6T >
  struct pointer_to_sixary_function
  {
    typedef ResultT (Signature)( Arg1T, Arg2T, Arg3T, Arg4T, Arg5T, Arg6T );
    ResultT (*fun)( Arg1T, Arg2T, Arg3T, Arg4T, Arg5T, Arg6T );
    typedef ResultT result_type;
    typedef Arg1T first_argument_type;
    typedef Arg2T second_argument_type;
    typedef Arg3T third_argument_type;
    typedef Arg4T fourth_argument_type;
    typedef Arg5T fifth_argument_type;
    typedef Arg6T sixth_argument_type;
    pointer_to_sixary_function( ResultT (*f)(Arg1T, Arg2T, Arg3T, Arg4T, Arg5T, Arg6T ) )
      : fun( f )
      {
      }
    ResultT operator()( Arg1T a, Arg2T b, Arg3T c, Arg4T d, Arg5T e, Arg6T f ) const
      {
        return (*fun)( a, b, c, d, e, f );
      }
  };

    template<typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T,
           typename Arg4T, typename Arg5T, typename Arg6T >
  pointer_to_sixary_function<ResultT, Arg1T, Arg2T, Arg3T, Arg4T, Arg5T, Arg6T>
  ptr_fun( ResultT (*fun)( Arg1T, Arg2T, Arg3T, Arg4T, Arg5T, Arg6T ) )
  {
    return pointer_to_sixary_function<ResultT, Arg1T, Arg2T, Arg3T, Arg4T, Arg5T, Arg6T>( fun );
  }


#if 0
    /** This does not work with RedHat 8.0 **/
  // the STL lacks a functor multiplying two objects of distinct
  // types.. multiplies<T> requires that a and b are both of type
  // T when calling operator()(a,b).  So I wrote my own replacement.
  // This relies on the GCC typeof C++ extension
  template<typename A, typename B>
  struct multiplies
  {
    typedef typeof( A() * B() ) result_type;
    typedef A first_argument_type;
    typedef B second_argument_type;

    result_type operator()( A a, B b ) const
      {
        return a*b;
      }
  };
  template<typename A, typename B>
  struct divides
  {
    typedef typeof( A() / B() ) result_type;
    typedef A first_argument_type;
    typedef B second_argument_type;

    result_type operator()( A a, B b ) const
      {
        return a/b;
      }
  };
#else
  template<typename R, typename A, typename B>
  struct multiplies3
  {
    typedef R result_type;
    typedef A first_argument_type;
    typedef B second_argument_type;

    result_type operator()( A a, B b ) const
      {
        return a*b;
      }
  };
  template<typename R, typename A, typename B>
  struct divides3
  {
    typedef R result_type;
    typedef A first_argument_type;
    typedef B second_argument_type;

    result_type operator()( A a, B b ) const
      {
        return a/b;
      }
  };
  template<>
  struct divides3<int, int, int>
  {
    typedef int result_type;
    typedef int first_argument_type;
    typedef int second_argument_type;

    result_type operator()( int a, int b ) const
      {
          // integer division by zero will throw a fatal
          // exception, aborting the program (SIGFPE). This is
          // unacceptable, the problem is however that
          // we can not signal an erronous expression in Orocos.
          // we propagate zero instead.
        return b == 0 ? 0 : a/b;
      }
  };
  template<>
  struct divides3<unsigned int, unsigned int, unsigned int>
  {
    typedef unsigned int result_type;
    typedef unsigned int first_argument_type;
    typedef unsigned int second_argument_type;

    result_type operator()( unsigned int a, unsigned int b ) const
      {
          // integer division by zero will throw a fatal
          // exception, aborting the program (SIGFPE). This is
          // unacceptable, the problem is however that
          // we can not signal an erronous expression in Orocos.
          // we propagate zero instead.
        return b == 0 ? 0 : a/b;
      }
  };
  template<>
  struct divides3<long long, long long, long long>
  {
    typedef long long result_type;
    typedef long long first_argument_type;
    typedef long long second_argument_type;

    result_type operator()( long long a, long long b ) const
      {
          // integer division by zero will throw a fatal
          // exception, aborting the program (SIGFPE). This is
          // unacceptable, the problem is however that
          // we can not signal an erronous expression in Orocos.
          // we propagate zero instead.
        return b == 0 ? 0 : a/b;
      }
  };
  template<>
  struct divides3<unsigned long long, unsigned long long, unsigned long long>
  {
    typedef unsigned long long result_type;
    typedef unsigned long long first_argument_type;
    typedef unsigned long long second_argument_type;

    result_type operator()( unsigned long long a, unsigned long long b ) const
      {
          // integer division by zero will throw a fatal
          // exception, aborting the program (SIGFPE). This is
          // unacceptable, the problem is however that
          // we can not signal an erronous expression in Orocos.
          // we propagate zero instead.
        return b == 0 ? 0 : a/b;
      }
  };
  template<typename R, typename A, typename B>
  struct adds3
  {
    typedef R result_type;
    typedef A first_argument_type;
    typedef B second_argument_type;

    result_type operator()( A a, B b ) const
      {
        return a+b;
      }
  };
  template<typename R, typename A, typename B>
  struct subs3
  {
    typedef R result_type;
    typedef A first_argument_type;
    typedef B second_argument_type;

    result_type operator()( A a, B b ) const
      {
        return a-b;
      }
  };
#endif
}} // namespace RTT



#endif
