/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  SwitchHomingInterface.hpp 

                        SwitchHomingInterface.hpp -  description
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
 
 

#ifndef SWITCHHOMINGINTERFACE_HPP
#define SWITCHHOMINGINTERFACE_HPP

#include <corelib/EventInterfaces.hpp>

namespace ORO_DeviceInterface
{

    /**
     * The interface for homing switches on an Axis
     */
    class SwitchHomingInterface
    {
        public:

        virtual ~SwitchHomingInterface()
        {}
        
        /**
         * Returns an HandlerRegistrationInterface on which you
         * can register for notification of a homing event.
         */
        virtual HandlerRegistrationInterface* homingEvent() = 0;

        /**
         * Returns if the back (reverse) switch is enabled.
         */
        virtual bool isBack() = 0;

        /**
         * Returns if the front (forward) switch is enabled.
         */
        virtual bool isFront() = 0;
    };

};

#endif
