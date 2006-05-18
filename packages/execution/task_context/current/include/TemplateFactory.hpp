/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  TemplateFactory.hpp 

                        TemplateFactory.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
#ifndef TEMPLATEFACTORY_HPP
#define TEMPLATEFACTORY_HPP

#include <boost/lexical_cast.hpp>
#include <boost/call_traits.hpp>
#include <boost/bind.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <string>

#include <corelib/mystd.hpp>

#include <corelib/PropertyBag.hpp>
#include <corelib/Property.hpp>
#include <corelib/DataSourceAdaptor.hpp>
#include "DataSource.hpp"
#include "FactoryExceptions.hpp"
#include "ArgumentDescription.hpp"

/**
 * @file TemplateFactory.hpp This file contains some code that is common between
 * TemplateCommandFactory and TemplateDataSourceFactory.
 */

namespace ORO_Execution
{
    namespace detail {
    /**
     * Type-selection for return type of methods and datasources.
     * primitives return by value, composite types by const reference
     */
    template<typename R>
    struct ReturnType
    {
        //typedef typename boost::call_traits<R>::param_type type;
        typedef R type;
    };
    /**
     * Type-selection for argument of methods, commands, datasources.
     * primitives pass by value, composite types by const reference
     */
    template<typename A>
    struct ArgType
    {
        //typedef typename boost::call_traits<A>::param_type type;
        typedef A type;
    };

    /**
     * The type of the component, We need to remove the const qualifier
     * from the component type when creating the factory parts, this qualifier
     * is present because the user uses '&Component::fun const' member ptrs
     * for datasources.
     */
    template<typename C>
    struct CompType
    {
        typedef typename boost::remove_const<C>::type type;
    };

    
  using ORO_CoreLib::Property;
  using ORO_CoreLib::PropertyBase;
  using ORO_CoreLib::PropertyBag;

        /**
         * @defgroup CMDFactories Command, Method and DataSource Factories.
         * This module groups all factores for commands, methods and 
         * datasources used in the ORO_Execution namespace. For each command,
         * method or datasource you wish to add to a factory, you need to generate
         * a \ref partFunctions. You \a add the part to the respective \ref objectFactory factory, which is in
         * turn added to a TaskContext's \ref globalFactory factory. 
         *
         * Thus : 
         *    -each \a command,\a dataource,\a method of your TaskContext requires one \a part. 
         *    -each \a part is inserted in exactly one \a object factory.
         *    -each \a object factory is inserted in exactly one \a global factory of a TaskContext.
         */ 

        /**
         * @defgroup partFunctions Functions to generate a 'Part Factory Object'
         * @ingroup CMDFactories
         * This module contains all 'C' functions you can use to generate 'part' object
         * factories. A part matches exactly one method, command or datasource.
         */

        /**
         * @defgroup objectFactory TaskContext Object Factories
         * @ingroup CMDFactories
         * An object factory delivers DataSources for the method
         * or datasource interface of a TaskContext or Command objects for the command
         * interface of a TaskContext. An object groups multiple 'parts'.
         */ 

        /**
         * @defgroup globalFactory TaskContext (Global) Interface Factories
         * @ingroup CMDFactories
         * There are only three 'Global' factories in a TaskContext delivering
         * commands, methods or datasources.
         */ 

  /**
   * @internal
   * @defgroup TemplateFactoryPart Base Classes for parts.
   * @brief Class keeping the information on how to generate one thing that
   * the factory can generate.
   *
   * Each name in the TemplateFactory will
   * be linked with one TemplateFactoryPart that knows how to produce
   * the thing that the name is used for..  Below are standard
   * implementations for functors of various signatures.
   * @{
   */
  template<typename ComponentT, typename ResultT>
  class TemplateFactoryPart
  {
      const char* mdesc;
  public:
      TemplateFactoryPart( const char* desc )
          : mdesc( desc )
      {
      }

      virtual ~TemplateFactoryPart() {};
      std::string description() const
      {
          return mdesc;
      }
      /**
       * Return the result (return) type of this part.
       */
      virtual std::string resultType() const = 0;

      /**
       * Get a description of the desired arguments in
       * the property format.
       */
      virtual PropertyBag getArgumentSpec() const = 0;
      /**
       * Get a description of the desired arguments in
       * the ArgumentDescription format.
       */
      virtual std::vector<ArgumentDescription> getArgumentList() const = 0;

      /**
       * Returns the arity (number of arguments) of this part.
       */
      virtual int arity() const = 0;

      /**
       * Create one part (function object) for a given component.
       * @param com The target object whose function is called.
       * @param args The arguments for the target object's function.
       * @param read_now true : Read-out the \a args right now (and store). 
       *                 false: store pointers to \a args and read out upon invocation.
       * @return An object which (internally) invokes the function of \a com with
       *         arguments \a args. Examples are ORO_CoreLib::DataSourceBase, ConditionInterface and CommandInterface.
       */
      virtual ResultT produce( ComponentT* com,
                               const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args
                               ) const = 0;
  };

  template<typename ComponentT, typename ResultT, typename FunctorT>
  class TemplateFactoryFunctorPart0
    : public TemplateFactoryPart<ComponentT, ResultT>
  {
    typedef FunctorT fun_t;
    fun_t fun;
  public:
    TemplateFactoryFunctorPart0( fun_t f, const char* desc )
      : TemplateFactoryPart<ComponentT,ResultT>( desc ), fun( f )
      {
      }

    PropertyBag getArgumentSpec() const
      {
        return PropertyBag();
      }

      std::string resultType() const
      {
          return DataSource<typename FunctorT::result_type>::GetType();
      }

    std::vector<ArgumentDescription> getArgumentList() const
      {
        return std::vector<ArgumentDescription>();
      }

      int arity() const { return 0; }

    ResultT produce(
      ComponentT* comp,
      const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args) const
      {
        if ( ! args.empty() )
          throw wrong_number_of_args_exception( 0, args.size() );
        return fun( comp );
      }
  };

  template<typename ComponentT, typename ResultT, typename FunctorT,
           typename FirstArgumentT>
  class TemplateFactoryFunctorPart1
    : public TemplateFactoryPart<ComponentT, ResultT>
  {
    typedef FunctorT fun_t;
    typedef FirstArgumentT first_argument_type;
    fun_t fun;
    const char* arg1name;
    const char* arg1desc;
  public:
    TemplateFactoryFunctorPart1( fun_t f, const char* desc,
                                 const char* a1n, const char* a1d )
      : TemplateFactoryPart<ComponentT,ResultT>( desc ),
        fun( f ), arg1name( a1n ), arg1desc( a1d )
      {
      }

      std::string resultType() const
      {
          return DataSource<typename FunctorT::result_type>::GetType();
      }

     std::vector< ArgumentDescription > getArgumentList( ) const
      {
          std::vector< ArgumentDescription > mlist;
          mlist.push_back( ArgumentDescription( arg1name, arg1desc, DataSource<first_argument_type>::GetType() ) );
          return mlist;
      }

    PropertyBag getArgumentSpec() const
      {
        PropertyBag ret;
        ret.add( new Property<first_argument_type>( arg1name, arg1desc ) );
        return ret;
      }

      int arity() const { return 1; }

    ResultT produce(
      ComponentT* comp,
      const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args) const
      {
        if ( args.size() != 1 )
            throw wrong_number_of_args_exception( 1, args.size() );
        typename DataSource<first_argument_type>::shared_ptr a =
            ORO_CoreLib::AdaptDataSource<first_argument_type>()( args[0] );
        if ( ! a )
            throw wrong_types_of_args_exception( 1, DataSource<first_argument_type>::GetType(), args[0]->getType() );
        return fun( comp, a.get() );
      }
  };

  template<typename ComponentT, typename ResultT, typename FunctorT,
           typename FirstArgumentT, typename SecondArgumentT>
  class TemplateFactoryFunctorPart2
    : public TemplateFactoryPart<ComponentT, ResultT>
  {
    typedef FunctorT fun_t;
    typedef FirstArgumentT first_argument_type;
    typedef SecondArgumentT second_argument_type;
    fun_t fun;
    const char* arg1name;
    const char* arg1desc;
    const char* arg2name;
    const char* arg2desc;
  public:
    TemplateFactoryFunctorPart2( fun_t f, const char* desc, const char* a1n,
                                 const char* a1d, const char* a2n,
                                 const char* a2d)
      : TemplateFactoryPart<ComponentT,ResultT>( desc ), fun( f ),
        arg1name( a1n ), arg1desc( a1d ), arg2name( a2n ),
        arg2desc( a2d )
      {
      }

      std::string resultType() const
      {
          return DataSource<typename FunctorT::result_type>::GetType();
      }

     std::vector< ArgumentDescription > getArgumentList( ) const
      {
          std::vector< ArgumentDescription > mlist;
          mlist.push_back( ArgumentDescription( arg1name, arg1desc, DataSource<first_argument_type>::GetType() ) );
          mlist.push_back( ArgumentDescription( arg2name, arg2desc, DataSource<second_argument_type>::GetType() ) );
          return mlist;
      }

    PropertyBag getArgumentSpec() const
      {
        PropertyBag ret;
        ret.add( new Property<first_argument_type>( arg1name, arg1desc ) );
        ret.add( new Property<second_argument_type>( arg2name, arg2desc ) );
        return ret;
      }

      int arity() const { return 2; }

    ResultT produce( ComponentT* comp, const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args) const
      {
        if ( args.size() != 2 )
          throw wrong_number_of_args_exception( 2, args.size() );

        typename DataSource<first_argument_type>::shared_ptr a =
          ORO_CoreLib::AdaptDataSource<first_argument_type>()( args[0] );
        if ( !a ) 
            throw wrong_types_of_args_exception( 1, DataSource<first_argument_type>::GetType(), args[0]->getType() );
        typename DataSource<second_argument_type>::shared_ptr b =
          ORO_CoreLib::AdaptDataSource<second_argument_type>()( args[1] );
        if ( !b ) 
            throw wrong_types_of_args_exception( 2, DataSource<second_argument_type>::GetType(), args[1]->getType() );

        return fun( comp, a.get(), b.get() );
      }
  };

  template<typename ComponentT, typename ResultT, typename FunctorT,
           typename FirstArgumentT, typename SecondArgumentT,
           typename ThirdArgumentT>
  class TemplateFactoryFunctorPart3
    : public TemplateFactoryPart<ComponentT, ResultT>
  {
    typedef FunctorT fun_t;
    typedef FirstArgumentT first_argument_type;
    typedef SecondArgumentT second_argument_type;
    typedef ThirdArgumentT  third_argument_type;

    fun_t fun;
    const char* arg1name;
    const char* arg1desc;
    const char* arg2name;
    const char* arg2desc;
    const char* arg3name;
    const char* arg3desc;
  public:
    TemplateFactoryFunctorPart3( fun_t f, const char* desc, const char* a1n,
                                 const char* a1d, const char* a2n,
                                 const char* a2d, const char* a3n,
                                 const char* a3d )
      : TemplateFactoryPart<ComponentT,ResultT>( desc ), fun( f ),
        arg1name( a1n ), arg1desc( a1d ),
        arg2name( a2n ), arg2desc( a2d ),
        arg3name( a3n ), arg3desc( a3d )
      {
      }

      std::string resultType() const
      {
          return DataSource<typename FunctorT::result_type>::GetType();
      }

    PropertyBag getArgumentSpec() const
      {
        PropertyBag ret;
        ret.add( new Property<first_argument_type>( arg1name, arg1desc ) );
        ret.add( new Property<second_argument_type>( arg2name, arg2desc ) );
        ret.add( new Property<third_argument_type>( arg3name, arg3desc ) );
        return ret;
      }

     std::vector< ArgumentDescription > getArgumentList( ) const
      {
          std::vector< ArgumentDescription > mlist;
          mlist.push_back( ArgumentDescription( arg1name, arg1desc, DataSource<first_argument_type>::GetType() ) );
          mlist.push_back( ArgumentDescription( arg2name, arg2desc, DataSource<second_argument_type>::GetType() ) );
          mlist.push_back( ArgumentDescription( arg3name, arg3desc, DataSource<third_argument_type>::GetType() ) );
          return mlist;
      }

      int arity() const { return 3; }

    ResultT produce( ComponentT* comp, const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args) const
      {
        if ( args.size() != 3 )
          throw wrong_number_of_args_exception( 3, args.size() );

        typename DataSource<first_argument_type>::shared_ptr a =
          ORO_CoreLib::AdaptDataSource<first_argument_type>()( args[0] );
        if ( !a ) 
            throw wrong_types_of_args_exception( 1, DataSource<first_argument_type>::GetType(), args[0]->getType() );
        typename DataSource<second_argument_type>::shared_ptr b =
          ORO_CoreLib::AdaptDataSource<second_argument_type>()( args[1] );
        if ( !b ) 
            throw wrong_types_of_args_exception( 2, DataSource<second_argument_type>::GetType(), args[1]->getType() );
        typename DataSource<third_argument_type>::shared_ptr c =
          ORO_CoreLib::AdaptDataSource<third_argument_type>()( args[2] );
        if ( !c ) 
            throw wrong_types_of_args_exception( 3, DataSource<third_argument_type>::GetType(), args[2]->getType() );

        return fun( comp, a.get(), b.get(), c.get() );
      }
  };

  template<typename ComponentT, typename ResultT, typename FunctorT,
           typename FirstArgumentT, typename SecondArgumentT,
           typename ThirdArgumentT, typename FourthArgumentT>
  class TemplateFactoryFunctorPart4
    : public TemplateFactoryPart<ComponentT, ResultT>
  {
    typedef FunctorT fun_t;
    typedef FirstArgumentT first_argument_type;
    typedef SecondArgumentT second_argument_type;
    typedef ThirdArgumentT  third_argument_type;
    typedef FourthArgumentT  fourth_argument_type;

    fun_t fun;
    const char* arg1name;
    const char* arg1desc;
    const char* arg2name;
    const char* arg2desc;
    const char* arg3name;
    const char* arg3desc;
    const char* arg4name;
    const char* arg4desc;
  public:
    TemplateFactoryFunctorPart4( fun_t f, const char* desc, const char* a1n,
                                 const char* a1d, const char* a2n,
                                 const char* a2d, const char* a3n,
                                 const char* a3d, const char* a4n,
                                 const char* a4d )
      : TemplateFactoryPart<ComponentT,ResultT>( desc ), fun( f ),
        arg1name( a1n ), arg1desc( a1d ),
        arg2name( a2n ), arg2desc( a2d ),
        arg3name( a3n ), arg3desc( a3d ),
        arg4name( a4n ), arg4desc( a4d )
      {
      }

      std::string resultType() const
      {
          return DataSource<typename FunctorT::result_type>::GetType();
      }

    PropertyBag getArgumentSpec() const
      {
        PropertyBag ret;
        ret.add( new Property<first_argument_type>( arg1name, arg1desc ) );
        ret.add( new Property<second_argument_type>( arg2name, arg2desc ) );
        ret.add( new Property<third_argument_type>( arg3name, arg3desc ) );
        ret.add( new Property<fourth_argument_type>( arg4name, arg4desc ) );
        return ret;
      }

     std::vector< ArgumentDescription > getArgumentList( ) const
      {
          std::vector< ArgumentDescription > mlist;
          mlist.push_back( ArgumentDescription( arg1name, arg1desc, DataSource<first_argument_type>::GetType() ) );
          mlist.push_back( ArgumentDescription( arg2name, arg2desc, DataSource<second_argument_type>::GetType() ) );
          mlist.push_back( ArgumentDescription( arg3name, arg3desc, DataSource<third_argument_type>::GetType() ) );
          mlist.push_back( ArgumentDescription( arg4name, arg4desc, DataSource<fourth_argument_type>::GetType() ) );
          return mlist;
      }

      int arity() const { return 4; }

    ResultT produce( ComponentT* comp, const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args) const
      {
        if ( args.size() != 4 )
          throw wrong_number_of_args_exception( 4, args.size() );

        typename DataSource<first_argument_type>::shared_ptr a =
          ORO_CoreLib::AdaptDataSource<first_argument_type>()( args[0] );
        if ( !a ) 
            throw wrong_types_of_args_exception( 1, DataSource<first_argument_type>::GetType(), args[0]->getType() );
        typename DataSource<second_argument_type>::shared_ptr b =
          ORO_CoreLib::AdaptDataSource<second_argument_type>()( args[1] );
        if ( !b ) 
            throw wrong_types_of_args_exception( 2, DataSource<second_argument_type>::GetType(), args[1]->getType() );
        typename DataSource<third_argument_type>::shared_ptr c =
          ORO_CoreLib::AdaptDataSource<third_argument_type>()( args[2] );
        if ( !c ) 
            throw wrong_types_of_args_exception( 3, DataSource<third_argument_type>::GetType(), args[2]->getType() );
        typename DataSource<fourth_argument_type>::shared_ptr d =
          ORO_CoreLib::AdaptDataSource<fourth_argument_type>()( args[3] );
        if ( !d )
            throw wrong_types_of_args_exception( 4, DataSource<fourth_argument_type>::GetType(), args[3]->getType() );

        return fun( comp, a.get(), b.get(), c.get(), d.get() );
      }
  };
    /**
     * @}
     */

  /**
   * @internal
   * @defgroup FactoryFunctorPartNHelper Helper functions for the FactoryFunctorPartN's
   * These functions create new TemplateFactoryFunctorPart instances.
   * @{
   */
  template<typename ComponentT, typename ResultT, typename FunctorT>
  TemplateFactoryFunctorPart0<ComponentT, ResultT, FunctorT>*
  fun_fact( FunctorT fun, const char* desc )
  {
    return new TemplateFactoryFunctorPart0<ComponentT, ResultT, FunctorT>(
      fun, desc );
  }

  template<typename ComponentT, typename ResultT, typename FunctorT>
  TemplateFactoryPart< DataSource< boost::weak_ptr<ComponentT> >, ResultT>*
  fun_fact_ds( FunctorT fun, const char* desc )
  {
    return new TemplateFactoryFunctorPart0< DataSource<boost::weak_ptr<ComponentT> >, ResultT, FunctorT>(
      fun, desc );
  }

  template<typename ComponentT, typename ResultT,
           typename Arg1T, typename FunctorT>
  TemplateFactoryFunctorPart1<ComponentT, ResultT, FunctorT, Arg1T>*
  fun_fact( FunctorT fun, const char* desc, const char* an, const char* ad )
  {
    return new TemplateFactoryFunctorPart1<ComponentT, ResultT, FunctorT,
      Arg1T>( fun, desc, an, ad );
  }

  template<typename ComponentT, typename ResultT,
           typename Arg1T, typename FunctorT>
  TemplateFactoryPart< DataSource< boost::weak_ptr<ComponentT> >, ResultT>*
  fun_fact_ds( FunctorT fun, const char* desc, const char* an, const char* ad )
  {
    return new TemplateFactoryFunctorPart1< DataSource<boost::weak_ptr<ComponentT> >, ResultT, FunctorT,
      Arg1T>( fun, desc, an, ad );
  }

  template<typename ComponentT, typename ResultT,
           typename Arg1T, typename Arg2T, typename FunctorT>
  TemplateFactoryFunctorPart2<ComponentT, ResultT, FunctorT, Arg1T, Arg2T>*
  fun_fact( FunctorT fun, const char* desc, const char* an, const char* ad,
            const char* bn, const char* bd )
  {
    return new TemplateFactoryFunctorPart2<ComponentT, ResultT, FunctorT,
      Arg1T, Arg2T>( fun, desc, an, ad, bn, bd );
  }

  template<typename ComponentT, typename ResultT,
           typename Arg1T, typename Arg2T, typename Arg3T,
           typename FunctorT>
  TemplateFactoryFunctorPart3<ComponentT, ResultT, FunctorT, Arg1T, Arg2T,
                              Arg3T>*
  fun_fact( FunctorT fun, const char* desc, const char* an, const char* ad,
            const char* bn, const char* bd, const char* cn, const char* cd )
  {
    return new TemplateFactoryFunctorPart3<ComponentT, ResultT, FunctorT,
      Arg1T, Arg2T, Arg3T>( fun, desc, an, ad, bn, bd, cn, cd);
  }

  template<typename ComponentT, typename ResultT,
           typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T,
           typename FunctorT>
  TemplateFactoryFunctorPart4<ComponentT, ResultT, FunctorT, Arg1T, Arg2T,
                              Arg3T, Arg4T>*
  fun_fact( FunctorT fun, const char* desc, const char* an, const char* ad,
            const char* bn, const char* bd, const char* cn, const char* cd, const char* dn, const char* dd )
  {
    return new TemplateFactoryFunctorPart4<ComponentT, ResultT, FunctorT,
      Arg1T, Arg2T, Arg3T, Arg4T>( fun, desc, an, ad, bn, bd, cn, cd, dn, dd);
  }
  /**
   * @}
   */

    }

    /**
     * @brief This factory is a template for creating parts, on a per
     * component basis. The parts themselves are created by
     * the TemplateFactoryFunctorPart functions.
     */
  template<typename ComponentT, typename ResultT>
  class TemplateFactory
  {
  protected:
    typedef std::map<std::string, detail::TemplateFactoryPart<ComponentT, ResultT>* > map_t;
    map_t data;
    ComponentT* comp;
  public:
    TemplateFactory( ComponentT* com )
      : comp( com )
      {
      }

    ~TemplateFactory()
      {
        for ( typename map_t::iterator i = data.begin(); i != data.end(); ++i )
          delete i->second;
      }

    std::vector<std::string> getNames() const
      {
        std::vector<std::string> ret;
        std::transform( data.begin(), data.end(),
                        std::back_inserter( ret ),
                        ORO_std::select1st<typename map_t::value_type>() );
        return ret;
      }

    bool hasMember( const std::string& name ) const
      {
        return data.find( name ) != data.end();
      }

      int getArity( const std::string& name ) const
      {
          typename map_t::const_iterator i = data.find( name );
          if ( i == data.end() ) return -1;
          return i->second->arity();
      }

    ResultT produce( const std::string& name, const ORO_CoreLib::PropertyBag& args ) const
      {
        typename map_t::const_iterator i = data.find( name );
        if ( i == data.end() ) throw name_not_found_exception();
        std::vector<ORO_CoreLib::DataSourceBase::shared_ptr> dsVect;
        std::transform( args.begin(), args.end(),
                        std::back_inserter( dsVect ),
                        boost::bind( &ORO_CoreLib::PropertyBase::getDataSource, _1));
        return i->second->produce( comp, dsVect);
      }

    ResultT produce( const std::string& name,
                     const std::vector<ORO_CoreLib::DataSourceBase*>& args ) const
      {
        typename map_t::const_iterator i = data.find( name );
        if ( i == data.end() ) throw name_not_found_exception();
        std::vector<ORO_CoreLib::DataSourceBase::shared_ptr> dsVect;
        std::vector<ORO_CoreLib::DataSourceBase*>::const_iterator di(args.begin());
        for( ; di != args.end(); ++di )
            dsVect.push_back( ORO_CoreLib::DataSourceBase::shared_ptr( *di ) );
        return i->second->produce( comp, dsVect );
      }

    ResultT produce( const std::string& name,
                     const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args ) const
      {
        typename map_t::const_iterator i = data.find( name );
        if ( i == data.end() ) throw name_not_found_exception();
        return i->second->produce( comp, args );
      }

    ORO_CoreLib::PropertyBag getArgumentSpec( const std::string& name ) const
      {
        typename map_t::const_iterator i = data.find( name );
        if ( i == data.end() ) throw name_not_found_exception();
        return i->second->getArgumentSpec();
      }

    std::vector<ArgumentDescription> getArgumentList( const std::string& name ) const
      {
        typename map_t::const_iterator i = data.find( name );
        if ( i == data.end() ) throw name_not_found_exception();
        return i->second->getArgumentList();
      }

    std::string getResultType( const std::string& name ) const
      {
        typename map_t::const_iterator i = data.find( name );
        if ( i == data.end() ) throw name_not_found_exception();
        return i->second->resultType();
      }

    std::string getDescription( const std::string& name ) const
      {
        typename map_t::const_iterator i = data.find( name );
        if ( i == data.end() ) throw name_not_found_exception();
        return i->second->description();
      }

    void add( const std::string& name,
              detail::TemplateFactoryPart<ComponentT, ResultT>* part )
      {
        typename map_t::iterator i = data.find( name );
        // XXX, wouldn't it be better to throw ?
        if ( i != data.end() )
          delete i->second;
        data[name] = part;
      }
  };
}

#endif
