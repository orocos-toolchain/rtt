/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ConditionBoolDataSource.hpp 

                        ConditionBoolDataSource.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef CONDITIONBOOLDATASOURCE_HPP
#define CONDITIONBOOLDATASOURCE_HPP

#include <corelib/ConditionInterface.hpp>
#include "DataSource.hpp"


namespace ORO_Execution
{

    using ORO_CoreLib::ConditionInterface;

  class ConditionBoolDataSource
    : public ConditionInterface
  {
    DataSource<bool>::shared_ptr data;
  public:
    ConditionBoolDataSource( DataSource<bool>* s )
      : data( s )
      {
      };
    ~ConditionBoolDataSource();
    bool evaluate();
    ConditionInterface* clone() const;
    void reset();
  };
};

#endif
