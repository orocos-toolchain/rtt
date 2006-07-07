/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  TemplateCommandFactory.hpp

                        TemplateCommandFactory.hpp -  description
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

#ifndef TEMPLATECOMMANDFACTORY_HPP
#define TEMPLATECOMMANDFACTORY_HPP

#include <boost/lexical_cast.hpp>
#include <boost/mem_fn.hpp>
#include <boost/bind.hpp>

#include "TemplateFactory.hpp"

#include <corelib/CommandInterface.hpp>
#include <corelib/ConditionInterface.hpp>
#include "CommandFactoryInterface.hpp"
#include "AsynchCommandDecorator.hpp"
#include "CommandDispatch.hpp"
#include "ConditionComposite.hpp"

/**
 * @file TemplateCommandFactory.hpp
 * This file contains the TemplateCommandFactory template, which
 * is a template class designed to reduce the amount of boilerplate
 * code necessary to add a CommandFactory to an OROCOS component.  If
 * you're not interested in the implementation details ( warning:
 * template code, may seem a bit messy ), then skip forward to the
 * documentation of the @ref newCommandFactory function.
 */
namespace ORO_Execution
{
    using ORO_CoreLib::ConditionInterface;
    using ORO_CoreLib::CommandInterface;

    namespace detail {

  /**
   * A command that simply calls the functor it was passed every
   * time.  Very useful for e.g. components that want a command to
   * call one of their members every time. They can simply use:
   * @verbatim
    newFunctorCommand( boost::bind( &Component::memberfun, this, args ) );
   * @endverbatim
   * or something similar.
   */
  template<typename FunctorT>
  class FunctorCommand0
    : public CommandInterface
  {
    FunctorT fun;
  public:
    FunctorCommand0( FunctorT f )
      : fun( f )
       {
      }

    void readArguments()
    {}

    bool execute()
      {
        return fun();
      }

      virtual CommandInterface* clone() const
      {
          return new FunctorCommand0( *this );
      }
  };

  template<typename CompT, typename FunctorT>
  class FunctorCommandDS
    : public CommandInterface
  {
      FunctorT fun;
      typedef boost::weak_ptr<CompT> CompW;
      typedef boost::shared_ptr<CompT> CompS;
      typename DataSource<CompW>::shared_ptr ds;
  public:
      FunctorCommandDS( DataSource<CompW>* c, FunctorT f )
          : fun( f ), ds(c)
      {
      }

    void readArguments()
    {}

    bool execute()
      {
          // the Component pointer is stored in a DataSource
          CompS c = ds->get().lock();
          if (c) {
              CompT* ct = c.get();
              return fun( ct );
          } else {
              return false; // destroyed.
          }
      }

      virtual CommandInterface* clone() const
      {
          return new FunctorCommandDS( ds.get(), fun );
      }

      virtual CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorCommandDS( ds->copy( alreadyCloned ), fun );
      }
  };

  /**
   * A functor command that stores a function needing one argument,
   * and one DataSource to get the data from.
   */
  template<typename FunctorT, typename Arg1T>
  class FunctorCommand1
    : public CommandInterface
  {
      FunctorT fun;
      typename DataSource<Arg1T>::shared_ptr aa;
      
  public:
    FunctorCommand1( FunctorT f, DataSource<Arg1T>* a )
      : fun( f ), aa( a )
      {
      }

    void readArguments()
      {
          aa->evaluate();
      }
      
    bool execute()
      {
          Arg1T a = aa->value();
          return fun( a );
      }

    void reset()
      {
        aa->reset();
      }

    virtual CommandInterface* clone() const
      {
          return new FunctorCommand1( fun, aa.get() );
      }

    virtual CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorCommand1( fun, aa->copy( alreadyCloned ) );
      }
  };

  /**
   * A functor DataSource command that stores a function needing one argument,
   * and one DataSource to get the data from.
   * @todo Maybe we can replace the FunctorCommandDS'N' classes with the original FunctorCommandDS'N+1',
   * using some clever reuse of mem_fun, since this class is technically a copy of FunctorCommand2. A
   * change in FunctorCommandGenerator might be enough to pull the trick.
   */
  template<typename CompT, typename FunctorT, typename Arg1T>
  class FunctorCommandDS1
    : public CommandInterface
  {
      FunctorT fun;
      typename DataSource<Arg1T>::shared_ptr aa;
      typename DataSource<boost::weak_ptr<CompT> >::shared_ptr ds;
  public:
      FunctorCommandDS1( DataSource<boost::weak_ptr<CompT> >* c, FunctorT f, DataSource<Arg1T>* a )
        : fun( f ), aa( a ), ds(c)
      {
      }

      void readArguments()
      {
          aa->evaluate();
      }
      
      bool execute()
      {
        boost::shared_ptr<CompT> c =  ds->get().lock();
        if (c){
            CompT* ct = c.get();
            Arg1T a = aa->value();
            bool r= fun( ct, a );
            return r;
        } else
            return false;
      }

      void reset()
      {
        aa->reset();
      }

      virtual CommandInterface* clone() const
      {
          return new FunctorCommandDS1( ds.get(), fun, aa.get() );
      }

      virtual CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
      {
          return new FunctorCommandDS1( ds->copy(alreadyCloned), fun, aa->copy( alreadyCloned ) );
      }
  };

  /**
   * A functor command that stores a function needing two arguments,
   * and two DataSources to get the data from.
   */
  template<typename FunctorT, typename Arg1T, typename Arg2T>
  class FunctorCommand2
    : public CommandInterface
  {
      FunctorT fun;
      typename DataSource<Arg1T>::shared_ptr aa;
      typename DataSource<Arg2T>::shared_ptr bb;
  public:
      FunctorCommand2( FunctorT f, DataSource<Arg1T>* a, DataSource<Arg2T>* b )
          : fun( f ), aa( a ), bb( b )
      {
      }

    void readArguments()
      {
          aa->evaluate();
          bb->evaluate();
      }
      
    bool execute()
      {
          Arg1T a = aa->value();
          Arg2T b = bb->value();
        bool r= fun( a, b );
        return r;
      };
    void reset()
      {
        aa->reset();
        bb->reset();
      }

    virtual CommandInterface* clone() const
      {
          return new FunctorCommand2( fun, aa.get(), bb.get() );
      }

    virtual CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorCommand2( fun, aa->copy( alreadyCloned ), bb->copy( alreadyCloned ) );
      }
  };

  /**
   * A functor command that stores a function needing three arguments,
   * and three DataSources to get the data from.
   */
  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T>
  class FunctorCommand3
    : public CommandInterface
  {
    FunctorT fun;
    typename DataSource<Arg1T>::shared_ptr aa;
    typename DataSource<Arg2T>::shared_ptr bb;
    typename DataSource<Arg3T>::shared_ptr cc;
  public:
    FunctorCommand3( FunctorT f, DataSource<Arg1T>* a, DataSource<Arg2T>* b, DataSource<Arg3T>* c )
        : fun( f ), aa( a ), bb( b ), cc( c )
      {
      };

    void readArguments()
      {
          aa->evaluate();
          bb->evaluate();
          cc->evaluate();
      }
      
    bool execute()
      {
          Arg1T a = aa->value();
          Arg2T b = bb->value();
          Arg3T c = cc->value();
          bool r= fun( a, b, c );
          return r;
      }

    void reset()
      {
        aa->reset();
        bb->reset();
        cc->reset();
      }

    virtual CommandInterface* clone() const
      {
          return new FunctorCommand3( fun, aa.get(), bb.get(), cc.get() );
      }

    virtual CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorCommand3( fun, aa->copy( alreadyCloned ), bb->copy( alreadyCloned ), cc->copy( alreadyCloned ) );
      }
  };

  /**
   * A functor command that stores a function needing four arguments,
   * and four DataSources to get the data from.
   */
  template<typename FunctorT, typename Arg1T, typename Arg2T,typename Arg3T, typename Arg4T>
  class FunctorCommand4
    : public CommandInterface
  {
    FunctorT fun;
    typename DataSource<Arg1T>::shared_ptr aa;
    typename DataSource<Arg2T>::shared_ptr bb;
    typename DataSource<Arg3T>::shared_ptr cc;
    typename DataSource<Arg4T>::shared_ptr dd;
  public:
    FunctorCommand4( FunctorT f, DataSource<Arg1T>* a, DataSource<Arg2T>* b, DataSource<Arg3T>* c, DataSource<Arg4T>* d )
        : fun( f ), aa( a ), bb( b ), cc( c ), dd(d)
      {
      };

    void readArguments()
      {
          aa->evaluate();
          bb->evaluate();
          cc->evaluate();
          dd->evaluate();
      }
      
    bool execute()
      {
          Arg1T a = aa->value();
          Arg2T b = bb->value();
          Arg3T c = cc->value();
          Arg4T d = dd->value();
        bool r= fun( a, b, c, d );
        return r;
      }

    void reset()
      {
        aa->reset();
        bb->reset();
        cc->reset();
        dd->reset();
      }

    virtual CommandInterface* clone() const
      {
          return new FunctorCommand4( fun, aa.get(), bb.get(), cc.get(), dd.get() );
      }

    virtual CommandInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorCommand4( fun, aa->copy( alreadyCloned ), bb->copy( alreadyCloned ), cc->copy( alreadyCloned ), dd->copy(alreadyCloned) );
      }
  };

  /**
   * @internal
   * @defgroup functorhelpers helper functions for the FunctorCommand classes above
   * @{
   */
  template<typename FunctorT>
  FunctorCommand0<FunctorT>*
  newFunctorCommand( FunctorT f )
  {
    return new FunctorCommand0<FunctorT>( f );
  };

  template<typename CompT, typename FunctorT>
  FunctorCommandDS<CompT, FunctorT>*
  newFunctorCommand( DataSource< boost::weak_ptr<CompT> >* c, FunctorT f )
  {
    return new FunctorCommandDS<CompT, FunctorT>(c, f );
  };

  template<typename CompT, typename FunctorT, typename Arg1T>
  FunctorCommandDS1<CompT, FunctorT, Arg1T>*
  newFunctorCommand( DataSource<boost::weak_ptr<CompT> >* c, FunctorT f, DataSource<Arg1T>* a )
  {
    return new FunctorCommandDS1<CompT, FunctorT, Arg1T>(c, f, a );
  };

  template<typename FunctorT, typename Arg1T>
  FunctorCommand1<FunctorT, Arg1T>*
  newFunctorCommand( FunctorT f, DataSource<Arg1T>* a )
  {
    return new FunctorCommand1<FunctorT, Arg1T>( f, a );
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T>
  FunctorCommand2<FunctorT, Arg1T, Arg2T>*
  newFunctorCommand( FunctorT f, DataSource<Arg1T>* a, DataSource<Arg2T>* b  )
  {
    return new FunctorCommand2<FunctorT, Arg1T, Arg2T>( f, a, b );
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T>
  FunctorCommand3<FunctorT, Arg1T, Arg2T, Arg3T>*
  newFunctorCommand( FunctorT f, DataSource<Arg1T>* a, DataSource<Arg2T>* b, DataSource<Arg3T>* c  )
  {
    return new FunctorCommand3<FunctorT, Arg1T, Arg2T, Arg3T>( f, a, b, c );
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
  FunctorCommand4<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T>*
  newFunctorCommand( FunctorT f, DataSource<Arg1T>* a, DataSource<Arg2T>* b, DataSource<Arg3T>* c, DataSource<Arg4T>* d )
  {
    return new FunctorCommand4<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T>( f, a, b, c, d );
  };
  /**
   * @}
   */

  /**
   * This basically is a functor returning a ComCon.  It stores two
   * other functors, one returning a CommandInterface*, and one
   * returning a ConditionInterface*, and combines them into a ComCon
   * in its operator().
   *
   * This class provides operator() overloads for 0 up to 3 args, even
   * though for each instantiation, only one of these overloads will
   * be used.  I could have put these in separate classes, but I was
   * too lazy ;)
   */
  template<typename CommandGeneratorT, typename ConditionGeneratorT>
  class ComConComposer
  {
    CommandGeneratorT comg;
    ConditionGeneratorT cong;
  public:
      typedef bool result_type;
    ComConComposer( CommandGeneratorT com, ConditionGeneratorT con )
      : comg( com ), cong( con )
      {
      };
    template<typename ComponentT>
    ComCon operator()( ComponentT* comp ) const
      {
        return std::make_pair( comg( comp ), cong( comp ) );
      };

    template<typename ComponentT, typename Arg1T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a ) const
      {
        return std::make_pair( comg( comp, a ), cong( comp, a ) );
      };

    template<typename ComponentT, typename Arg1T, typename Arg2T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a,
                       DataSource<Arg2T>* b ) const
      {
        return std::make_pair( comg( comp, a, b ),
                               cong( comp, a, b ) );
      };
    template<typename ComponentT, typename Arg1T, typename Arg2T, typename Arg3T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a,
                       DataSource<Arg2T>* b,
                       DataSource<Arg3T>* c ) const
      {
        return std::make_pair( comg( comp, a, b, c ),
                               cong( comp, a, b, c ) );
      };

    template<typename ComponentT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a,
                       DataSource<Arg2T>* b,
                       DataSource<Arg3T>* c,
                       DataSource<Arg4T>* d ) const
      {
        return std::make_pair( comg( comp, a, b, c, d ),
                               cong( comp, a, b, c, d ) );
      };
  };

  /**
   * Helper function for the ComConComposer class.
   */
  template<typename A, typename B>
  ComConComposer<A, B>
  comcon_composer( A a, B b )
  {
    return ComConComposer<A, B>( a, b );
  };

  template<typename CommandGeneratorT, typename ConditionGeneratorT>
  class ComConNullaryComposer
  {
    CommandGeneratorT comg;
    ConditionGeneratorT cong;
  public:
      typedef bool result_type;
    ComConNullaryComposer( CommandGeneratorT com, ConditionGeneratorT con )
      : comg( com ), cong( con )
      {
      }

      /**
       * Create DataSource-based ComCon generators (for Parser).
       */
    template<typename ComponentT, typename Arg1T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a ) const
      {
        return std::make_pair( comg( comp, a ), cong( comp ) );
      }

    template<typename ComponentT, typename Arg1T, typename Arg2T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a,
                       DataSource<Arg2T>* b ) const
      {
        return std::make_pair( comg( comp, a, b ),
                               cong( comp ) );
      }

    template<typename ComponentT, typename Arg1T, typename Arg2T, typename Arg3T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a,
                       DataSource<Arg2T>* b,
                       DataSource<Arg3T>* c ) const
      {
        return std::make_pair( comg( comp, a, b, c ),
                               cong( comp) );
      }

    template<typename ComponentT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a,
                       DataSource<Arg2T>* b,
                       DataSource<Arg3T>* c,
                       DataSource<Arg4T>* d ) const
      {
        return std::make_pair( comg( comp, a, b, c, d ),
                               cong( comp) );
      }
  };

  /**
   * Helper function for the ComNullaryConComposer class.
   */
  template<typename A, typename B>
  ComConNullaryComposer<A, B>
  comcon_nullary_composer( A a, B b )
  {
    return ComConNullaryComposer<A, B>( a, b );
  };

  template<typename CommandGeneratorT, typename ConditionGeneratorT>
  class ComConUnaryComposer
  {
    CommandGeneratorT comg;
    ConditionGeneratorT cong;
  public:
      typedef bool result_type;
    ComConUnaryComposer( CommandGeneratorT com, ConditionGeneratorT con )
      : comg( com ), cong( con )
      {
      };

    template<typename ComponentT, typename Arg1T, typename Arg2T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a,
                       DataSource<Arg2T>* b ) const
      {
        return std::make_pair( comg( comp, a, b ),
                               cong( comp, a ) );
      };

    template<typename ComponentT, typename Arg1T, typename Arg2T, typename Arg3T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a,
                       DataSource<Arg2T>* b,
                       DataSource<Arg3T>* c ) const
      {
        return std::make_pair( comg( comp, a, b, c ),
                               cong( comp, a ) );
      };

    template<typename ComponentT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a,
                       DataSource<Arg2T>* b,
                       DataSource<Arg3T>* c,
                       DataSource<Arg4T>* d ) const
      {
        return std::make_pair( comg( comp, a, b, c, d ),
                               cong( comp, a ) );
      };
  };

  /**
   * Helper function for the ComUnaryConComposer class.
   */
  template<typename A, typename B>
  ComConUnaryComposer<A, B>
  comcon_unary_composer( A a, B b )
  {
    return ComConUnaryComposer<A, B>( a, b );
  };

  /**
   * A functor that you construct with a function, and that will
   * always return a Command calling that functor in its operator()
   * method.  As with ComConComposer, we provide overloads for
   * different number of arguments, even though for each
   * instantiation, only one of these overloads will be used.
   */
  template<typename FunctorT>
  class FunctorCommandGenerator
  {
    FunctorT f;
  public:
    FunctorCommandGenerator( FunctorT fun )
      : f( fun )
      {
      };
      /**
       * No arguments Generators.
       */
    template<typename ComponentT>
    CommandInterface* operator()( ComponentT* c ) const
      {
        return newFunctorCommand( boost::bind( f, c ) );
      };

    template<typename ComponentT >
    CommandInterface* operator()( DataSource<boost::weak_ptr<ComponentT> >* c ) const
      {
        return newFunctorCommand( c, f );
      };

      /**
       * DataSource arguments Generators.
       */
    template<typename ComponentT, typename Arg1T >
    CommandInterface* operator()( DataSource<boost::weak_ptr<ComponentT> >* c, DataSource<Arg1T>* a ) const
      {
        return newFunctorCommand( c, f, a );
      };
    template<typename ComponentT, typename Arg1T>
    CommandInterface* operator()( ComponentT* c,
                                  DataSource<Arg1T>* a
      ) const
      {
        return newFunctorCommand( boost::bind( f, c, _1 ), a );
      };

    template<typename ComponentT, typename Arg1T, typename Arg2T>
    CommandInterface* operator()( ComponentT* c,
                                  DataSource<Arg1T>* a, DataSource<Arg2T>* b
      ) const
      {
          //by ps: We use _1 and _2 so that both arguments are not bound yet.
          // we need to evaluate lateron, otherwise it would evaluate the DS now !
          // I really see no other way (not even overriding the Arg1T() operator)
        return newFunctorCommand( boost::bind( f, c, _1, _2 ), a, b );
      };
    template<typename ComponentT, typename Arg1T, typename Arg2T, typename Arg3T>
    CommandInterface* operator()( ComponentT* c,
                                  DataSource<Arg1T>* a, DataSource<Arg2T>* b, DataSource<Arg3T>* d
      ) const
      {
        return newFunctorCommand( boost::bind( f, c, _1, _2, _3 ), a, b, d );
      };

    template<typename ComponentT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
    CommandInterface* operator()( ComponentT* c,
                                  DataSource<Arg1T>* a, DataSource<Arg2T>* b, DataSource<Arg3T>* d, DataSource<Arg4T>* e
      ) const
      {
        return newFunctorCommand( boost::bind( f, c, _1, _2, _3, _4 ), a, b, d, e );
      };

  };

  /**
   * Helper function for the FunctorCommandGenerator class.
   */
  template<typename FunctorT>
  FunctorCommandGenerator<FunctorT>
  functor_command_generator( FunctorT fun )
  {
    return FunctorCommandGenerator<FunctorT>( fun );
  };

  /**
   * A condition that calls a boolean functor each time in its
   * evaluate method.
   * @todo replace this with a boolean FunctorDataSource, wrapped in
   * a standard ConditionWrapper such as the one in
   * interfacing/src/ConditionBoolDataSource
   */
  template<typename FunctorT>
  class FunctorCondition0
    : public ConditionInterface
  {
    FunctorT fun;
      bool invert;
  public:
    FunctorCondition0( FunctorT f, bool _invert )
        : fun( f ), invert(_invert)
      {
      }
    bool evaluate()
      {
          // logical XOR :
        return fun() != invert;
      }
    ConditionInterface* clone() const
      {
        return new FunctorCondition0( fun, invert );
      }
  };

    /**
     * Special case : the Component is given in a DataSource.
     */
  template<typename CompT, typename FunctorT>
  class FunctorConditionDS
    : public ConditionInterface
  {
      FunctorT fun;
      typename DataSource< boost::weak_ptr<CompT> >::shared_ptr ds;
      bool invert;
  public:
    FunctorConditionDS( DataSource<boost::weak_ptr<CompT> >* c, FunctorT f, bool _invert )
        : fun( f ), ds(c), invert(_invert)
      {
      }
    bool evaluate()
      {
          // logical XOR :
          boost::shared_ptr<CompT> c = ds->get().lock();
          if (c){
              CompT* ct = c.get();
              return fun( ct ) != invert;
          } else
              return false;
      }
    ConditionInterface* clone() const
      {
        return new FunctorConditionDS( ds.get(), fun, invert );
      }

    ConditionInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorConditionDS( ds->copy(alreadyCloned), fun, invert );
      }
  };

    /**
     * Special case : the Component is given in a DataSource.
     */
  template<typename CompT, typename FunctorT, typename Arg1T>
  class FunctorConditionDS1
    : public ConditionInterface
  {
      FunctorT fun;
      typename DataSource<boost::weak_ptr<CompT> >::shared_ptr ds;
      typename DataSource<Arg1T>::shared_ptr aa;
      bool invert;
  public:
    FunctorConditionDS1( DataSource<boost::weak_ptr<CompT> >* c, FunctorT f, DataSource<Arg1T>* a, bool _invert )
        : fun( f ), ds(c), aa(a), invert(_invert)
      {
      }
    bool evaluate()
      {
          Arg1T a =  aa->value();
          boost::shared_ptr<CompT> c = ds->get().lock();
          // logical XOR :
          if (c) {
              CompT* ct = c.get();
              bool r= fun( ct, a ) != invert;
              aa->updated();
              return r;
          } else
              return false;
      }
    ConditionInterface* clone() const
      {
        return new FunctorConditionDS1( ds.get(), fun, aa.get(), invert );
      }

    ConditionInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorConditionDS1( ds->copy(alreadyCloned), fun, aa->copy(alreadyCloned), invert );
      }
  };

  /**
   * An extension of FunctorCondition0 to unary functions.  This
   * class stores a pointer to a DataSource, where it will get the
   * argument for the function from.
   */
  template<typename FunctorT, typename Arg1T>
  class FunctorCondition1
    : public ConditionInterface
  {
    FunctorT fun;
    typename DataSource<Arg1T>::shared_ptr aa;
      bool invert;
  public:
    FunctorCondition1( FunctorT f, DataSource<Arg1T>* a, bool _invert )
        : fun( f ), aa( a ), invert(_invert)
      {
      };
    bool evaluate()
      {
        Arg1T a = aa->value();
        bool res = fun( a ) != invert;
        aa->updated();
        return res;
      };
    ConditionInterface* clone() const
      {
        return new FunctorCondition1( fun, aa.get(), invert );
      };
    ConditionInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorCondition1( fun, aa->copy( alreadyCloned ), invert );
      }
  };

  /**
   * An extension of FunctorCondition0 to binary functions.  This
   * class stores pointers to two DataSources, where it will get the
   * arguments for the function from.
   */
  template<typename FunctorT, typename Arg1T, typename Arg2T>
  class FunctorCondition2
    : public ConditionInterface
  {
    FunctorT fun;
    typename DataSource<Arg1T>::shared_ptr aa;
    typename DataSource<Arg2T>::shared_ptr bb;
      bool invert;
  public:
    FunctorCondition2( FunctorT f, DataSource<Arg1T>* a , DataSource<Arg2T>* b, bool _invert )
        : fun( f ), aa( a ), bb( b ), invert(_invert)
      {
      };
    bool evaluate()
      {
        Arg1T a = aa->value();
        Arg2T b = bb->value();
        bool r= fun( a, b ) != invert;
        aa->updated();
        bb->updated();
        return r;
      };
    ConditionInterface* clone() const
      {
        return new FunctorCondition2( fun, aa.get(), bb.get(), invert );
      };
    ConditionInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorCondition2( fun, aa->copy( alreadyCloned ), bb->copy( alreadyCloned ), invert );
      }
  };

  /**
   * An extension of FunctorCondition0 to ternary functions.  This
   * class stores pointers to three DataSources, where it will get the
   * arguments for the function from.
   */
  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T>
  class FunctorCondition3
    : public ConditionInterface
  {
    FunctorT fun;
    typename DataSource<Arg1T>::shared_ptr aa;
    typename DataSource<Arg2T>::shared_ptr bb;
    typename DataSource<Arg3T>::shared_ptr cc;
      bool invert;
  public:
    FunctorCondition3( FunctorT f, DataSource<Arg1T>* a , DataSource<Arg2T>* b, DataSource<Arg3T>*c, bool _invert )
        : fun( f ), aa( a ), bb( b ), cc( c ), invert(_invert)
      {
      }
    bool evaluate()
      {
        Arg1T a = aa->value();
        Arg2T b = bb->value();
        Arg3T c = cc->value();
        bool r= fun( a, b, c ) != invert;
        aa->updated();
        bb->updated();
        cc->updated();
        return r;
      }
    ConditionInterface* clone() const
      {
        return new FunctorCondition3( fun, aa.get(), bb.get(), cc.get(), invert );
      }
    ConditionInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorCondition3( fun, aa->copy( alreadyCloned ), bb->copy( alreadyCloned ), cc->copy( alreadyCloned ), invert );
      }
  };

  /**
   * An extension of FunctorCondition0 to quad-arity functions.  This
   * class stores pointers to three DataSources, where it will get the
   * arguments for the function from.
   */
  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
  class FunctorCondition4
    : public ConditionInterface
  {
    FunctorT fun;
    typename DataSource<Arg1T>::shared_ptr aa;
    typename DataSource<Arg2T>::shared_ptr bb;
    typename DataSource<Arg3T>::shared_ptr cc;
    typename DataSource<Arg4T>::shared_ptr dd;
      bool invert;
  public:
    FunctorCondition4( FunctorT f, DataSource<Arg1T>* a , DataSource<Arg2T>* b, DataSource<Arg3T>*c, DataSource<Arg4T>* d, bool _invert )
        : fun( f ), aa( a ), bb( b ), cc( c ), dd(d), invert(_invert)
      {
      }
    bool evaluate()
      {
        Arg1T a = aa->value();
        Arg2T b = bb->value();
        Arg3T c = cc->value();
        Arg3T d = dd->value();
        bool r= fun( a, b, c, d ) != invert;
        aa->updated();
        bb->updated();
        cc->updated();
        dd->updated();
        return r;
      }
    ConditionInterface* clone() const
      {
        return new FunctorCondition4( fun, aa.get(), bb.get(), cc.get(), dd.get(), invert );
      }
    ConditionInterface* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
      {
        return new FunctorCondition4( fun, aa->copy( alreadyCloned ), bb->copy( alreadyCloned ), cc->copy( alreadyCloned ), dd->copy(alreadyCloned), invert );
      }
  };

  /**
   * @internal
   * @defgroup condfunctorhelper Helper functions for the FunctorCondition classes.
   * @{
   */
  template<typename FunctorT>
  ConditionInterface* newFunctorCondition( FunctorT fun, bool _invert )
  {
    return new FunctorCondition0<FunctorT>( fun, _invert );
  };

  template<typename CompT, typename FunctorT>
  ConditionInterface* newFunctorCondition( DataSource<boost::weak_ptr<CompT> >* c, FunctorT fun, bool _invert )
  {
    return new FunctorConditionDS<CompT, FunctorT>(c, fun, _invert );
  };

  template<typename FunctorT, typename Arg1T>
  ConditionInterface* newFunctorCondition(
    FunctorT fun, DataSource<Arg1T>* a , bool _invert)
  {
    return new FunctorCondition1<FunctorT, Arg1T>( fun, a, _invert );
  };

  template<typename CompT, typename FunctorT, typename Arg1T>
  ConditionInterface* newFunctorCondition( DataSource<boost::weak_ptr<CompT> >* c, FunctorT fun, DataSource<Arg1T>* a, bool _invert )
  {
    return new FunctorConditionDS1<CompT, FunctorT, Arg1T>(c, fun, a,  _invert );
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T>
  ConditionInterface* newFunctorCondition(
    FunctorT fun, DataSource<Arg1T>* a, DataSource<Arg2T>* b , bool _invert)
  {
    return new FunctorCondition2<FunctorT, Arg1T, Arg2T>( fun, a, b, _invert );
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T>
  ConditionInterface* newFunctorCondition(
    FunctorT fun, DataSource<Arg1T>* a, DataSource<Arg2T>* b , DataSource<Arg3T>* c, bool _invert)
  {
    return new FunctorCondition3<FunctorT, Arg1T, Arg2T, Arg3T>( fun, a, b, c, _invert );
  };

  template<typename FunctorT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
  ConditionInterface* newFunctorCondition(
    FunctorT fun, DataSource<Arg1T>* a, DataSource<Arg2T>* b , DataSource<Arg3T>* c, DataSource<Arg4T>* d, bool _invert)
  {
    return new FunctorCondition4<FunctorT, Arg1T, Arg2T, Arg3T, Arg4T>( fun, a, b, c, d, _invert );
  };
  /**
   * @}
   */

  /**
   * A functor that takes a boolean functor in its constructor, and
   * returns a FunctorCondition calling that functor in its
   * operator() method.
   */
  template<typename FunctorT>
  class FunctorConditionGenerator
  {
    FunctorT fun;
      bool invert;
  public:
    FunctorConditionGenerator( FunctorT f, bool _invert )
        : fun( f ), invert(_invert)
      {
      };

      /**
       * No Arguments Generators.
       */
    template<typename ComponentT>
    ConditionInterface* operator()( ComponentT* comp ) const
      {
        return newFunctorCondition( boost::bind( fun, comp ), invert );
      };

    template<typename ComponentT>
    ConditionInterface* operator()( DataSource<boost::weak_ptr<ComponentT> >* comp ) const
      {
        return newFunctorCondition( comp, fun, invert );
      };

      /**
       * DataSource Arguments Generator.
       */
    template<typename ComponentT, typename Arg1T>
    ConditionInterface* operator()( DataSource<boost::weak_ptr<ComponentT> >* comp, DataSource<Arg1T>* a ) const
      {
        return newFunctorCondition( comp, fun, a, invert );
      };

    template<typename ComponentT, typename Arg1T>
    ConditionInterface* operator()(
      ComponentT* comp, DataSource<Arg1T>* a ) const
      {
        return newFunctorCondition( boost::bind( fun, comp, _1 ), a, invert );
      };

    template<typename ComponentT, typename Arg1T, typename Arg2T>
    ConditionInterface* operator()(
      ComponentT* comp, DataSource<Arg1T>* a, DataSource<Arg2T>* b ) const
      {
        return newFunctorCondition( boost::bind( fun, comp, _1, _2 ), a, b, invert );
      };

    template<typename ComponentT, typename Arg1T, typename Arg2T, typename Arg3T>
    ConditionInterface* operator()(
      ComponentT* comp, DataSource<Arg1T>* a, DataSource<Arg2T>* b, DataSource<Arg3T>* c ) const
      {
        return newFunctorCondition( boost::bind( fun, comp, _1, _2, _3 ), a, b, c, invert );
      };

    template<typename ComponentT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T>
    ConditionInterface* operator()(
      ComponentT* comp, DataSource<Arg1T>* a, DataSource<Arg2T>* b, DataSource<Arg3T>* c, DataSource<Arg4T>* d ) const
      {
        return newFunctorCondition( boost::bind( fun, comp, _1, _2, _3, _4 ), a, b, c, d, invert );
      };

  };

  /**
   * @internal
   * helper function for the FunctorConditionGenerator class
   */
  template<typename FunctorT>
  FunctorConditionGenerator<FunctorT>
  functor_condition_generator( FunctorT fun, bool _invert )
  {
    return FunctorConditionGenerator<FunctorT>( fun, _invert );
  };

    }
  /**
   * @defgroup commandFactories Command Part Factory Functions
   * @ingroup partFunctions
   * The command function is the function you should use to pass a
   * member function to the TemplateCommandFactory. You can
   * currently only use it with a member function taking a low number
   * of arguments ( current limit is three, but this can be improved
   * by writing some more overloads ).
   *
   * The first function you need to pass is a function that will be
   * called when the command is executed, and the second is a function
   * that will be called as the default termination condition.
   * The condition must have the same arguments as the command or
   * none (a Nullary condition) or the one argument which has the same
   * type as the first argument of the command.
   *
   * You should pass a description for the command as the third
   * argument, and a name and description for every argument of the
   * command as the following arguments.
   * @{
   */
  template<typename ComponentT>
  detail::TemplateFactoryPart<ComponentT, ComCon>*
  command( bool (ComponentT::*comf)(), bool (ComponentT::*conf)() const,
           const char* desc , bool _invert = false)
  {
    return detail::fun_fact<ComponentT, ComCon>(
      detail::comcon_composer(
        detail::functor_command_generator( boost::mem_fn( comf ) ),
        detail::functor_condition_generator( boost::mem_fn( conf ), _invert )
        ), desc );
  };

  // store the Component in a DataSource too !
  template<typename ComponentT>
  detail::TemplateFactoryPart< DataSource< boost::weak_ptr<ComponentT> >, ComCon>*
  command_ds( bool (ComponentT::*comf)(), bool (ComponentT::*conf)() const,
           const char* desc , bool _invert = false)
  {
    return detail::fun_fact_ds< ComponentT, ComCon>(
      detail::comcon_composer(
        detail::functor_command_generator( boost::mem_fn( comf ) ),
        detail::functor_condition_generator( boost::mem_fn( conf ), _invert )
        ), desc );
  };

  template<typename ComponentT, typename Arg1T>
  detail::TemplateFactoryPart< DataSource<boost::weak_ptr<ComponentT> >, ComCon>*
  command_ds( bool (ComponentT::*comf)(Arg1T),
              bool (ComponentT::*conf)(Arg1T) const,
              const char* desc , const char* arg1name, const char* arg1desc, bool _invert = false)
  {
    return detail::fun_fact_ds< ComponentT, ComCon, typename detail::ArgType<Arg1T>::type>(
      detail::comcon_composer(
        detail::functor_command_generator( boost::mem_fn( comf ) ),
        detail::functor_condition_generator( boost::mem_fn( conf ), _invert )
        ), desc, arg1name, arg1desc );
  };

  template<typename ComponentT, typename Arg1T>
  detail::TemplateFactoryPart< DataSource<boost::weak_ptr<ComponentT> >, ComCon>*
  command_ds( bool (ComponentT::*comf)(Arg1T),
              bool (ComponentT::*conf)() const,
              const char* desc , const char* arg1name, const char* arg1desc, bool _invert = false)
  {
    return detail::fun_fact_ds< ComponentT, ComCon, typename detail::ArgType<Arg1T>::type>(
      detail::comcon_nullary_composer(
        detail::functor_command_generator( boost::mem_fn( comf ) ),
        detail::functor_condition_generator( boost::mem_fn( conf ), _invert )
        ), desc, arg1name, arg1desc );
  };

    // extra functor tryout.
//   template<typename ComponentT, typename ResT, typename F>
//   detail::TemplateFactoryPart<ComponentT, ComCon>*
//   command( ResT (ComponentT::*comf)(), F conf,
//            const char* desc )
//   {
//     return detail::fun_fact<ComponentT, ComCon>(
//       detail::comcon_composer(
//         detail::functor_command_generator( boost::mem_fn( comf ) ),
//         detail::functor_condition_generator( conf )
//         ), desc );
//   };

  template<typename ComponentT, typename Arg1T>
  detail::TemplateFactoryPart<ComponentT, ComCon>*
  command( bool (ComponentT::*comf)( Arg1T ),
           bool (ComponentT::*conf)( Arg1T ) const,
           const char* desc, const char* arg1name, const char* arg1desc, bool invert = false )
  {
    return detail::fun_fact<ComponentT,
      ComCon, typename detail::ArgType<Arg1T>::type>(
        detail::comcon_composer(
          detail::functor_command_generator( boost::mem_fn( comf ) ),
          detail::functor_condition_generator( boost::mem_fn( conf ), invert )
          ), desc, arg1name, arg1desc );
  };

    // same as above, but with nullary terminationcondition checker
  template<typename ComponentT, typename Arg1T>
  detail::TemplateFactoryPart<ComponentT, ComCon>*
  command( bool (ComponentT::*comf)( Arg1T ),
           bool (ComponentT::*conf)() const,
           const char* desc, const char* arg1name, const char* arg1desc, bool invert = false )
  {
    return detail::fun_fact<ComponentT,
      ComCon, typename detail::ArgType<Arg1T>::type>(
        detail::comcon_nullary_composer(
          detail::functor_command_generator( boost::mem_fn( comf ) ),
          detail::functor_condition_generator( boost::mem_fn( conf ), invert )
          ), desc, arg1name, arg1desc );
  };

  template<typename ComponentT, typename Arg1T, typename Arg2T>
  detail::TemplateFactoryPart<ComponentT, ComCon>*
  command( bool (ComponentT::*comf)( Arg1T, Arg2T ),
           bool (ComponentT::*conf)( Arg1T, Arg2T ) const,
           const char* desc, const char* arg1name, const char* arg1desc,
           const char* arg2name, const char* arg2desc, bool invert = false )
  {
    return detail::fun_fact<ComponentT, ComCon,
      typename detail::ArgType<Arg1T>::type,
      typename detail::ArgType<Arg2T>::type>(
        detail::comcon_composer( detail::functor_command_generator( boost::mem_fn( comf ) ),
                         detail::functor_condition_generator( boost::mem_fn( conf ), invert )
          ), desc, arg1name, arg1desc, arg2name, arg2desc );
  };

    // same as above, but with nullary terminationcondition checker
  template<typename ComponentT, typename Arg1T, typename Arg2T>
  detail::TemplateFactoryPart<ComponentT, ComCon>*
  command( bool (ComponentT::*comf)( Arg1T, Arg2T ),
           bool (ComponentT::*conf)() const,
           const char* desc, const char* arg1name, const char* arg1desc,
           const char* arg2name, const char* arg2desc, bool invert = false )
  {
    return detail::fun_fact<ComponentT, ComCon,
      typename detail::ArgType<Arg1T>::type,
      typename detail::ArgType<Arg2T>::type>(
        detail::comcon_nullary_composer( detail::functor_command_generator( boost::mem_fn( comf ) ),
                         detail::functor_condition_generator( boost::mem_fn( conf ), invert )
          ), desc, arg1name, arg1desc, arg2name, arg2desc );
  };

    // same as above, but with unary terminationcondition checker
  template<typename ComponentT, typename Arg1T, typename Arg2T>
  detail::TemplateFactoryPart<ComponentT, ComCon>*
  command( bool (ComponentT::*comf)( Arg1T, Arg2T ),
           bool (ComponentT::*conf)( Arg1T ) const,
           const char* desc, const char* arg1name, const char* arg1desc,
           const char* arg2name, const char* arg2desc, bool invert = false )
  {
    return detail::fun_fact<ComponentT, ComCon,
      typename detail::ArgType<Arg1T>::type,
      typename detail::ArgType<Arg2T>::type>(
        detail::comcon_unary_composer( detail::functor_command_generator( boost::mem_fn( comf ) ),
                         detail::functor_condition_generator( boost::mem_fn( conf ), invert )
          ), desc, arg1name, arg1desc, arg2name, arg2desc );
  };

  template<typename ComponentT, typename Arg1T, typename Arg2T,
           typename Arg3T>
  detail::TemplateFactoryPart<ComponentT, ComCon>*
  command( bool (ComponentT::*comf)( Arg1T, Arg2T, Arg3T ),
           bool (ComponentT::*conf)( Arg1T, Arg2T, Arg3T ) const,
           const char* desc, const char* arg1name, const char* arg1desc,
           const char* arg2name, const char* arg2desc,
           const char* arg3name, const char* arg3desc, bool invert = false )
  {
    return detail::fun_fact<ComponentT, ComCon,
      typename detail::ArgType<Arg1T>::type,
      typename detail::ArgType<Arg2T>::type,
      typename detail::ArgType<Arg3T>::type >(
        detail::comcon_composer(
          detail::functor_command_generator( boost::mem_fn( comf ) ),
          detail::functor_condition_generator( boost::mem_fn( conf ), invert )
          ), desc, arg1name, arg1desc, arg2name,
        arg2desc, arg3name, arg3desc );
  };

    // Nullary Condition Checking.
  template<typename ComponentT,  typename Arg1T, typename Arg2T,
           typename Arg3T>
  detail::TemplateFactoryPart<ComponentT, ComCon>*
  command( bool (ComponentT::*comf)( Arg1T, Arg2T, Arg3T ),
           bool (ComponentT::*conf)( ) const,
           const char* desc, const char* arg1name, const char* arg1desc,
           const char* arg2name, const char* arg2desc,
           const char* arg3name, const char* arg3desc, bool invert = false )
  {
    return detail::fun_fact<ComponentT, ComCon,
      typename detail::ArgType<Arg1T>::type,
      typename detail::ArgType<Arg2T>::type,
      typename detail::ArgType<Arg3T>::type >(
        detail::comcon_nullary_composer(
          detail::functor_command_generator( boost::mem_fn( comf ) ),
          detail::functor_condition_generator( boost::mem_fn( conf ), invert )
          ), desc, arg1name, arg1desc, arg2name,
        arg2desc, arg3name, arg3desc );
  };
    // Unary Condition Checking.
  template<typename ComponentT,  typename Arg1T, typename Arg2T,
           typename Arg3T>
  detail::TemplateFactoryPart<ComponentT, ComCon>*
  command( bool (ComponentT::*comf)( Arg1T, Arg2T, Arg3T ),
           bool (ComponentT::*conf)( Arg1T ) const,
           const char* desc, const char* arg1name, const char* arg1desc,
           const char* arg2name, const char* arg2desc,
           const char* arg3name, const char* arg3desc, bool invert = false )
  {
    return detail::fun_fact<ComponentT, ComCon,
      typename detail::ArgType<Arg1T>::type,
      typename detail::ArgType<Arg2T>::type,
      typename detail::ArgType<Arg3T>::type >(
        detail::comcon_unary_composer(
          detail::functor_command_generator( boost::mem_fn( comf ) ),
          detail::functor_condition_generator( boost::mem_fn( conf ), invert )
          ), desc, arg1name, arg1desc, arg2name,
        arg2desc, arg3name, arg3desc );
  };

    // 4 Arguments commands:
  template<typename ComponentT,
           typename Arg1T, typename Arg2T,
           typename Arg3T, typename Arg4T>
  detail::TemplateFactoryPart<ComponentT, ComCon>*
  command( bool (ComponentT::*comf)( Arg1T, Arg2T, Arg3T, Arg4T ),
           bool (ComponentT::*conf)( Arg1T, Arg2T, Arg3T, Arg4T ) const,
           const char* desc, const char* arg1name, const char* arg1desc,
           const char* arg2name, const char* arg2desc,
           const char* arg3name, const char* arg3desc,
           const char* arg4name, const char* arg4desc, bool invert = false )
  {
      return detail::fun_fact<ComponentT, ComCon,
          typename detail::ArgType<Arg1T>::type,
          typename detail::ArgType<Arg2T>::type,
          typename detail::ArgType<Arg3T>::type,
          typename detail::ArgType<Arg4T>::type >(
        detail::comcon_composer(
          detail::functor_command_generator( boost::mem_fn( comf ) ),
          detail::functor_condition_generator( boost::mem_fn( conf ), invert )
          ), desc, arg1name, arg1desc, arg2name,
        arg2desc, arg3name, arg3desc, arg4name, arg4desc );
  };

    // Nullary Condition Checking.
  template<typename ComponentT,  typename Arg1T, typename Arg2T,
           typename Arg3T, typename Arg4T>
  detail::TemplateFactoryPart<ComponentT, ComCon>*
  command( bool (ComponentT::*comf)( Arg1T, Arg2T, Arg3T, Arg4T ),
           bool (ComponentT::*conf)( ) const,
           const char* desc, const char* arg1name, const char* arg1desc,
           const char* arg2name, const char* arg2desc,
           const char* arg3name, const char* arg3desc,
           const char* arg4name, const char* arg4desc,bool invert = false )
  {
    return detail::fun_fact<ComponentT, ComCon,
        typename detail::ArgType<Arg1T>::type,
        typename detail::ArgType<Arg2T>::type,
        typename detail::ArgType<Arg3T>::type,
        typename detail::ArgType<Arg4T>::type >(
        detail::comcon_nullary_composer(
          detail::functor_command_generator( boost::mem_fn( comf ) ),
          detail::functor_condition_generator( boost::mem_fn( conf ), invert )
          ), desc, arg1name, arg1desc, arg2name,
        arg2desc, arg3name, arg3desc, arg4name, arg4desc );
  };
    // Unary Condition Checking.
  template<typename ComponentT,  typename Arg1T, typename Arg2T,
           typename Arg3T, typename Arg4T>
  detail::TemplateFactoryPart<ComponentT, ComCon>*
  command( bool (ComponentT::*comf)( Arg1T, Arg2T, Arg3T, Arg4T ),
           bool (ComponentT::*conf)( Arg1T ) const,
           const char* desc, const char* arg1name, const char* arg1desc,
           const char* arg2name, const char* arg2desc,
           const char* arg3name, const char* arg3desc,
           const char* arg4name, const char* arg4desc, bool invert = false )
  {
    return detail::fun_fact<ComponentT, ComCon,
      typename detail::ArgType<Arg1T>::type,
      typename detail::ArgType<Arg2T>::type,
      typename detail::ArgType<Arg3T>::type,
      typename detail::ArgType<Arg4T>::type >(
        detail::comcon_unary_composer(
          detail::functor_command_generator( boost::mem_fn( comf ) ),
          detail::functor_condition_generator( boost::mem_fn( conf ), invert )
          ), desc, arg1name, arg1desc, arg2name,
        arg2desc, arg3name, arg3desc, arg3name, arg4desc );
  };
  /**
   * @}
   */

  /**
   * TemplateCommandFactory is a template class, written to facilitate
   * the addition of a CommandFactory to a TaskContext.  It removes the
   * need to define a special class, and also takes care of the
   * parsing of the arguments.
   * @ingroup objectFactory
   *
   */
  template<typename ComponentT>
  class TemplateCommandFactory
    : public CommandFactoryInterface,
      private TemplateFactory<ComponentT, ComCon>
  {
    typedef TemplateFactory<ComponentT, ComCon> _TF;
  public:
    TemplateCommandFactory( ComponentT* comp )
      : _TF( comp )
      {
      }

    std::vector<std::string> getCommandList() const
      {
        return _TF::getNames();
      }

    std::string getResultType( const std::string& com ) const
      {
        return _TF::getResultType( com );
      }

    bool hasCommand( const std::string& com ) const
      {
        return _TF::hasMember( com );
      }

      int getArity(const std::string& com ) const {
          return _TF::getArity( com );
      }

    std::string getDescription( const std::string& com ) const
      {
        return _TF::getDescription( com );
      }

    PropertyBag getArgumentSpec( const std::string& name ) const
      {
        return _TF::getArgumentSpec( name );
      }

    std::vector< ArgumentDescription > getArgumentList( const std::string& method ) const
      {
          return _TF::getArgumentList( method );
      }

    ComCon create( const std::string& name, const PropertyBag& args, bool nodispatch ) const
      {
        ComCon res =  _TF::produce( name, args );
        if (nodispatch)
            res.first = new AsynchCommandDecorator( res.first );
        else {
            // dispatch a Command to other processor, overthere, the result is ignored,
            // it is interpreted here, with the implcond. Other condition branches
            // must be guarded likewise with wrapCondition().
            // we compare processors, as dispatching is not done if the processor is shared.
            CommandDispatch* dcom = new CommandDispatch( this->mcp, res.first, res.second->clone() );
            res.first = dcom;
        }
        return res;
      }
    ComCon create( const std::string& name,
                   const std::vector<ORO_CoreLib::DataSourceBase*>& args, bool nodispatch ) const
      {
        ComCon res =  _TF::produce( name, args );
        if (nodispatch)
            res.first = new AsynchCommandDecorator( res.first );
        else {
            // dispatch a Command to other processor, overthere, the result is ignored,
            // it is interpreted here, with the implcond. Other condition branches
            // must be guarded likewise with wrapCondition().
            // we compare processors, as dispatching is not done if the processor is shared.
            CommandDispatch* dcom = new CommandDispatch( this->mcp, res.first, res.second->clone() );
            res.first = dcom;
        }
        return res;
      }

      ComCon create( const std::string& name,
                     const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args, bool nodispatch ) const
      {
          ComCon res =  _TF::produce( name, args );
          if (nodispatch)
              res.first = new AsynchCommandDecorator( res.first );
          else {
              // dispatch a Command to other processor, overthere, the result is ignored,
              // it is interpreted here, with the implcond.
              CommandDispatch* dcom = new CommandDispatch( mcp, res.first, res.second->clone() );
              res.first = dcom;
          }
          return res;
      }

    void add( const std::string& name,
              detail::TemplateFactoryPart<ComponentT, ComCon>* part )
      {
        _TF::add( name, part );
      }
  };

  /**
   * Helper function for constructing a TemplateCommandFactory.
   */
  template<typename ComponentT>
  TemplateCommandFactory<ComponentT>*
  newCommandFactory( ComponentT* comp )
  {
    return new TemplateCommandFactory<ComponentT>(
      comp );
  };

    /**
     * Specialisation where Component is stored in a DataSource
     */
  template<typename ComponentT>
  TemplateCommandFactory< DataSource<boost::weak_ptr<ComponentT> > >*
  newCommandFactory( DataSource<boost::weak_ptr<ComponentT> >* comp )
  {
    return new TemplateCommandFactory< DataSource<boost::weak_ptr<ComponentT> > >( comp );
  };
}

#endif
