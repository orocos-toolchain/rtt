/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionBool.hpp

                        ConditionBool.hpp -  description
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


#ifndef CONDITIONBOOL_HPP
#define CONDITIONBOOL_HPP

#include <corelib/ConditionInterface.hpp>

namespace ORO_Execution
{

    using namespace ORO_CoreLib;


    class ConditionBool :
                public ConditionInterface
    {

        public:

            /**
             * Construct a new ConditionBool object and set it's value to false.
             */
            ConditionBool();

            /**
             * Construct a new ConditionBool object with a specified value.
             * @param value  The value of the ConditionBool object.
             */
            ConditionBool( bool value );

            /**
             * Destroy the ConditionBool object.
             */
            virtual ~ConditionBool();

            /**
             * Return the boolean value of the ConditionBool object.
             */
            virtual bool evaluate();

            /**
             * Set the value of the ConditionBool object to true.
             */
            virtual void setTrue();

            /**
             * Set the value of the ConditionBool object to false.
             */
            virtual void setFalse();

            /**
             * Return a copy of this Condition...
             */
            virtual ConditionInterface* clone() const;

        private:
            /**
             * The value of this ConditionBool object.
             */
            bool state;
    };

}

#endif
