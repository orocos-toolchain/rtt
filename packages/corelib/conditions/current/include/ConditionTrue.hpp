/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionTrue.hpp

                        ConditionTrue.hpp -  description
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



#ifndef CONDITIONTRUE_HPP
#define CONDITIONTRUE_HPP

#include "ConditionInterface.hpp"

namespace ORO_CoreLib
{

    /**
     * A conditional that evaluates true
     */

    class ConditionTrue
                : public ConditionInterface
    {

        public:
            ConditionTrue()
            {}

            virtual ~ConditionTrue()
            {}

            virtual bool evaluate()
            {
                return true;
            }

            virtual ConditionInterface* clone() const
            {
                return new ConditionTrue;
            }
    };

}

#endif
