/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionOnce.hpp

                        ConditionOnce.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#ifndef CONDITIONONCE_HPP
#define CONDITIONONCE_HPP

#include "ConditionInterface.hpp"

namespace ORO_CoreLib
{

    /**
     * A conditional that evaluates the first time true
     * and afterwards always false
     */

    class ConditionOnce
                : public ConditionInterface
    {

        public:
            ConditionOnce() : ran( false )
            {}

            virtual ~ConditionOnce()
            {}

            virtual bool evaluate()
            {
                return ( ran == true ? false : ran = true );
            }

            virtual void reset()
            {
                ran = false;
            };

            ConditionInterface* copy() const
            {
                return new ConditionOnce;
            }

        private:
            /**
             * Becomes true after one evaluation after its reset()
             * method has been called.
             */
            bool ran;
    };

}

#endif
