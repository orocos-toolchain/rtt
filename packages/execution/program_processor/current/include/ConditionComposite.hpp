#ifndef CONDITIONCOMPOSITE_HPP
#define CONDITIONCOMPOSITE_HPP

#include <functional>
#include <typeinfo>

#include <corelib/ConditionInterface.hpp>

namespace ORO_Execution {

  using namespace ORO_CoreLib;

  /**
   * Composite software pattern for conditions.  This is a general
   * class, allowing you to pass a binary predicate functor taking two
   * booleans as the compare operator.  Use things like
   * std::logical_or<bool>, std::logical_and<bool>,
   * std::not_equal_to<bool> to specify the "or", "and", and "xor"
   * operators..
   */
  template<typename op>
  class ConditionBinaryComposite
    : public ConditionInterface
  {
    ConditionInterface* lhs;
    ConditionInterface* rhs;
    op o;
  public:
    /**
     * constructor: deze klasse wordt eigenaar van l en r, die
     * gealloceerd moeten zijn geweest met new.
     */
    ConditionBinaryComposite( ConditionInterface* l, ConditionInterface* r )
      : lhs( l ), rhs( r ), o()
      {
      };

    ConditionInterface* clone() const
      {
        return new ConditionBinaryComposite<op>( lhs->clone(), rhs->clone() );
      };

    virtual ~ConditionBinaryComposite() {
      delete lhs;
      delete rhs;
    }

    virtual bool evaluate() {
      return o( lhs->evaluate(), rhs->evaluate() );
    }

    void reset()
      {
        lhs->reset();
        rhs->reset();
      };
  };

  typedef ConditionBinaryComposite<std::logical_or<bool> > ConditionCompositeOR;
  typedef ConditionBinaryComposite<std::logical_and<bool> > ConditionCompositeAND;

  class ConditionCompositeNOT
    : public ConditionInterface
  {
    ConditionInterface* cond;
  public:
    ConditionCompositeNOT( ConditionInterface* c )
      : cond( c )
      {
      };
    ~ConditionCompositeNOT();
    bool evaluate();
    ConditionInterface* clone() const;
    void reset();
  };
}

#endif
