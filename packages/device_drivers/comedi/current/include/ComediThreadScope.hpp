/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:51 CEST 2005  ComediThreadScope.hpp 

                        ComediThreadScope.hpp -  description
                           -------------------
    begin                : Sat May 21 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef ORO_COMEDI_THREAD_SCOPE_HPP
#define ORO_COMEDI_THREAD_SCOPE_HPP

#include <pkgconf/device_drivers_comedi.h>
#include "comedi/ComediDevice.hpp"
#include "comedi/ComediSubDeviceDOut.hpp"

namespace RTT
{

    /**
     * A ThreadScope implementatation for Comedi Digital Outputs.
     * Enable the Comedi Thread Scope in the configtool to use
     * a comedi device as a thread monitoring scope.
     * @see ParallelPort
     */
    class ComediThreadScope
        : public ComediDevice,
          public ComediSubDeviceDOut
    {
    public:
        ComediThreadScope()
            : ComediDevice( ORODAT_DEVICE_DRIVERS_COMEDI_THREAD_SCOPE_MINOR ),
              ComediSubDeviceDOut( this, ORODAT_DEVICE_DRIVERS_COMEDI_THREAD_SCOPE_SUBDEVICE )
        {}

        ComediThreadScope(const std::string& name)
            : ComediDevice( ORODAT_DEVICE_DRIVERS_COMEDI_THREAD_SCOPE_MINOR ),
              ComediSubDeviceDOut( this, name, ORODAT_DEVICE_DRIVERS_COMEDI_THREAD_SCOPE_SUBDEVICE )
        {}
    };

}

#endif
