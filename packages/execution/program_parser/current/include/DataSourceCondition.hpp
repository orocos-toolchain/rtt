/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  DataSourceCondition.hpp

                        DataSourceCondition.hpp -  description
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

#ifndef DATASOURCECONDITION_HPP
#define DATASOURCECONDITION_HPP

#include "DataSource.hpp"

namespace ORO_CoreLib
{
  class ConditionInterface;
}

namespace ORO_Execution
{
  using ORO_CoreLib::ConditionInterface;

  /**
   * A class that wraps a Condition in a DataSource<bool>
   * interface.
   */
  class DataSourceCondition
    : public DataSource<bool>
  {
    ConditionInterface* cond;
  public:
    /**
     * DataSourceCondition takes ownership of the condition you pass
     * it.
     */
      DataSourceCondition( ConditionInterface* c );
      DataSourceCondition( const DataSourceCondition& orig );
      ~DataSourceCondition();
      bool get() const;
      void reset();
      ConditionInterface* condition() const;
      virtual DataSourceCondition* clone() const;
      virtual DataSourceCondition* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
  };
}

#endif
