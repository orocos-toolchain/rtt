#ifndef TEMPLATECOMMANDFACTORY_HPP
#define TEMPLATECOMMANDFACTORY_HPP

#include <boost/lexical_cast.hpp>
#include <boost/mem_fn.hpp>
#include <boost/bind.hpp>

#include "TemplateFactory.hpp"

#include <corelib/CommandInterface.hpp>
#include <corelib/ConditionInterface.hpp>
#include "CommandFactoryInterface.hpp"

/**
 * @file This file contains the TemplateCommandFactory template, which
 * is a template class designed to reduce the amount of boilerplate
 * code necessary to add a CommandFactory to an OROCOS component.  If
 * you're not interested in the implementation details ( warning:
 * template code, may seem a bit messy ), then skip forward to the
 * documentation of the @ref newTemplateCommandFactory function.
 */
namespace ORO_Execution
{
  using boost::mem_fn;
  using boost::bind;

  /**
   * A command that simply calls the functor it was passed every
   * time..  Very useful for e.g. components that want a command to
   * call one of their members every time..  They can simply use:
   * <pre>
   * newFunctorCommand( boost::bind( &Component::memberfun, this, args ) );
   * </pre>
   * or something similar..
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
      };
    void execute()
      {
        fun();
      };
    std::string toString()
      {
        return "FunctorCommand0";
      };
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
      };
    void execute()
      {
        Arg1T a = aa->get();
        fun( a );
      };
    std::string toString()
      {
        return "FunctorCommand1";
      };
    void reset()
      {
        aa->reset();
      }
  };

  /**
   * helper functions for the two FunctorCommand classes above
   * @{
   */
  template<typename FunctorT>
  FunctorCommand0<FunctorT>*
  newFunctorCommand( FunctorT f )
  {
    return new FunctorCommand0<FunctorT>( f );
  };

  template<typename FunctorT, typename Arg1T>
  FunctorCommand1<FunctorT, Arg1T>*
  newFunctorCommand( FunctorT f, DataSource<Arg1T>* a )
  {
    return new FunctorCommand1<FunctorT, Arg1T>( f, a );
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
   * This class provides operator() overloads for 0 up to 2 args, even
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
    ComCon operator()( ComponentT* comp, Arg1T a ) const
      {
        return std::make_pair( comg( comp, a ), cong( comp, a ) );
      };
    template<typename ComponentT, typename Arg1T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a ) const
      {
        return std::make_pair( comg( comp, a ), cong( comp, a ) );
      };
    template<typename ComponentT, typename Arg1T, typename Arg2T>
    ComCon operator()( ComponentT* comp, Arg1T a, Arg2T b ) const
      {
        return std::make_pair( comg( comp, a, b ), cong( comp, a, b ) );
      };
    template<typename ComponentT, typename Arg1T, typename Arg2T>
    ComCon operator()( ComponentT* comp,
                       DataSource<Arg1T>* a,
                       DataSource<Arg2T>* b ) const
      {
        return std::make_pair( comg( comp, a, b ),
                               cong( comp, a, b ) );
      };
  };

  /**
   * Helper function for the ComConComposer class..
   */
  template<typename A, typename B>
  ComConComposer<A, B>
  comcon_composer( A a, B b )
  {
    return ComConComposer<A, B>( a, b );
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
    template<typename ComponentT>
    CommandInterface* operator()( ComponentT* c ) const
      {
        return newFunctorCommand( bind( f, c ) );
      };
    template<typename ComponentT, typename Arg1T>
    CommandInterface* operator()( ComponentT* c, Arg1T a ) const
      {
        return newFunctorCommand( bind( f, c, a ) );
      };
    template<typename ComponentT, typename Arg1T>
    CommandInterface* operator()( ComponentT* c,
                                  DataSource<Arg1T>* a
      ) const
      {
        return newFunctorCommand( bind( f, c, _1 ), a );
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
   * evaluate method..
   * @todo replace this with a boolean FunctorDataSource, wrapped in
   * a standard ConditionWrapper such as the one in
   * interfacing/src/ConditionBoolDataSource
   */
  template<typename FunctorT>
  class FunctorCondition0
    : public ConditionInterface
  {
    FunctorT fun;
  public:
    FunctorCondition0( FunctorT f )
      : fun( f )
      {
      };
    bool evaluate()
      {
        return fun();
      };
    ConditionInterface* clone() const
      {
        return new FunctorCondition0( fun );
      };
  };

  /**
   * An extension of FunctorCondition0 to unary functions.  This
   * class stores a pointer to a DataSource, where it will get the
   * argument for the function from..
   */
  template<typename FunctorT, typename Arg1T>
  class FunctorCondition1
    : public ConditionInterface
  {
    FunctorT fun;
    typename DataSource<Arg1T>::shared_ptr aa;
  public:
    FunctorCondition1( FunctorT f, DataSource<Arg1T>* a )
      : fun( f ), aa( a )
      {
      };
    bool evaluate()
      {
        Arg1T a = aa->get();
        return fun( a );
      };
    ConditionInterface* clone() const
      {
        return new FunctorCondition1( fun, aa.get() );
      };
  };

  /**
   * Helper functions for the FunctorCondition classes..
   * @{
   */
  template<typename FunctorT>
  ConditionInterface* newFunctorCondition( FunctorT fun )
  {
    return new FunctorCondition0<FunctorT>( fun );
  };

  template<typename FunctorT, typename Arg1T>
  ConditionInterface* newFunctorCondition(
    FunctorT fun, DataSource<Arg1T>* a )
  {
    return new FunctorCondition1<FunctorT, Arg1T>( fun, a );
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
  public:
    FunctorConditionGenerator( FunctorT f )
      : fun( f )
      {
      };
    template<typename ComponentT>
    ConditionInterface* operator()( ComponentT* comp ) const
      {
        return newFunctorCondition( boost::bind( fun, comp ) );
      };
    template<typename ComponentT, typename Arg1T>
    ConditionInterface* operator()( ComponentT* comp, Arg1T a ) const
      {
        return newFunctorCondition( boost::bind( fun, comp, a ) );
      };
    template<typename ComponentT, typename Arg1T>
    ConditionInterface* operator()(
      ComponentT* comp, DataSource<Arg1T>* a ) const
      {
        return newFunctorCondition( boost::bind( fun, comp, _1 ), a );
      };
  };

  /**
   * helper function for the FunctorConditionGenerator class
   */
  template<typename FunctorT>
  FunctorConditionGenerator<FunctorT>
  functor_condition_generator( FunctorT fun )
  {
    return FunctorConditionGenerator<FunctorT>( fun );
  };

  /**
   * The command function is the function you should use to pass a
   * member function to the TemplateCommandFactory..  You can
   * currently only use it with a member function taking a low number
   * of arguments ( current limit is three, but this can be improved
   * by writing some more overloads )..
   *
   * The first function you need to pass is a function that will be
   * called when the command is executed, and the second is a function
   * that will be called as the default termination condition..
   *
   * You should pass a description for the command as the third
   * argument, and a name and description for every argument of the
   * command as the following arguments..
   *
   * @{
   */
  template<typename ComponentT>
  TemplateFactoryPart<typename boost::remove_const<ComponentT>::type, ComCon>*
  command( void (ComponentT::*comf)(), bool (ComponentT::*conf)(),
           const char* desc )
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type, ComCon>(
      comcon_composer(
        functor_command_generator( boost::mem_fn( comf ) ),
        functor_condition_generator( boost::mem_fn( conf ) )
        ), desc );
  };

  template<typename ComponentT, typename Arg1T>
  TemplateFactoryPart<typename boost::remove_const<ComponentT>::type, ComCon>*
  command( void (ComponentT::*comf)( Arg1T ),
           bool (ComponentT::*conf)( Arg1T ),
           const char* desc, const char* arg1name, const char* arg1desc )
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type,
      ComCon, typename remove_cr<Arg1T>::type>(
        comcon_composer(
          functor_command_generator( boost::mem_fn( comf ) ),
          functor_condition_generator( boost::mem_fn( conf ) )
          ), desc, arg1name, arg1desc );
  };

  template<typename ComponentT, typename Arg1T, typename Arg2T>
  TemplateFactoryPart<typename boost::remove_const<ComponentT>::type, ComCon>*
  command( void (ComponentT::*comf)( Arg1T, Arg2T ),
           bool (ComponentT::*conf)( Arg1T, Arg2T ),
           const char* desc, const char* arg1name, const char* arg1desc,
           const char* arg2name, const char* arg2desc )
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type, ComCon,
      typename remove_cr<Arg1T>::type,
      typename remove_cr<Arg2T>::type>(
        comcon_composer( functor_command_generator( boost::mem_fn( comf ) ),
                         functor_condition_generator( boost::mem_fn( conf ) )
          ), desc, arg1name, arg1desc, arg2name, arg2desc );
  };

  template<typename ComponentT, typename Arg1T, typename Arg2T,
           typename Arg3T>
  TemplateFactoryPart<typename boost::remove_const<ComponentT>::type, ComCon>*
  command( void (ComponentT::*comf)( Arg1T, Arg2T, Arg3T ),
           bool (ComponentT::*conf)( Arg1T, Arg2T, Arg3T ),
           const char* desc, const char* arg1name, const char* arg1desc,
           const char* arg2name, const char* arg2desc,
           const char* arg3name, const char* arg3desc )
  {
    return fun_fact<typename boost::remove_const<ComponentT>::type, ComCon,
      typename remove_cr<Arg1T>::type,
      typename remove_cr<Arg2T>::type,
      typename remove_cr<Arg3T>::type >(
        comcon_composer(
          functor_command_generator( boost::mem_fn( comf ) ),
          functor_condition_generator( boost::mem_fn( conf ) )
          ), desc, arg1name, arg1desc, arg2name,
        arg2desc, arg3name, arg3descyes );
  };
  /**
   * @}
   */

  /**
   * TemplateCommandFactory is a template class, written to facilitate
   * the addition of a CommandFactory to a Component.  It removes the
   * need to define a special class, and also takes care of the
   * parsing of the arguments.
   *
   * For using this, please refer to the file orocos-parser.xml in
   * orocos-tree/parser/doc/
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
      };

    std::vector<std::string> commandNames() const
      {
        return _TF::getNames();
      };

    bool hasCommand( const std::string& com ) const
      {
        return _TF::hasName( com );
      };

    std::string getDescription( const std::string& com ) const
      {
        return _TF::getDescription( com );
      };

    PropertyBag getArgumentSpec( const std::string& name ) const
      {
        return _TF::getArgumentSpec( name );
      };

    ComCon create( const std::string& name, const PropertyBag& args ) const
      {
        return _TF::produce( name, args );
      };
    ComCon create( const std::string& name,
                   const std::vector<DataSourceBase*>& args ) const
      {
        return _TF::produce( name, args );
      };

    void add( const std::string& name,
              TemplateFactoryPart<ComponentT, ComCon>* part )
      {
        _TF::add( name, part );
      };
  };

  /**
   * Helper function for constructing a TemplateCommandFactory...
   */
  template<typename ComponentT>
  TemplateCommandFactory<ComponentT>*
  newTemplateCommandFactory( ComponentT* comp )
  {
    return new TemplateCommandFactory<ComponentT>(
      comp );
  };
}

#endif
