/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ConditionComposite.hpp

                        ConditionComposite.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef CONDITIONCOMPOSITE_HPP
#define CONDITIONCOMPOSITE_HPP

#include "rtt-scripting-config.h"
#include "ConditionInterface.hpp"

namespace RTT { namespace scripting {


  /**
   * Compose an 'AND' function of two Conditions.
   */
  class RTT_SCRIPTING_API ConditionBinaryCompositeAND
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

      ConditionBinaryCompositeAND* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
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
  class RTT_SCRIPTING_API ConditionBinaryCompositeOR
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

      ConditionBinaryCompositeOR* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
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

  class RTT_SCRIPTING_API ConditionCompositeNOT
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
    ConditionCompositeNOT* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const;
    void reset();
  };
}}

#endif
