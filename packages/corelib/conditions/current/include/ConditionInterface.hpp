/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionInterface.hpp

                        ConditionInterface.hpp -  description
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



#ifndef CONDITIONINTERFACE_HPP
#define CONDITIONINTERFACE_HPP

namespace ORO_CoreLib
{

    class ConditionInterface
    {

        public:
            virtual ~ConditionInterface()
            {}

            virtual bool evaluate() = 0;

            /**
             * Some conditions need to be reset at some points.
             * E.g. a ConditionDuration counts the time since the
             * first time a Command was executed, and if this time
             * exceeds a certain preset time, returns true.
             * Therefore, it needs to be reset, i.e. it needs to start
             * counting, when the command is first executed..
             * ConditionOnce has a similar need.  This function is
             * called at such times.
             */
            virtual void reset()
            {
            }

            virtual ConditionInterface* clone() const = 0;
    };

}

#endif
