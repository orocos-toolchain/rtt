#include "execution/Operators.hpp"

#include <functional>

#include <pkgconf/system.h>
#ifdef OROPKG_GEOMETRY
// Include geometry support
#include <geometry/frames.h>
#endif

#include <boost/type_traits.hpp>

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
      };
  };

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
      };
  };
};

namespace ORO_Execution
{
#ifdef OROPKG_GEOMETRY
  using ORO_Geometry::Frame;
  using ORO_Geometry::Vector;
  using ORO_Geometry::Rotation;
#endif

  template<typename function>
  class UnaryOperator
    : public UnaryOp
  {
    typedef typename mystl::remove_cr<typename function::argument_type>::type arg_t;
    typedef typename function::result_type result_t;
    const char* mop;
    function fun;
  public:
    UnaryOperator( const char* op, function f )
      : mop( op ), fun( f )
      {
      };
    DataSource<result_t>* build( const std::string& op, DataSourceBase* a )
      {
        if ( op != mop ) return 0;
        DataSource<arg_t>* arg =
          dynamic_cast<DataSource<arg_t>*>( a );
        if ( ! arg ) return 0;
        return new UnaryDataSource<function>( arg, fun );
      };
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

  template<typename function>
  class BinaryOperator
    : public BinaryOp
  {
    typedef typename mystl::remove_cr<typename function::first_argument_type>::type arg1_t;
    typedef typename mystl::remove_cr<typename function::second_argument_type>::type arg2_t;
    typedef typename function::result_type result_t;
    const char* mop;
    function fun;
  public:
    BinaryOperator( const char* op, function f )
      : mop( op ), fun( f )
      {
      };
    DataSource<result_t>* build( const std::string& op, DataSourceBase* a,
                           DataSourceBase* b )
      {
        if ( op != mop ) return 0;
        DataSource<arg1_t>* arg1 =
          dynamic_cast<DataSource<arg1_t>*>( a );
        DataSource<arg2_t>* arg2 =
          dynamic_cast<DataSource<arg2_t>*>( b );
        if ( !arg1 || ! arg2 ) return 0;
        return new BinaryDataSource<function>( arg1, arg2, fun );
      };
  };

  template<typename function>
  BinaryOperator<function>*
  newBinaryOperator( const char* op, function f )
  {
    return new BinaryOperator<function>( op, f );
  }

  template<typename function>
  class TernaryOperator
    : public TernaryOp
  {
    typedef typename mystl::remove_cr<typename function::first_argument_type>::type arg1_t;
    typedef typename mystl::remove_cr<typename function::second_argument_type>::type arg2_t;
    typedef typename mystl::remove_cr<typename function::third_argument_type>::type arg3_t;
    typedef typename function::result_type result_t;
    const char* mop;
    function fun;
  public:
    TernaryOperator( const char* op, function f )
      : mop( op ), fun( f )
      {
      };
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
      };
  };

  template<typename function>
  TernaryOperator<function>*
  newTernaryOperator( const char* op, function f )
  {
    return new TernaryOperator<function>( op, f );
  }

  OperatorRegistry& OperatorRegistry::instance()
  {
    static OperatorRegistry reg;
    return reg;
  };

#ifdef OROPKG_GEOMETRY
  Frame framevr( const Vector& v, const Rotation& r )
  {
    return Frame( r, v );
  }

  Vector vectorxyz( double a, double b, double c )
  {
    return Vector( a, b, c );
  }
#endif

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


    // strings
    add( newBinaryOperator( "+", std::plus<std::string>() ) );
    add( newBinaryOperator( "==", std::equal_to<std::string>() ) );
    add( newBinaryOperator( "!=", std::not_equal_to<std::string>() ) );
    add( newBinaryOperator( "<", std::less<std::string>() ) );

#ifdef OROPKG_GEOMETRY
    // vectors: I'm simply exporting all those that are available, not
    //          adding any myself..
    add( newUnaryOperator( "-", std::negate<Vector>() ) );
    add( newBinaryOperator( "*", std::multiplies<Vector>() ) );
    add( newBinaryOperator( "+", std::plus<Vector>() ) );
    add( newBinaryOperator( "-", std::minus<Vector>() ) );
    add( newBinaryOperator( "*", mystl::multiplies<int, Vector>() ) );
    add( newBinaryOperator( "*", mystl::multiplies<Vector, int>() ) );
    add( newBinaryOperator( "*", mystl::multiplies<double, Vector>() ) );
    add( newBinaryOperator( "*", mystl::multiplies<Vector, double>() ) );

    // constructors:
    add( newTernaryOperator( "vectorxyz", mystl::ptr_fun( &vectorxyz ) ) );
    add( newTernaryOperator( "rotationeuler",
                             mystl::ptr_fun( Rotation::EulerZYZ ) ) );
    add( newBinaryOperator( "framevr", std::ptr_fun( &framevr ) ) );
#endif
  };

  void OperatorRegistry::add( UnaryOp* a )
  {
    unaryops.push_back( a );
  };

  void OperatorRegistry::add( BinaryOp* b )
  {
    binaryops.push_back( b );
  };

  void OperatorRegistry::add( TernaryOp* b )
  {
    ternaryops.push_back( b );
  };

  OperatorRegistry::~OperatorRegistry()
  {
    mystl::delete_all( unaryops.begin(), unaryops.end() );
    mystl::delete_all( binaryops.begin(), binaryops.end() );
    mystl::delete_all( ternaryops.begin(), ternaryops.end() );
  };

  DataSourceBase* OperatorRegistry::applyUnary(
    const std::string& op, DataSourceBase* a )
  {
    typedef std::vector<UnaryOp*> vec;
    typedef vec::iterator iter;
    for ( iter i = unaryops.begin(); i != unaryops.end(); ++i )
    {
      DataSourceBase* ret = (*i)->build( op, a );
      if ( ret ) return ret;
    };
    return 0;
  };

  DataSourceBase* OperatorRegistry::applyBinary(
    const std::string& op, DataSourceBase* a, DataSourceBase* b )
  {
    typedef std::vector<BinaryOp*> vec;
    typedef vec::iterator iter;
    for ( iter i = binaryops.begin(); i != binaryops.end(); ++i )
    {
      DataSourceBase* ret = (*i)->build( op, a, b );
      if ( ret ) return ret;
    };
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
    };
    return 0;
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
}
