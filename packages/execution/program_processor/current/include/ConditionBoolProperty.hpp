/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ConditionBoolProperty.hpp 

                        ConditionBoolProperty.hpp -  description
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
 
 
#ifndef CONDITIONBOOLPROPERTY_HPP
#define CONDITIONBOOLPROPERTY_HPP

#include <corelib/ConditionInterface.hpp>
#include <corelib/Property.hpp>

using ORO_CoreLib::ConditionInterface;
using ORO_CoreLib::Property;

namespace ORO_Execution
{
  /**
   * ConditionBoolProperty is a Condition which reads out a
   * Property<bool>.
   */
  class ConditionBoolProperty
    : public ConditionInterface
  {
    const Property<bool>& mprop;
  public:
    ConditionBoolProperty( const Property<bool>& prop )
      : mprop( prop )
      {
      };
    bool evaluate();
    ConditionInterface* clone() const;
    std::string toString();
  };
}

#endif
