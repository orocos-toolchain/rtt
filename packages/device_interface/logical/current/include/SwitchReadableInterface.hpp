/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  SwitchReadableInterface.hpp 

                        SwitchReadableInterface.hpp -  description
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
 
#ifndef SWITCH_READABLE_INTERFACE_HPP
#define SWITCH_READABLE_INTERFACE_HPP

namespace ORO_DeviceInterface
{

    /**
     * An interface that describes a switch one can query
     * for its binary status
     */

    class SwitchReadableInterface
    {

        public:
            /**
             * Read the status of this switch
             * @return false The switch is off
             * @return treu  The switch is on
             */
            virtual bool statusGet() = 0;
    };

} // Namespace ORO_CoreLib

#endif
