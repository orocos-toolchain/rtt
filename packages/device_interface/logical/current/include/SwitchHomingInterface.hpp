/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  SwitchHomingInterface.hpp 

                        SwitchHomingInterface.hpp -  description
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
