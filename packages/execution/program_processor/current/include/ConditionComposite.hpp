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

#include <corelib/ConditionInterface.hpp>

namespace ORO_Execution {

  /**
   * Compose an 'AND' function of two Conditions.
   */
  class ConditionBinaryCompositeAND
    : public ORO_CoreLib::ConditionInterface
  {
    ORO_CoreLib::ConditionInterface* lhs;
    ORO_CoreLib::ConditionInterface* rhs;
  public:
    /**
     * If \a l evaluates to true, evaluate \r and return the result, otherwise,
     * return false.
     * Constructor takes ownership of \a l and \a r.
     */
    ConditionBinaryCompositeAND( ORO_CoreLib::ConditionInterface* l, ORO_CoreLib::ConditionInterface* r )
      : lhs( l ), rhs( r )
      {
      }

    virtual ORO_CoreLib::ConditionInterface* clone() const
      {
        return new ConditionBinaryCompositeAND( lhs->clone(), rhs->clone() );
      }

      ConditionBinaryCompositeAND* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
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
    : public ORO_CoreLib::ConditionInterface
  {
    ORO_CoreLib::ConditionInterface* lhs;
    ORO_CoreLib::ConditionInterface* rhs;
  public:
    /**
     * If \a l evaluates to true, evaluate \r and return the result, otherwise,
     * return false.
     * Constructor takes ownership of \a l and \a r.
     */
    ConditionBinaryCompositeOR( ORO_CoreLib::ConditionInterface* l, ORO_CoreLib::ConditionInterface* r )
      : lhs( l ), rhs( r )
      {
      }

    virtual ORO_CoreLib::ConditionInterface* clone() const
      {
        return new ConditionBinaryCompositeOR( lhs->clone(), rhs->clone() );
      }

      ConditionBinaryCompositeOR* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
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
    : public ORO_CoreLib::ConditionInterface
  {
    ORO_CoreLib::ConditionInterface* cond;
  public:
    ConditionCompositeNOT( ORO_CoreLib::ConditionInterface* c )
      : cond( c )
      {
      }
    ~ConditionCompositeNOT();
    bool evaluate();
    ConditionCompositeNOT* clone() const;
    ConditionCompositeNOT* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const;
    void reset();
  };
}

#endif
