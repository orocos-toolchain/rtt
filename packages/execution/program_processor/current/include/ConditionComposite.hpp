/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ConditionComposite.hpp

                        ConditionComposite.hpp -  description
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
      }

    virtual ConditionInterface* clone() const
      {
        return new ConditionBinaryComposite<op>( lhs->clone(), rhs->clone() );
      }

      ConditionBinaryComposite<op>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
          return new ConditionBinaryComposite<op>(lhs->copy(alreadyCloned),rhs->copy(alreadyCloned));
      }

    virtual ~ConditionBinaryComposite() {
      delete lhs;
      delete rhs;
    }

    virtual bool evaluate() {
      return o( lhs->evaluate(), rhs->evaluate() );
    }

    virtual void reset()
      {
        lhs->reset();
        rhs->reset();
      }
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
      }
    ~ConditionCompositeNOT();
    bool evaluate();
    ConditionCompositeNOT* clone() const;
    ConditionCompositeNOT* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    void reset();
  };
}

#endif
