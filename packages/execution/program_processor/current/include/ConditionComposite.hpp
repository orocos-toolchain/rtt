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

#include "ConditionInterface.hpp"

namespace RTT {

  /**
   * Compose an 'AND' function of two Conditions.
   */
  class ConditionBinaryCompositeAND
    : public ConditionInterface
  {
    ConditionInterface* lhs;
    ConditionInterface* rhs;
  public:
    /**
     * If \a l evaluates to true, evaluate \r and return the result, otherwise,
     * return false.
     * Constructor takes ownership of \a l and \a r.
     */
    ConditionBinaryCompositeAND( ConditionInterface* l, ConditionInterface* r )
      : lhs( l ), rhs( r )
      {
      }

    virtual ConditionInterface* clone() const
      {
        return new ConditionBinaryCompositeAND( lhs->clone(), rhs->clone() );
      }

      ConditionBinaryCompositeAND* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
          return new ConditionBinaryCompositeAND(lhs->copy(alreadyCloned),rhs->copy(alreadyCloned));
      }

    virtual ~ConditionBinaryCompositeAND() {
      delete lhs;
      delete rhs;
    }

    virtual bool evaluate() {
        if ( lhs->evaluate() )
            if ( rhs->evaluate() )
                return true;
        return false;
    }

    virtual void reset()
      {
        lhs->reset();
        rhs->reset();
      }
  };

    /*
     * Compose an 'OR' function of two Conditions.
     */
  class ConditionBinaryCompositeOR
    : public ConditionInterface
  {
    ConditionInterface* lhs;
    ConditionInterface* rhs;
  public:
    /**
     * If \a l evaluates to true, evaluate \r and return the result, otherwise,
     * return false.
     * Constructor takes ownership of \a l and \a r.
     */
    ConditionBinaryCompositeOR( ConditionInterface* l, ConditionInterface* r )
      : lhs( l ), rhs( r )
      {
      }

    virtual ConditionInterface* clone() const
      {
        return new ConditionBinaryCompositeOR( lhs->clone(), rhs->clone() );
      }

      ConditionBinaryCompositeOR* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
          return new ConditionBinaryCompositeOR(lhs->copy(alreadyCloned),rhs->copy(alreadyCloned));
      }

    virtual ~ConditionBinaryCompositeOR() {
      delete lhs;
      delete rhs;
    }

    virtual bool evaluate() {
        if ( lhs->evaluate() )
            return true;
        if ( rhs->evaluate() )
            return true;
        return false;
    }

    virtual void reset()
      {
        lhs->reset();
        rhs->reset();
      }
  };

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
