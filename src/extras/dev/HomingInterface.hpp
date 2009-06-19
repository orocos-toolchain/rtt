/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:22:14 CEST 2002  Axis.hpp

                          HomingInterface.hpp -  description
                          -------------------
   begin                : Thu October 21 2004
   copyright            : (C) 2004 Johan Rutgeerts
   email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HOMING_INTERFACE_HPP
#define _HOMING_INTERFACE_HPP

namespace RTT
{

class HomingInterface
{
public:

    /**
     * A HomingInterface provides all necessary data to perform axis homing.
     *
     * @ingroup DeviceInterface
     */
    HomingInterface();

    virtual ~HomingInterface();

    /**
     * Returns true if a 'home switch event' has occurred
     * (e.g. a home switch pulsed or changed its state) since
     * the last reset.
     */
    virtual bool homingStatus() = 0;


    /**
     * Sets the homingstatus to false.
     */
    virtual void resetHomingStatus() = 0;


    /**
     * Returns the recommended drive value for coarse movement to
     * the home switch. If the axis is driven with this value,
     * a 'home switch event' is guaranteed to occur.
     */
    virtual double homingDriveValue() = 0;

};

}; //namespace RTT

#endif //_HOMING_INTERFACE_HPP

