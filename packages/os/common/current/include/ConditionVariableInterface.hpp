/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionVariableInterface.hpp 

                        ConditionVariableInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 
#ifndef CONDITIONVARIABLEINTERFACE_HPP
#define CONDITIONVARIABLEINTERFACE_HPP

#include <os/fosi.h>
#include "ConditionInterface.hpp"

namespace ORO_OS
{

    /**
     * An object of this class is a condition variable that permits to
     * test the value of a condition (ConditionInterface).
     *
     * All potential modifications of the value of the ConditionInterface should happen in a critical section
     * secured by a mutex and the last call in the critical section has to be a broadcast, so the 
     * ConditionVariable knows the value could have been changed. The mutex should be given as an argument to the
     * constructor of the ConditionVariable. Look at ConditionVariableTimed.hpp for an example.
     * 
     */

    class ConditionVariableInterface
    {

        public:
            virtual ~ConditionVariableInterface()
            {}

            /**
             * Returns the value of the ConditionVariableInterface object.
             */
            virtual ConditionInterface* conditionGet() = 0 ;

            /**
             * Broadcast that the condition could have been changed.
             */
            virtual void broadcast() = 0;

    };

}

#endif // CONDITIONVARIABLEINTERFACE_HPP
