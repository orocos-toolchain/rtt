/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:55 CEST 2006  RTT.hpp 

                        RTT.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 

/**
 * @file RTT.hpp
 * This file includes the commonly used header files from the 
 * whole Orocos Real-Time Toolkit.
 */
 
/**
 * @brief State Machines, Activities, XML Properties, Scripting,...
 * 
 *
 * The Real-Time Toolkit is documented in <a href="../../orocos-manual.html">
 * The Orocos Real-Time Toolkit Online Manual</a>
 */
namespace RTT {}

#include <corelib/CoreLib.hpp>
#include <execution/Execution.hpp>
#include <os/OS.hpp>
#include <device_interface/DeviceInterface.hpp>
#include <device_drivers/DeviceDrivers.hpp>

namespace RTT {
    using namespace ORO_CoreLib;
    using namespace ORO_Execution;
    using namespace ORO_OS;
    using namespace ORO_DeviceInterface;
    using namespace ORO_DeviceDriver;
}
