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
#include <boost/type_traits.hpp>

#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <string>

#include <corelib/PropertyBag.hpp>
#include <corelib/Property.hpp>
#include "DataSource.hpp"
#include "FactoryExceptions.hpp"
#include "ArgumentDescription.hpp"

/**
 * @file This file contains some code that is common between
 * TemplateCommandFactory and TemplateDataSourceFactory.
 */

namespace ORO_Execution
{

#ifndef NO_DOXYGEN
  namespace
  {
    // combines remove_reference and remove_const
    template<typename T>
    struct remove_cr
    {
      typedef typename boost::remove_const<
        typename boost::remove_reference<T>::type>::type type;
    };
  }

  // My STL seems to be lacking std::select1st..  Seems like it's an
  // SGI extension, so I guess I can't blaim them..
  namespace mystl
  {
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
  };

  using ORO_CoreLib::Property;
  using ORO_CoreLib::PropertyBase;
  using ORO_CoreLib::PropertyBag;

  /**
   * @defgroup TemplateFactoryPart
   * @brief Class keeping the information on how to generate one thing that
   * the factory can generate.
   *
   * Each name in the TemplateFactory will
   * be linked with one TemplateFactoryPart that knows how to produce
   * the thing that the name is used for..  Below are standard
   * implementations for functors of various signatures.
   * @internal
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
    virtual PropertyBag getArgumentSpec() const = 0;
    virtual std::vector<ArgumentDescription> getArgumentList() const = 0;
    virtual ResultT produce( ComponentT* com,
                             const PropertyBag& args ) const = 0;
    virtual ResultT produce(
      ComponentT* com,
      const std::vector<DataSourceBase*>& args ) const = 0;
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

    std::vector<ArgumentDescription> getArgumentList() const
      {
        return std::vector<ArgumentDescription>();
      }

    ResultT produce( ComponentT* c, const PropertyBag& bag ) const
      {
        if ( ! bag.getProperties().empty() )
          throw wrong_number_of_args_exception(
           0,  bag.getProperties().size() );
        return fun( c );
      }

    ResultT produce(
      ComponentT* comp,
      const std::vector<DataSourceBase*>& args ) const
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
    typedef typename remove_cr<FirstArgumentT>::type
      first_argument_type;
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

     std::vector< ArgumentDescription > getArgumentList( ) const
      {
          std::vector< ArgumentDescription > mlist;
          mlist.push_back( ArgumentDescription( arg1name, arg1desc ) );
          return mlist;
      }

    PropertyBag getArgumentSpec() const
      {
        PropertyBag ret;
        ret.add( new Property<first_argument_type>( arg1name, arg1desc ) );
        return ret;
      }
    ResultT produce( ComponentT* c, const PropertyBag& bag ) const
      {
        PropertyBag::PropertyContainerType props = bag.getProperties();
        if ( props.size() != 1 )
          throw wrong_number_of_args_exception( 1, props.size() );
        Property<first_argument_type>* arg1 =
          dynamic_cast<Property<first_argument_type>*>( props[0] );
        if ( ! arg1 )
          throw wrong_types_of_args_exception( 1 );
        return fun( c, arg1->get() );
      }
    ResultT produce(
      ComponentT* comp,
      const std::vector<DataSourceBase*>& args ) const
      {
        if ( args.size() != 1 )
          throw wrong_number_of_args_exception( 1, args.size() );
        DataSource<first_argument_type>* a =
          dynamic_cast<DataSource<first_argument_type>*>( args[0] );
        if ( ! a )
          throw wrong_types_of_args_exception( 1 );
        return fun( comp, a );
      }
  };

  template<typename ComponentT, typename ResultT, typename FunctorT,
           typename FirstArgumentT, typename SecondArgumentT>
  class TemplateFactoryFunctorPart2
    : public TemplateFactoryPart<ComponentT, ResultT>
  {
    typedef FunctorT fun_t;
    typedef typename remove_cr<FirstArgumentT>::type
      first_argument_type;
    typedef typename remove_cr<SecondArgumentT>::type
      second_argument_type;
    fun_t fun;
    const char* arg1name;
    const char* arg1desc;
    const char* arg2name;
    const char* arg2desc;
  public:
    TemplateFactoryFunctorPart2( fun_t f, const char* desc, const char* a1n,
                                 const char* a1d, const char* a2n,
                                 const char* a2d )
      : TemplateFactoryPart<ComponentT,ResultT>( desc ), fun( f ),
        arg1name( a1n ), arg1desc( a1d ), arg2name( a2n ),
        arg2desc( a2d )
      {
      }

     std::vector< ArgumentDescription > getArgumentList( ) const
      {
          std::vector< ArgumentDescription > mlist;
          mlist.push_back( ArgumentDescription( arg1name, arg1desc ) );
          mlist.push_back( ArgumentDescription( arg2name, arg2desc ) );
          return mlist;
      }

    PropertyBag getArgumentSpec() const
      {
        PropertyBag ret;
        ret.add( new Property<first_argument_type>( arg1name, arg1desc ) );
        ret.add( new Property<second_argument_type>( arg2name, arg2desc ) );
        return ret;
      }
    ResultT produce( ComponentT* comp, const PropertyBag& bag ) const
      {
        PropertyBag::PropertyContainerType props = bag.getProperties();
        if ( props.size() != 2 )
          throw wrong_number_of_args_exception( 2, props.size() );
        Property<first_argument_type>* arg1 =
          dynamic_cast<Property<first_argument_type>*>( props[0] );
        if ( ! arg1 ) throw wrong_types_of_args_exception( 1 );
        Property<second_argument_type>* arg2 =
          dynamic_cast<Property<second_argument_type>*>( props[1] );
        if ( !arg2 ) throw wrong_types_of_args_exception( 2 );
        return fun( comp, arg1->get(), arg2->get() );
      }
    ResultT produce( ComponentT* comp, const std::vector<DataSourceBase*>& args ) const
      {
        if ( args.size() != 2 )
          throw wrong_number_of_args_exception( 2, args.size() );
        DataSource<first_argument_type>* a =
          dynamic_cast<DataSource<first_argument_type>*>( args[0] );
        if ( !a ) throw wrong_types_of_args_exception( 1 );
        DataSource<second_argument_type>* b =
          dynamic_cast<DataSource<second_argument_type>*>( args[1] );
        if ( !b ) throw wrong_types_of_args_exception( 2 );
        return fun( comp, a, b );
      }
  };

  template<typename ComponentT, typename ResultT, typename FunctorT,
           typename FirstArgumentT, typename SecondArgumentT,
           typename ThirdArgumentT>
  class TemplateFactoryFunctorPart3
    : public TemplateFactoryPart<ComponentT, ResultT>
  {
    typedef FunctorT fun_t;
    typedef typename remove_cr<FirstArgumentT>::type
      first_argument_type;
    typedef typename remove_cr<SecondArgumentT>::type
      second_argument_type;
    typedef typename remove_cr<ThirdArgumentT>::type
      third_argument_type;

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
          mlist.push_back( ArgumentDescription( arg1name, arg1desc ) );
          mlist.push_back( ArgumentDescription( arg2name, arg2desc ) );
          mlist.push_back( ArgumentDescription( arg3name, arg3desc ) );
          return mlist;
      }

    ResultT produce( ComponentT* comp, const PropertyBag& bag ) const
      {
        PropertyBag::PropertyContainerType props = bag.getProperties();
        if ( props.size() != 3 )
          throw wrong_number_of_args_exception( 3, props.size() );
        Property<first_argument_type>* arg1 =
          dynamic_cast<Property<first_argument_type>*>( props[0] );
        if ( !arg1 ) throw wrong_types_of_args_exception( 1 );
        Property<second_argument_type>* arg2 =
          dynamic_cast<Property<second_argument_type>*>( props[1] );
        if ( !arg2 ) throw wrong_types_of_args_exception( 2 );
        Property<third_argument_type>* arg3 =
          dynamic_cast<Property<third_argument_type>*>( props[2] );
        if ( !arg3 ) throw wrong_types_of_args_exception( 3 );
        return fun( comp, arg1->get(), arg2->get(), arg3->get() );
      }
    ResultT produce( ComponentT* comp, const std::vector<DataSourceBase*>& args ) const
      {
        if ( args.size() != 3 )
          throw wrong_number_of_args_exception( 3, args.size() );
        DataSource<first_argument_type>* a =
          dynamic_cast<DataSource<first_argument_type>*>( args[0] );
        if ( !a ) throw wrong_types_of_args_exception( 1 );
        DataSource<second_argument_type>* b =
          dynamic_cast<DataSource<second_argument_type>*>( args[1] );
        if ( !b ) throw wrong_types_of_args_exception( 2 );
        DataSource<third_argument_type>* c =
          dynamic_cast<DataSource<third_argument_type>*>( args[2] );
        if ( !c ) throw wrong_types_of_args_exception( 3 );
        return fun( comp, a, b, c );
      }
  };
    /**
     * @}
     */

  /**
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

  template<typename ComponentT, typename ResultT,
           typename Arg1T, typename FunctorT>
  TemplateFactoryFunctorPart1<ComponentT, ResultT, FunctorT, Arg1T>*
  fun_fact( FunctorT fun, const char* desc, const char* an, const char* ad )
  {
    return new TemplateFactoryFunctorPart1<ComponentT, ResultT, FunctorT,
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
  /**
   * @}
   */
#endif // #ifndef NO_DOXYGEN

    /**
     * @brief This factory is a template for creating parts, on a per
     * component basis. The parts themselves are created by
     * the TemplateFactoryFunctorPart functions.
     */
  template<typename ComponentT, typename ResultT>
  class TemplateFactory
  {
  protected:
    typedef std::map<std::string, TemplateFactoryPart<ComponentT, ResultT>* > map_t;
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
                        mystl::select1st<typename map_t::value_type>() );
        return ret;
      }

    bool hasName( const std::string& name ) const
      {
        return data.find( name ) != data.end();
      }

    ResultT produce( const std::string& name, const PropertyBag& args ) const
      {
        typename map_t::const_iterator i = data.find( name );
        if ( i == data.end() ) throw name_not_found_exception();
        return i->second->produce( comp, args );
      }

    ResultT produce( const std::string& name,
                     const std::vector<DataSourceBase*>& args ) const
      {
        typename map_t::const_iterator i = data.find( name );
        if ( i == data.end() ) throw name_not_found_exception();
        return i->second->produce( comp, args );
      }

    PropertyBag getArgumentSpec( const std::string& name ) const
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

    std::string getDescription( const std::string& name ) const
      {
        typename map_t::const_iterator i = data.find( name );
        if ( i == data.end() ) throw name_not_found_exception();
        return i->second->description();
      }

    void add( const std::string& name,
              TemplateFactoryPart<ComponentT, ResultT>* part )
      {
        typename map_t::iterator i = data.find( name );
        if ( i != data.end() )
          delete i->second;
        data[name] = part;
      }
  };
}

#endif
