/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  CoreLib.hpp 

                        CoreLib.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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

/**
 * @file CoreLib.hpp
 * This file includes the commonly used header files from the 
 * whole ORO_CoreLib package.
 */
 
/**
 * @brief Buffers, DataObjects, Commands, NameServing, Activities, Events, Timing, Properties
 *
 * It is meant to group all packages which provide OS independent
 * core functionality in a realtime system. Frequently used classes are Event,
 * PeriodicActivity, BufferLockFree, DataObjectLockFree, TimeService, NameServer, Property,
 * PropertyBag.
 *
 * The corelib is documented in <a href="../../orocos-corelib.html">
 * The Orocos CoreLib Online Manual</a>
 */
namespace ORO_CoreLib {}

#include "Common.hpp"
#include "Properties.hpp"
#include "Events.hpp"
#include "Activities.hpp"
#include "Buffers.hpp"
#include "TimeService.hpp"

