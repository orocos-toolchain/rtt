
#include "DataSource.hpp"
//#include "DataSourceFactoryInterface.hpp"
#include "TemplateFactory.hpp"

#include <boost/bind.hpp>
#include <boost/type_traits.hpp>

#ifndef MEMBER
#error "Do not include this file directly. Use TemplateDataSourceFactory.hpp or TemplateMethodFactory.hpp"
#endif

/**
 * @file This file contains the TemplateMemberFactory template,
 * which is a template designed to reduce the amount of boilerplate
 * code necessary to add a DataSourceFactory and MethodFactory to an OROCOS component.
 *
 * This is an internal file, not to be used directly by users.
 */
namespace ORO_Execution
{
#ifndef NO_DOXYGEN
#ifndef FUNCTORDATASOURCES
#define FUNCTORDATASOURCES
    /**
     * These classes are generic DataSources that take a
     * functor, and a number of DataSources corresponding with the
     * arguments of the functor, and use it to get data from..
     * @{
     */
    template<typename FunctorT>
    class FunctorDataSource0
      : public DataSource<
      typename boost::remove_const<typename boost::remove_reference<
      typename FunctorT::result_type>::type>::type >
    {
      FunctorT gen;
      typedef typename boost::remove_const<typename boost::remove_reference<
        typename FunctorT::result_type>::type>::type value_t;
    public:
      FunctorDataSource0( FunctorT g )
        : gen( g )
        {
        };

      value_t get() const
        {
          return gen();
        }

        virtual DataSource<value_t>* clone() const
        {
            return new FunctorDataSource0( gen );
        }
      virtual DataSource<value_t>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
        {
          return new FunctorDataSource0<FunctorT>( gen );
        }
    };

  template<typename FunctorT, typename Arg1T>
  class FunctorDataSource1
    : public DataSource<
    typename boost::remove_const<typename boost::remove_reference<
    typename FunctorT::result_type>::type>::type >
  {
    FunctorT gen;
    typedef typename boost::remove_const<typename boost::remove_reference<
      typename FunctorT::result_type>::type>::type value_t;
    typename DataSource<Arg1T>::shared_ptr arg1;
  public:
    FunctorDataSource1( FunctorT g, DataSource<Arg1T>* a1 )
      : gen( g ), arg1( a1 )
      {
      };

    value_t get() const
      {
        Arg1T a = arg1->get();
        return gen( a );
      }

    virtual DataSource<value_t>* clone() const
      {
        return new FunctorDataSource1<FunctorT, Arg1T>( gen, arg1.get() );
      }
    virtual DataSource<value_t>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorDataSource1<FunctorT, Arg1T>( gen, arg1->copy( alreadyCloned ) );
      }
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T>
  class FunctorDataSource2
    : public DataSource<
    typename boost::remove_const<typename boost::remove_reference<
    typename FunctorT::result_type>::type>::type >
  {
    FunctorT gen;
    typedef typename boost::remove_const<typename boost::remove_reference<
      typename FunctorT::result_type>::type>::type value_t;
    typename DataSource<Arg1T>::shared_ptr arg1;
    typename DataSource<Arg2T>::shared_ptr arg2;
  public:
    FunctorDataSource2( FunctorT g, DataSource<Arg1T>* a1, DataSource<Arg2T>* a2 )
        : gen( g ), arg1( a1 ), arg2(a2)
      {
      };

    value_t get() const
      {
        Arg1T a_1 = arg1->get();
        Arg2T a_2 = arg2->get();
        return gen( a_1, a_2 );
      }

    virtual DataSource<value_t>* clone() const
      {
        return new FunctorDataSource2<FunctorT, Arg1T, Arg2T>( gen, arg1.get(), arg2.get() );
      }
    virtual DataSource<value_t>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorDataSource2<FunctorT, Arg1T, Arg2T>( gen, arg1->copy( alreadyCloned ), arg2->copy( alreadyCloned) );
      }
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T>
  class FunctorDataSource3
    : public DataSource<
    typename boost::remove_const<typename boost::remove_reference<
    typename FunctorT::result_type>::type>::type >
  {
    FunctorT gen;
    typedef typename boost::remove_const<typename boost::remove_reference<
      typename FunctorT::result_type>::type>::type value_t;
    typename DataSource<Arg1T>::shared_ptr arg1;
    typename DataSource<Arg2T>::shared_ptr arg2;
    typename DataSource<Arg3T>::shared_ptr arg3;
  public:
    FunctorDataSource3( FunctorT g, DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3 )
        : gen( g ), arg1( a1 ), arg2(a2), arg3(a3)
      {
      };

    value_t get() const
      {
        Arg1T a_1 = arg1->get();
        Arg2T a_2 = arg2->get();
        Arg3T a_3 = arg3->get();
        return gen( a_1, a_2, a_3 );
      }

    virtual DataSource<value_t>* clone() const
      {
        return new FunctorDataSource3<FunctorT, Arg1T, Arg2T, Arg3T>( gen, arg1.get(), arg2.get(), arg3.get() );
      }
    virtual DataSource<value_t>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorDataSource3<FunctorT, Arg1T, Arg2T, Arg3T>( gen, arg1->copy( alreadyCloned ), arg2->copy( alreadyCloned), arg3->copy( alreadyCloned) );
      }
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
  class FunctorDataSource4
    : public DataSource<
    typename boost::remove_const<typename boost::remove_reference<
    typename FunctorT::result_type>::type>::type >
  {
    FunctorT gen;
    typedef typename boost::remove_const<typename boost::remove_reference<
      typename FunctorT::result_type>::type>::type value_t;
    typename DataSource<Arg1T>::shared_ptr arg1;
    typename DataSource<Arg2T>::shared_ptr arg2;
    typename DataSource<Arg3T>::shared_ptr arg3;
    typename DataSource<Arg4T>::shared_ptr arg4;
  public:
    FunctorDataSource4( FunctorT g, DataSource<Arg1T>* a1, DataSource<Arg2T>* a2,
                        DataSource<Arg3T>* a3, DataSource<Arg4T>* a4 )
        : gen( g ), arg1( a1 ), arg2(a2), arg3(a3), arg4(a4)
      {
      };

    value_t get() const
      {
        Arg1T a_1 = arg1->get();
        Arg2T a_2 = arg2->get();
        Arg3T a_3 = arg3->get();
        Arg4T a_4 = arg4->get();
        return gen( a_1, a_2, a_3, a_4 );
      }

    virtual DataSource<value_t>* clone() const
      {
        return new FunctorDataSource4<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T>( gen, arg1.get(), arg2.get(),
                                                                             arg3.get(), arg4.get() );
      }
    virtual DataSource<value_t>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorDataSource4<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T>( gen, arg1->copy( alreadyCloned ), arg2->copy( alreadyCloned), arg3->copy( alreadyCloned), arg4->copy( alreadyCloned) );
      }
  };

  /**
   * @}
   */

  /**
   * @internal Helper functions for the use of FunctorDataSource.  This
   * is provided because for classes, C++ compilers never deduce
   * template parameters, only for functions.  This is a common
   * paradigm, std::back_inserter does the same thing..
   * @{
   */
  template<typename FunctorT>
  DataSource<
    typename boost::remove_const<typename boost::remove_reference<
    typename FunctorT::result_type>::type>::type>*
  newFunctorDataSource( FunctorT g )
  {
    return new FunctorDataSource0<FunctorT>( g );
  };

  template<typename FunctorT, typename Arg1T>
  DataSource<
    typename boost::remove_const<typename boost::remove_reference<
    typename FunctorT::result_type>::type>::type>*
  newFunctorDataSource( FunctorT g, DataSource<Arg1T>* a )
  {
    return new FunctorDataSource1<FunctorT, Arg1T>( g, a );
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T>
  DataSource<
    typename boost::remove_const<typename boost::remove_reference<
    typename FunctorT::result_type>::type>::type>*
  newFunctorDataSource( FunctorT g, DataSource<Arg1T>* a, DataSource<Arg2T>* a2 )
  {
    return new FunctorDataSource2<FunctorT, Arg1T, Arg2T>( g, a, a2 );
  };
  /**
   * @}
   */

  /**
   * @internal These are the classes responsible for the creation of
   * FunctorDataSource's from component member functions.  They are
   * implemented as functors themselves, because that's how we will be
   * using them..
   * TODO: add more overloads to support a larger number of arguments..
   * @{
   */
  template<typename ComponentT, typename ResultT,
           typename FunctorT>
  class FunctorDataSourceGenerator0
  {
    FunctorT fun;
  public:
    FunctorDataSourceGenerator0( FunctorT f )
      : fun( f )
      {
      };
    DataSource<ResultT>* operator()( ComponentT* c ) const
      {
        return newFunctorDataSource( boost::bind( fun, c ) );
      };
  };

  template<typename ComponentT, typename ResultT, typename Arg1T,
           typename FunctorT>
  class FunctorDataSourceGenerator1
  {
    FunctorT fun;
  public:
    FunctorDataSourceGenerator1( FunctorT f )
      : fun( f )
      {
      };
    DataSource<ResultT>* operator()( ComponentT* c, Arg1T a ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, a ) );
      };
    DataSource<ResultT>* operator()(
      ComponentT* c, DataSource<Arg1T>* a ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, _1 ), a );
      };
  };

    template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T,
           typename FunctorT>
  class FunctorDataSourceGenerator2
  {
    FunctorT fun;
  public:
    FunctorDataSourceGenerator2( FunctorT f )
      : fun( f )
      {
      };
    DataSource<ResultT>* operator()( ComponentT* c, Arg1T a, Arg2T a2 ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, a, a2 ) );
      };
    DataSource<ResultT>* operator()(
      ComponentT* c, DataSource<Arg1T>* a, DataSource<Arg2T>* a2 ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, _1, _2 ), a, a2 );
      };
  };

    template<typename ComponentT, typename ResultT,
             typename Arg1T, typename Arg2T,
             typename Arg3T,
           typename FunctorT>
  class FunctorDataSourceGenerator3
  {
    FunctorT fun;
  public:
    FunctorDataSourceGenerator3( FunctorT f )
      : fun( f )
      {
      };
    DataSource<ResultT>* operator()( ComponentT* c, Arg1T a, Arg2T a2, Arg3T a3 ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, a, a2, a3) );
      };
    DataSource<ResultT>* operator()(
      ComponentT* c, DataSource<Arg1T>* a, DataSource<Arg2T>* a2
      , DataSource<Arg3T>* a3 ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, _1, _2, _3 ), a, a2, a3);
      };
  };

    template<typename ComponentT, typename ResultT,
             typename Arg1T, typename Arg2T,
             typename Arg3T, typename Arg4T,
           typename FunctorT>
  class FunctorDataSourceGenerator4
  {
    FunctorT fun;
  public:
    FunctorDataSourceGenerator4( FunctorT f )
      : fun( f )
      {
      };
    DataSource<ResultT>* operator()( ComponentT* c, Arg1T a, Arg2T a2, Arg3T a3, Arg4T a4 ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, a, a2, a3, a4 ) );
      };
    DataSource<ResultT>* operator()(
      ComponentT* c, DataSource<Arg1T>* a, DataSource<Arg2T>* a2
      , DataSource<Arg3T>* a3, DataSource<Arg4T>* a4 ) const
      {
        return newFunctorDataSource( boost::bind( fun, c, _1, _2, _3, _4 ), a, a2, a3, a4 );
      };
  };
  /**
   * @}
   */

  /**
   * @internal FunctorDataSourceGenerator helper functions to let the
   * compiler do the type deducing for us..
   * @{
   */
  template<typename ComponentT, typename ResultT, typename FunctorT>
  FunctorDataSourceGenerator0<ComponentT, ResultT, FunctorT>
  fun_datasource_gen( FunctorT fun )
  {
    return FunctorDataSourceGenerator0<ComponentT, ResultT, FunctorT>( fun );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T,
           typename FunctorT>
  FunctorDataSourceGenerator1<ComponentT, ResultT, Arg1T, FunctorT>
  fun_datasource_gen( FunctorT fun )
  {
    return FunctorDataSourceGenerator1<
      ComponentT, ResultT, Arg1T, FunctorT>( fun );
  };

    template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T,
           typename FunctorT>
  FunctorDataSourceGenerator2<ComponentT, ResultT, Arg1T, Arg2T, FunctorT>
  fun_datasource_gen( FunctorT fun )
  {
    return FunctorDataSourceGenerator2<
      ComponentT, ResultT, Arg1T, Arg2T, FunctorT>( fun );
  };

    template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T,
           typename FunctorT>
  FunctorDataSourceGenerator3<ComponentT, ResultT, Arg1T, Arg2T, Arg3T, FunctorT>
  fun_datasource_gen( FunctorT fun )
  {
    return FunctorDataSourceGenerator3<
      ComponentT, ResultT, Arg1T, Arg2T, Arg3T, FunctorT>( fun );
  };
    template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T,
           typename FunctorT>
  FunctorDataSourceGenerator4<ComponentT, ResultT, Arg1T, Arg2T, Arg3T, Arg4T, FunctorT>
  fun_datasource_gen( FunctorT fun )
  {
    return FunctorDataSourceGenerator4<
      ComponentT, ResultT, Arg1T, Arg2T, Arg3T, Arg4T, FunctorT>( fun );
  };
  /**
   * @}
   */

#endif // FUNCTORDATASOURCES

  /**
   * This is the DataSource factory class that is generated
   * by the newDataSourceFactory function..  Use the helper function
   * newDataSourceFactory.
   */
  template<typename ComponentT>
  class TEMPLATE_FACTORY_NAME
    : public FACTORY_INTERFACE,
      private TemplateFactory<ComponentT, DataSourceBase*>
  {
    typedef TemplateFactory<ComponentT, DataSourceBase*> _TF;
  public:
    TEMPLATE_FACTORY_NAME( ComponentT* c )
      : _TF( c )
      {
      };

    std::vector<std::string> dataNames() const
      {
        return _TF::getNames();
      };

    bool hasData( const std::string& name ) const
      {
        return _TF::hasName( name );
      };

    std::string getDescription( const std::string& method ) const
      {
        return _TF::getDescription( method );
      };

    PropertyBag getArgumentSpec( const std::string& method ) const
      {
        return _TF::getArgumentSpec( method );
      };

    std::vector< ArgumentDescription > getArgumentList( const std::string& method ) const
      {
          return _TF::getArgumentList( method );
      }

    DataSourceBase* create(
      const std::string& name,
      const PropertyBag& args ) const
      {
        return _TF::produce( name, args );
      };

    DataSourceBase* create(
      const std::string& name,
      const std::vector<DataSourceBase*>& args ) const
      {
        return _TF::produce( name, args );
      };

    void add( const std::string& name,
              TemplateFactoryPart<ComponentT, DataSourceBase*>* part )
      {
        _TF::add( name, part );
      }
  };

  /**
   * This is the function that generates the DataSourceFactory you
   * want.  Here's an example of its use..
   *
   * Please refer to the documentation in
   * <orocos-tree>/parser/doc/orocos-parser.xml for more information
   * on its use.
   */
  template<typename ComponentT>
  TEMPLATE_FACTORY_NAME<ComponentT>*
  NEW_MEMBER_FACTORY( ComponentT* t )
  {
    return new TEMPLATE_FACTORY_NAME<ComponentT>( t );
  };


  /**
   * @defgroup data_factories DataSource Part Factories
   *
   * The data function is the function you should use to specify a
   * certain property to be added to your TemplateDataSourceFactory..
   * Several overloads are provided, for supporting various numbers of
   * arguments..
   *
   * There is also an overload, which allows you to provide access to
   * a member variable, thus avoiding the need for a getter
   * function..
   *
   * as the second argument, you should pass a description for the
   * data, and the following arguments are a name and description for
   * each argument of the data.
   *
   * TODO: more overloads to support a larger number of arguments.
   * We support zero to four arguments.
   * @{
   */
  template<typename ComponentT, typename ResultT>
  TemplateFactoryPart<
    typename boost::remove_const<ComponentT>::type,
    DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*var) , const char* desc )
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type,
      DataSourceBase*>( fun_datasource_gen<ComponentT, ResultT>(
                          boost::bind( var, _1 ) ), desc );
  };

  template<typename ComponentT, typename ResultT>
  TemplateFactoryPart<typename boost::remove_const<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)() MEMBER_CONST, const char* desc )
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type,
      DataSourceBase*>( fun_datasource_gen<ComponentT, ResultT>(
                          boost::mem_fn( fun ) ), desc );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T>
  TemplateFactoryPart<typename boost::remove_const<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)( Arg1T ) MEMBER_CONST, const char* desc,
        const char* a1n, const char* a1d )
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type,
      DataSourceBase*, Arg1T>(
        fun_datasource_gen<ComponentT, ResultT,
        typename remove_cr<Arg1T>::type>(
          boost::mem_fn( fun ) ), desc, a1n, a1d );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T>
  TemplateFactoryPart<typename boost::remove_const<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)( Arg1T, Arg2T ) MEMBER_CONST, const char* desc,
        const char* a1n, const char* a1d,
        const char* a2n, const char* a2d)
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type,
      DataSourceBase*, Arg1T, Arg2T>(
        fun_datasource_gen<ComponentT, ResultT,
        typename remove_cr<Arg1T>::type,
        typename remove_cr<Arg2T>::type>
        (
          boost::mem_fn( fun ) ), desc, a1n, a1d, a2n, a2d );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T>
  TemplateFactoryPart<typename boost::remove_const<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)( Arg1T, Arg2T, Arg3T ) MEMBER_CONST, const char* desc,
        const char* a1n, const char* a1d,
        const char* a2n, const char* a2d,
        const char* a3n, const char* a3d)
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type,
      DataSourceBase*, Arg1T, Arg2T, Arg3T>(
        fun_datasource_gen<ComponentT, ResultT,
        typename remove_cr<Arg1T>::type,
        typename remove_cr<Arg2T>::type,
        typename remove_cr<Arg3T>::type>
        (
          boost::mem_fn( fun ) ), desc, a1n, a1d, a2n, a2d, a3n, a3d);
  };

  template<typename ComponentT, typename ResultT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
  TemplateFactoryPart<typename boost::remove_const<ComponentT>::type,
                      DataSourceBase*>*
  MEMBER( ResultT (ComponentT::*fun)( Arg1T, Arg2T, Arg3T, Arg4T ) MEMBER_CONST, const char* desc,
        const char* a1n, const char* a1d,
        const char* a2n, const char* a2d,
        const char* a3n, const char* a3d,
        const char* a4n, const char* a4d)
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type,
      DataSourceBase*, Arg1T, Arg2T, Arg3T, Arg4T>(
        fun_datasource_gen<ComponentT, ResultT,
        typename remove_cr<Arg1T>::type,
        typename remove_cr<Arg2T>::type,
        typename remove_cr<Arg3T>::type,
        typename remove_cr<Arg4T>::type>
        (
          boost::mem_fn( fun ) ), desc, a1n, a1d, a2n, a2d, a3n, a3d, a4n, a4d );
  };

  /**
   * @}
   */
#endif // #ifndef NO_DOXYGEN

}

