/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  TemplateDataSourceFactory.hpp 

                        TemplateDataSourceFactory.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TEMPLATE_DATASOURCE_FACTORY_HPP 
#define TEMPLATE_DATASOURCE_FACTORY_HPP
 
#include "DataSource.hpp"
#include "DataSourceFactory.hpp"
#include "TemplateFactory.hpp"

#include <boost/bind.hpp>
#include <boost/type_traits.hpp>

/**
 * @file This file contains the TemplateDataSourceFactory template,
 * which is a template designed to reduce the amount of boilerplate
 * code necessary to add a DataSourceFactory to an OROCOS component.
 * If you're not interested in the implementation details ( warning:
 * template code, may seem a bit messy ), then skip forward to the
 * documentation of the @ref newDataSourceFactory function.
 */
namespace ORO_Execution
{
    /**
     * @internal These classes are generic DataSources that take a
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
        };

        virtual DataSource<value_t>* clone() const
        {
            return new FunctorDataSource0( gen );
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
      };

        virtual DataSource<value_t>* clone() const
        {
            return new FunctorDataSource1( gen, arg1 );
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
  /**
   * @}
   */

  /**
   * @internal This is the DataSource factory class that is generated
   * by the newDataSourceFactory function..  Use the helper function
   * newDataSourceFactory.
   */
  template<typename ComponentT>
  class TemplateDataSourceFactory
    : public DataSourceFactory,
      private TemplateFactory<ComponentT, DataSourceBase*>
  {
    typedef TemplateFactory<ComponentT, DataSourceBase*> _TF;
  public:
    TemplateDataSourceFactory( ComponentT* c )
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
  TemplateDataSourceFactory<ComponentT>*
  newDataSourceFactory( ComponentT* t )
  {
    return new TemplateDataSourceFactory<ComponentT>( t );
  };

  /**
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
   * @{
   */
  template<typename ComponentT, typename ResultT>
  TemplateFactoryPart<
    typename boost::remove_const<ComponentT>::type,
    DataSourceBase*>*
  data( ResultT (ComponentT::*var), const char* desc )
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type,
      DataSourceBase*>( fun_datasource_gen<ComponentT, ResultT>(
                          boost::bind( var, _1 ) ), desc );
  };

  template<typename ComponentT, typename ResultT>
  TemplateFactoryPart<typename boost::remove_const<ComponentT>::type,
                      DataSourceBase*>*
  data( ResultT (ComponentT::*fun)(), const char* desc )
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type,
      DataSourceBase*>( fun_datasource_gen<ComponentT, ResultT>(
                          boost::mem_fn( fun ) ), desc );
  };

  template<typename ComponentT, typename ResultT, typename Arg1T>
  TemplateFactoryPart<typename boost::remove_const<ComponentT>::type,
                      DataSourceBase*>*
  data( ResultT (ComponentT::*fun)( Arg1T ), const char* desc,
        const char* a1n, const char* a1d )
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type,
      DataSourceBase*, Arg1T>(
        fun_datasource_gen<ComponentT, ResultT,
        typename remove_cr<Arg1T>::type>(
          boost::mem_fn( fun ) ), desc, a1n, a1d );
  };

  /**
   * @}
   */
}

#endif
