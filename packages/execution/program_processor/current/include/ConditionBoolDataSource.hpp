/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ConditionBoolDataSource.hpp

                        ConditionBoolDataSource.hpp -  description
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

#ifndef CONDITIONBOOLDATASOURCE_HPP
#define CONDITIONBOOLDATASOURCE_HPP

#include <corelib/ConditionInterface.hpp>
#include "DataSource.hpp"


namespace ORO_Execution
{

    using ORO_CoreLib::ConditionInterface;

    /**
     * A Condition which holds a boolean DataSource.
     */
  class ConditionBoolDataSource
    : public ConditionInterface
  {
    DataSource<bool>::shared_ptr data;
  public:
      /**
       * The constructor stores the DataSource \a s in a shared_ptr.
       */
    ConditionBoolDataSource( DataSource<bool>* s )
      : data( s )
      {
      }
    ~ConditionBoolDataSource();
    bool evaluate();
    ConditionBoolDataSource* clone() const;
    void reset();
    ConditionBoolDataSource* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const;
  };
};

#endif
