/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  Operators.cxx

                        Operators.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
#include "execution/Operators.hpp"

#include <functional>

#include <pkgconf/system.h>
#ifdef OROPKG_GEOMETRY
// Include geometry support
#include <geometry/frames.h>
#endif

// Cappellini Consonni Extension
#include <corelib/MultiVector.hpp>

#include <corelib/Logger.hpp>
using namespace ORO_CoreLib;

#include <boost/type_traits.hpp>
#include <boost/shared_ptr.hpp>

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
namespace mystl
{

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


  // combines boost::remove_reference and boost::remove_const
  template<typename T>
  struct remove_cr
  {
    typedef typename boost::remove_const<
      typename boost::remove_reference<T>::type>::type type;
  };

  template<typename iter>
  static void delete_all( iter a, iter b )
  {
    for ( ; a < b; a++ )
      delete *a;
  };

#if 0
    /** This does not work with RedHat 8.0 **/
  // the STL lacks a functor multiplying two objects of distinct
  // types.. std::multiplies<T> requires that a and b are both of type
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
  struct multiplies
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
  struct divides
  {
    typedef R result_type;
    typedef A first_argument_type;
    typedef B second_argument_type;

    result_type operator()( A a, B b ) const
      {
        return a/b;
      }
  };
  template<typename R, typename A, typename B>
  struct adds
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
  struct subs
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
};

namespace ORO_Execution
{
#ifdef OROPKG_GEOMETRY
  using ORO_Geometry::Frame;
  using ORO_Geometry::Vector;
  using ORO_Geometry::Rotation;
  using ORO_Geometry::Wrench;
  using ORO_Geometry::Twist;
#endif
  using ORO_CoreLib::Double6D;

    using namespace detail;

  // Unary
  template<typename function>
  class UnaryOperator
    : public UnaryOp
  {
//     typedef typename mystl::remove_cr<typename function::argument_type>::type arg_t;
    typedef typename function::argument_type arg_t;
    typedef typename function::result_type result_t;
    const char* mop;
    function fun;
  public:
    UnaryOperator( const char* op, function f )
      : mop( op ), fun( f )
      {
      }
    DataSource<result_t>* build( const std::string& op, DataSourceBase* a )
      {
        if ( op != mop ) return 0;
        DataSource<arg_t>* arg =
          dynamic_cast<DataSource<arg_t>*>( a );
        if ( ! arg ) return 0;
        return new UnaryDataSource<function>( arg, fun );
      }
  };

  /**
   * helper function for the use of UnaryOperator
   */
  template<typename function>
  UnaryOperator<function>*
  newUnaryOperator( const char* op, function f )
  {
    return new UnaryOperator<function>( op, f );
  }


  // Binary
  template<typename function>
  class BinaryOperator
    : public BinaryOp
  {
    typedef typename function::first_argument_type arg1_t;
    typedef typename function::second_argument_type arg2_t;
//     typedef typename mystl::remove_cr<typename function::first_argument_type>::type arg1_t;
//     typedef typename mystl::remove_cr<typename function::second_argument_type>::type arg2_t;
    typedef typename function::result_type result_t;
    const char* mop;
    function fun;
  public:
    BinaryOperator( const char* op, function f )
      : mop( op ), fun( f )
      {
      }
    DataSource<result_t>* build( const std::string& op, DataSourceBase* a,
                           DataSourceBase* b )
      {
        if ( op != mop ) return 0;
//         Logger::log() << Logger::Debug << "BinaryOperator: "<< op << Logger::nl;
        DataSource<arg1_t>* arg1 =
          dynamic_cast<DataSource<arg1_t>*>( a );
        DataSource<arg2_t>* arg2 =
          dynamic_cast<DataSource<arg2_t>*>( b );
//         Logger::log() << "arg1 : "<< arg1 <<" second arg: "<<arg2<<"..." << Logger::endl;
//         Logger::log() << "arg1 was: "<< typeid(arg1).name()  <<" a was: "<<typeid(a).name()<<"..." << Logger::endl;
        if ( !arg1 || ! arg2 ) return 0;
//         Logger::log() << "success !"<< Logger::endl;
        return new BinaryDataSource<function>( arg1, arg2, fun );
      }
  };

  template<typename function>
  BinaryOperator<function>*
  newBinaryOperator( const char* op, function f )
  {
    return new BinaryOperator<function>( op, f );
  }


  // Ternary
  template<typename function>
  class TernaryOperator
    : public TernaryOp
  {
    typedef typename function::first_argument_type arg1_t;
    typedef typename function::second_argument_type arg2_t;
    typedef typename function::third_argument_type arg3_t;
//     typedef typename mystl::remove_cr<typename function::first_argument_type>::type arg1_t;
//     typedef typename mystl::remove_cr<typename function::second_argument_type>::type arg2_t;
//     typedef typename mystl::remove_cr<typename function::third_argument_type>::type arg3_t;
    typedef typename function::result_type result_t;
    const char* mop;
    function fun;
  public:
    TernaryOperator( const char* op, function f )
      : mop( op ), fun( f )
      {
      }
    DataSource<result_t>* build( const std::string& op, DataSourceBase* a,
                                 DataSourceBase* b, DataSourceBase* c )
      {
        if ( op != mop ) return 0;
        DataSource<arg1_t>* arg1 =
          dynamic_cast<DataSource<arg1_t>*>( a );
        DataSource<arg2_t>* arg2 =
          dynamic_cast<DataSource<arg2_t>*>( b );
        DataSource<arg3_t>* arg3 =
          dynamic_cast<DataSource<arg3_t>*>( c );
        if ( !arg1 || ! arg2 || !arg3 ) return 0;
        return new TernaryDataSource<function>( arg1, arg2, arg3, fun );
      }
  };

  template<typename function>
  TernaryOperator<function>*
  newTernaryOperator( const char* op, function f )
  {
    return new TernaryOperator<function>( op, f );
  }



  // sixary
  template<typename function>
  class SixaryOperator
    : public SixaryOp
  {
    typedef typename function::first_argument_type arg1_t;
    typedef typename function::second_argument_type arg2_t;
    typedef typename function::third_argument_type arg3_t;
    typedef typename function::fourth_argument_type arg4_t;
    typedef typename function::fifth_argument_type arg5_t;
    typedef typename function::sixth_argument_type arg6_t;
//     typedef typename mystl::remove_cr<typename function::first_argument_type>::type arg1_t;
//     typedef typename mystl::remove_cr<typename function::second_argument_type>::type arg2_t;
//     typedef typename mystl::remove_cr<typename function::third_argument_type>::type arg3_t;
//     typedef typename mystl::remove_cr<typename function::forth_argument_type>::type arg4_t;
//     typedef typename mystl::remove_cr<typename function::fifth_argument_type>::type arg5_t;
//     typedef typename mystl::remove_cr<typename function::sixth_argument_type>::type arg6_t;
    typedef typename function::result_type result_t;
    const char* mop;
    function fun;
  public:
    SixaryOperator( const char* op, function f )
      : mop( op ), fun( f )
      {
      }
    DataSource<result_t>* build( const std::string& op,
                                 DataSourceBase* a, DataSourceBase* b, DataSourceBase* c,
                                 DataSourceBase* d, DataSourceBase* e, DataSourceBase* f )
      {
        if ( op != mop ) return 0;
        DataSource<arg1_t>* arg1 =
          dynamic_cast<DataSource<arg1_t>*>( a );
        DataSource<arg2_t>* arg2 =
          dynamic_cast<DataSource<arg2_t>*>( b );
        DataSource<arg3_t>* arg3 =
          dynamic_cast<DataSource<arg3_t>*>( c );
        DataSource<arg4_t>* arg4 =
          dynamic_cast<DataSource<arg4_t>*>( d );
        DataSource<arg5_t>* arg5 =
          dynamic_cast<DataSource<arg5_t>*>( e );
        DataSource<arg6_t>* arg6 =
          dynamic_cast<DataSource<arg6_t>*>( f );
        if ( !arg1 || ! arg2 || !arg3 || !arg4 || !arg5 || !arg6 ) return 0;
        return new SixaryDataSource<function>( arg1, arg2, arg3, arg4, arg5, arg6, fun );
      }
  };

  template<typename function>
  SixaryOperator<function>*
  newSixaryOperator( const char* op, function f )
  {
    return new SixaryOperator<function>( op, f );
  }

    /**
     *  Dot : '.' for member access of composite values
     */
  template<typename function>
  class DotOperator
    : public DotOp
  {
    typedef typename function::argument_type arg1_t;
    typedef typename function::result_type result_t;
    const char* memb;
    function fun;
  public:
      /**
       * Create a Dot '.' accessor to a composite parser type.
       */
    DotOperator( const char* m, function f )
      : memb( m ), fun( f )
      {
      }
      DataSource<result_t>* build( const std::string& member, DataSourceBase* a)
      {
        if ( member != memb ) return 0;
//         Logger::log() << Logger::Debug << "DotOperator: "<< op << Logger::nl;
        DataSource<arg1_t>* arg1 =
          dynamic_cast<DataSource<arg1_t>*>( a );
        if ( !arg1 ) return 0;
//         Logger::log() << "success !"<< Logger::endl;
        return new UnaryDataSource<function>( arg1, fun );
      }
  };

  template<typename function>
  DotOperator<function>*
  newDotOperator( const char* member, function f )
  {
    return new DotOperator<function>( member, f );
  }



  OperatorRegistry& OperatorRegistry::instance()
  {
    static OperatorRegistry reg;
    return reg;
  }

#ifdef OROPKG_GEOMETRY
    struct framevr
        : public std::binary_function<Vector,Rotation, Frame>
    {
        Frame operator()( const Vector& v, const Rotation& r ) const
        {
            return Frame( r, v );
        }
    };

    struct wrenchft
        : public std::binary_function<Vector,Vector,Wrench>
    {
        Wrench operator()( const Vector& force, const Vector& torque ) const
        {
            return Wrench( force, torque );
        }
    };

    struct twistvw
        : public std::binary_function<Vector,Vector,Twist>
    {
        Twist operator()( const Vector& trans, const Vector& rot ) const
        {
            return Twist( trans, rot );
        }
    };

    struct vectorxyz
        : public std::ternary_function<double,double,double,Vector>
    {
        Vector operator()( double a, double b, double c ) const
        {
            return Vector( a, b, c );
        }
    };

  
    template<class WT>
    struct wrenchtwist_index
        : public std::binary_function<WT,int,double>
    {
        double operator()( WT& w, int index) const
        {
            if (index > 5 || index <0)
                return 0.0;
            else
                return w[index];
        }
    };
    /*
  double twist_index( Twist& t, int index)
  {
    if (index > 5 || index <0)
      return 0.0;
    else
      return t[index];
      }
*/


    struct vector_index
        : public std::binary_function<Vector, int, double>
    {
        double operator()( Vector& v, int index ) const
            {
                if (index > 2 || index <0)
                    return 0.0;
                else
                    return v[index];
            }
    };

    template< int I>
    struct vector_dot
        : public std::unary_function<Vector, double>
    {
        double operator()(const Vector& v ) const
            {
                return v[I];
            }
    };

    struct twist_rot
        : public std::unary_function<Twist, Vector>
    {
        Vector operator()(const Twist& t ) const
            {
                return t.rot;
            }
    };

    struct twist_vel
        : public std::unary_function<Twist, Vector>
    {
        Vector operator()(const Twist& t ) const
            {
                return t.vel;
            }
    };

    struct wrench_torque
        : public std::unary_function<Wrench, Vector>
    {
        Vector operator()(const Wrench& t ) const
            {
                return t.torque;
            }
    };

    struct wrench_force
        : public std::unary_function<Wrench, Vector>
    {
        Vector operator()(const Wrench& t ) const
            {
                return t.force;
            }
    };

    struct frame_pos
        : public std::unary_function<Frame, Vector>
    {
        Vector operator()(const Frame& f ) const
            {
                return f.p;
            }
    };

    struct frame_rot
        : public std::unary_function<Frame, Rotation>
    {
        Rotation operator()(const Frame& f ) const
            {
                return f.M;
            }
    };

    struct rotation_roll
        : public std::unary_function<Rotation, double>
    {
        double operator()(const Rotation& rot ) const
            {
                double r,p,y;
                rot.GetRPY(r,p,y);
                return r;
            }
    };

    struct rotation_pitch
        : public std::unary_function<Rotation, double>
    {
        double operator()(const Rotation& rot ) const
            {
                double r,p,y;
                rot.GetRPY(r,p,y);
                return p;
            }
    };

    struct rotation_yaw
        : public std::unary_function<Rotation, double>
    {
        double operator()(const Rotation& rot ) const
            {
                double r,p,y;
                rot.GetRPY(r,p,y);
                return y;
            }
    };

#endif

  Double6D double6Dd( double d )
  {
    Double6D d6d;
    for (int i=0; i<6; i++)
      d6d[i] = d;
    return d6d;
  }

  Double6D double6D6d( double a, double b, double c, double d, double e, double f )
  {
    Double6D d6d;
    d6d[0] = a;
    d6d[1] = b;
    d6d[2] = c;
    d6d[3] = d;
    d6d[4] = e;
    d6d[5] = f;
    return d6d;
  }

    struct array_ctor
        : public std::unary_function<int, const std::vector<double>&>
    {
        mutable boost::shared_ptr< std::vector<double> > ptr;
        array_ctor()
            : ptr( new std::vector<double>() ) {}
        const std::vector<double>& operator()( int size ) const
        {
            ptr->resize( size );
            return *(ptr);
        }
    };

    struct string_index
        : public std::binary_function<const std::string&, int, char>
    {
        char operator()(const std::string& s, int index) const
        {
            if ( index >= (int)(s.length()) || index < 0)
                return 0;
            return s[index];
        }
    };
                    

  double double6D_index( Double6D d6,  int index )
  {
      if ( index > 5 || index < 0 )
          return 0.0;
      return d6[index];
  }

    struct array_index
        : public std::binary_function<const std::vector<double>&, int, double>
    {
        double operator()(const std::vector<double>& v, int index) const
        {
            if ( index >= (int)(v.size()) || index < 0)
                return 0;
            return v[index];
        }
    };
                    
  OperatorRegistry::OperatorRegistry()
  {
    // boolean stuff:
    add( newUnaryOperator( "!", std::logical_not<bool>() ) );
    add( newBinaryOperator( "&&", std::logical_and<bool>() ) );
    add( newBinaryOperator( "||", std::logical_or<bool>() ) );
    add( newBinaryOperator( "==", std::equal_to<bool>() ) );
    add( newBinaryOperator( "!=", std::not_equal_to<bool>() ) );

    // int stuff
    add( newUnaryOperator( "-", std::negate<int>() ) );
    add( newUnaryOperator( "+", mystl::identity<int>() ) );
    add( newBinaryOperator( "*", std::multiplies<int>() ) );
    add( newBinaryOperator( "/", std::divides<int>() ) );
    add( newBinaryOperator( "%", std::modulus<int>() ) );
    add( newBinaryOperator( "+", std::plus<int>() ) );
    add( newBinaryOperator( "-", std::minus<int>() ) );
    add( newBinaryOperator( "<", std::less<int>() ) );
    add( newBinaryOperator( "<=", std::less_equal<int>() ) );
    add( newBinaryOperator( ">", std::greater<int>() ) );
    add( newBinaryOperator( ">=", std::greater_equal<int>() ) );
    add( newBinaryOperator( "==", std::equal_to<int>() ) );
    add( newBinaryOperator( "!=", std::not_equal_to<int>() ) );


    // double stuff..
    add( newUnaryOperator( "-", std::negate<double>() ) );
    add( newUnaryOperator( "+", mystl::identity<double>() ) );
    add( newBinaryOperator( "*", std::multiplies<double>() ) );
    add( newBinaryOperator( "/", std::divides<double>() ) );
    add( newBinaryOperator( "+", std::plus<double>() ) );
    add( newBinaryOperator( "-", std::minus<double>() ) );
    add( newBinaryOperator( "<", std::less<double>() ) );
    add( newBinaryOperator( "<=", std::less_equal<double>() ) );
    add( newBinaryOperator( ">", std::greater<double>() ) );
    add( newBinaryOperator( ">=", std::greater_equal<double>() ) );
    add( newBinaryOperator( "==", std::equal_to<double>() ) );
    add( newBinaryOperator( "!=", std::not_equal_to<double>() ) );
    // with integers :
    add( newBinaryOperator( "*", mystl::multiplies<double,int, double>() ) );
    add( newBinaryOperator( "*", mystl::multiplies<double,double, int>() ) );
    add( newBinaryOperator( "/", mystl::divides<double,int, double>() ) );
    add( newBinaryOperator( "/", mystl::divides<double,double, int>() ) );
    add( newBinaryOperator( "+", mystl::adds<double,int, double>() ) );
    add( newBinaryOperator( "+", mystl::adds<double,double, int>() ) );
    add( newBinaryOperator( "-", mystl::subs<double,int, double>() ) );
    add( newBinaryOperator( "-", mystl::subs<double,double, int>() ) );


    // strings
//  add( newBinaryOperator( "+", std::plus<std::string>() ) );
    add( newBinaryOperator( "==", std::equal_to<const std::string&>() ) );
    add( newBinaryOperator( "!=", std::not_equal_to< const std::string&>() ) );
    add( newBinaryOperator( "<", std::less<const std::string&>() ) );
    add( newBinaryOperator( ">", std::greater<const std::string&>() ) );
    add( newBinaryOperator( "[]", string_index() ) );

    // chars
    add( newBinaryOperator( "==", std::equal_to<char>() ) );
    add( newBinaryOperator( "!=", std::not_equal_to<char>() ) );
    add( newBinaryOperator( "<", std::less<char>() ) );

#ifdef OROPKG_GEOMETRY
    // vectors: I'm simply exporting all those that are available, not
    //          adding any myself..
    add( newUnaryOperator( "-", std::negate<Vector>() ) );
    add( newBinaryOperator( "*", std::multiplies<Vector>() ) );
    add( newBinaryOperator( "*", std::multiplies<Frame>() ) );
    add( newBinaryOperator( "*", std::multiplies<Rotation>() ) );
    add( newBinaryOperator( "+", std::plus<Vector>() ) );
    add( newBinaryOperator( "-", std::minus<Vector>() ) );
    add( newBinaryOperator( "+", std::plus<Wrench>() ) );
    add( newBinaryOperator( "-", std::minus<Wrench>() ) );
    add( newBinaryOperator( "+", std::plus<Twist>() ) );
    add( newBinaryOperator( "-", std::minus<Twist>() ) );
    add( newBinaryOperator( "*", mystl::multiplies<Vector,int, Vector>() ) );
    add( newBinaryOperator( "*", mystl::multiplies<Vector,Vector, int>() ) );
    add( newBinaryOperator( "*", mystl::multiplies<Vector,double, Vector>() ) );
    add( newBinaryOperator( "*", mystl::multiplies<Vector,Vector, double>() ) );
    add (newBinaryOperator( "*", mystl::multiplies<Wrench, Frame, Wrench>() ) );
    add (newBinaryOperator( "*", mystl::multiplies<Twist, Frame, Twist>() ) );
    add (newBinaryOperator( "*", mystl::multiplies<Vector, Frame, Vector>() ) );

    add( newBinaryOperator( "[]", vector_index() ) );
    add( newBinaryOperator( "[]", wrenchtwist_index<Wrench>() ) );
    add( newBinaryOperator( "[]", wrenchtwist_index<Twist>() ) );

    // access to members of composite types, use the 'dot' operator
    add( newDotOperator( "x", vector_dot<0>() ) );
    add( newDotOperator( "y", vector_dot<1>() ) );
    add( newDotOperator( "z", vector_dot<2>() ) );
    add( newDotOperator( "rot", twist_rot() ) );
    add( newDotOperator( "vel", twist_vel() ) );
    add( newDotOperator( "force", wrench_torque() ) );
    add( newDotOperator( "torque", wrench_force() ) );
    add( newDotOperator( "r", rotation_roll() ) );
    add( newDotOperator( "p", rotation_pitch() ) );
    add( newDotOperator( "y", rotation_yaw() ) );
    add( newDotOperator( "p", frame_pos() ) );
    add( newDotOperator( "M", frame_rot() ) );

    add( newBinaryOperator( "[]", wrenchtwist_index<Wrench>() ) );
    add( newBinaryOperator( "[]", wrenchtwist_index<Twist>() ) );

    // constructors:
    add( newTernaryOperator( "vectorxyz", vectorxyz() ));
    add( newTernaryOperator( "rotationRPY", mystl::ptr_fun( Rotation::RPY ) ) );
    add( newBinaryOperator( "framevr", framevr() ) );
    add( newBinaryOperator( "wrenchft", wrenchft( ) ) );
    add( newBinaryOperator( "twistvw", twistvw () ) );
#endif
    add( newUnaryOperator( "double6Dd", std::ptr_fun( &double6Dd ) ) );
    add( newSixaryOperator( "double6D6d", mystl::ptr_fun( &double6D6d ) ) );
    add( newUnaryOperator( "array", array_ctor() ) );

    add( newBinaryOperator( "==", std::equal_to<Double6D>() ) );
    add( newBinaryOperator( "!=", std::not_equal_to<Double6D>() ) );
    add( newUnaryOperator( "-", std::negate<Double6D>() ) );
    add( newBinaryOperator( "*", std::multiplies<Double6D>() ) );
    add( newBinaryOperator( "+", std::plus<Double6D>() ) );
    add( newBinaryOperator( "-", std::minus<Double6D>() ) );
    add( newBinaryOperator( "*", mystl::multiplies<Double6D, double, Double6D>() ) );
    add( newBinaryOperator( "*", mystl::multiplies<Double6D, Double6D, double>() ) );
    add( newBinaryOperator( "*", mystl::divides<Double6D, Double6D, double>() ) );
    add( newBinaryOperator( "[]", std::ptr_fun( &double6D_index ) ) );
    add( newBinaryOperator( "[]", array_index() ) );
  }

  void OperatorRegistry::add( DotOp* a )
  {
    dotops.push_back( a );
  }

  void OperatorRegistry::add( UnaryOp* a )
  {
    unaryops.push_back( a );
  }

  void OperatorRegistry::add( BinaryOp* b )
  {
    binaryops.push_back( b );
  }

  void OperatorRegistry::add( TernaryOp* b )
  {
    ternaryops.push_back( b );
  }

  void OperatorRegistry::add( SixaryOp* b )
  {
    sixaryops.push_back( b );
  }

  OperatorRegistry::~OperatorRegistry()
  {
    mystl::delete_all( unaryops.begin(), unaryops.end() );
    mystl::delete_all( binaryops.begin(), binaryops.end() );
    mystl::delete_all( ternaryops.begin(), ternaryops.end() );
    mystl::delete_all( sixaryops.begin(), sixaryops.end() ); 
 }

  DataSourceBase* OperatorRegistry::applyDot(
    const std::string& mem, DataSourceBase* a )
  {
    typedef std::vector<DotOp*> vec;
    typedef vec::iterator iter;
    for ( iter i = dotops.begin(); i != dotops.end(); ++i )
    {
      DataSourceBase* ret = (*i)->build( mem, a );
      if ( ret ) return ret;
    }
    return 0;
  }

  DataSourceBase* OperatorRegistry::applyUnary(
    const std::string& op, DataSourceBase* a )
  {
    typedef std::vector<UnaryOp*> vec;
    typedef vec::iterator iter;
    for ( iter i = unaryops.begin(); i != unaryops.end(); ++i )
    {
      DataSourceBase* ret = (*i)->build( op, a );
      if ( ret ) return ret;
    }
    return 0;
  }

  DataSourceBase* OperatorRegistry::applyBinary(
    const std::string& op, DataSourceBase* a, DataSourceBase* b )
  {
    typedef std::vector<BinaryOp*> vec;
    typedef vec::iterator iter;
    for ( iter i = binaryops.begin(); i != binaryops.end(); ++i )
    {
      DataSourceBase* ret = (*i)->build( op, a, b );
      if ( ret ) return ret;
    }
    return 0;
  }

  DataSourceBase* OperatorRegistry::applyTernary(
    const std::string& op, DataSourceBase* a, DataSourceBase* b,
    DataSourceBase* c )
  {
    typedef std::vector<TernaryOp*> vec;
    typedef vec::iterator iter;
    for ( iter i = ternaryops.begin(); i != ternaryops.end(); ++i )
    {
      DataSourceBase* ret = (*i)->build( op, a, b, c );
      if ( ret ) return ret;
    }
    return 0;
  }

  DataSourceBase* OperatorRegistry::applySixary(
    const std::string& op,
    DataSourceBase* a, DataSourceBase* b,
    DataSourceBase* c, DataSourceBase* d,
    DataSourceBase* e, DataSourceBase* f )
  {
    typedef std::vector<SixaryOp*> vec;
    typedef vec::iterator iter;
    for ( iter i = sixaryops.begin(); i != sixaryops.end(); ++i )
    {
      DataSourceBase* ret = (*i)->build( op, a, b, c, d, e, f );
      if ( ret ) return ret;
    }
    return 0;
  }

  DotOp::~DotOp()
  {
  }

  UnaryOp::~UnaryOp()
  {
  }

  BinaryOp::~BinaryOp()
  {
  }

  TernaryOp::~TernaryOp()
  {
  }

  SixaryOp::~SixaryOp()
  {
  }
}
