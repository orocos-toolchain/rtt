/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionFalse.hpp

                        ConditionFalse.hpp -  description
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



#ifndef CONDITIONFALSE_HPP
#define CONDITIONFALSE_HPP

#include "ConditionInterface.hpp"

namespace ORO_CoreLib
{

    /**
     * A conditional that evaluates false
     */

    class ConditionFalse
                : public ConditionInterface
    {

        public:
            ConditionFalse()
            {}

            virtual ~ConditionFalse()
            {}

            virtual bool evaluate()
            {
                return false;
            }

            virtual ConditionInterface* clone() const
            {
                return new ConditionFalse;
            }
    };

}

#endif
