/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:55 CEST 2002  ActuatorInterface.hpp 

                        ActuatorInterface.hpp -  description
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
 
#ifndef ACTUATOR_INTERFACE_HPP
#define ACTUATOR_INTERFACE_HPP

namespace ORO_DeviceInterface
{

    /**
     * This interface describes a basic actuator (motor). Since they use
     * mostly a hardware drive to be accessed, this interface describes
     * the functionality of a basic speedcontrol drive.
     *
     * Attention, the offset is only applied when the drive/stop methods are 
     * called. offset() itself does not drive the actuator.
     */
    template <class T>
    class ActuatorInterface
    {

        public:
            typedef T InputStruct;

            ActuatorInterface()
            {}

            virtual ~ActuatorInterface()
            {}

            /**
             * Drive the actuator.
             */
            virtual int drive( const InputStruct& v ) = 0;

            /**
             * Returns the current driven velocity
             */
            virtual InputStruct speedGet() const = 0;

            /**
             * Returns the maximum drive value (forward)
             */
            virtual InputStruct maxDriveGet() const = 0;

            /**
             * Returns the minimum drive value (reverse)
             */
            virtual InputStruct minDriveGet() const = 0;

            /**
             * Returns the value which denotes standstill, without
             * offset.
             */
            virtual InputStruct zeroGet() const = 0;

            /**
             * Stop the actuator
             */
            virtual void stop() = 0;

            /**
             * Set an offset value
             * 
             * @param <o> The offset to be added to each drive command
             *            The unit is the same as the unit of drive()
             * @post  Each drive call from now on will be offsetted with this value
             */
            virtual void offsetSet( const InputStruct& o ) = 0;
    };

};

#endif // ACTUATOR_INTERFACE_HPP

