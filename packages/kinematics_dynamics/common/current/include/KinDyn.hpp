/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  KinDyn.hpp 

                        KinDyn.hpp -  description
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
 * @brief This namespace contains all classes of the kinematics_dynamics package.
 *
 * It is meant to group all packages which provide kinematic and 
 * dynamic algorithms. The algorithms are state-less and need all
 * information passed through the parameters. The KinematicComponent
 * uses these algorithms to provide a state-full component which
 * keeps track of the current configuration and position, velocities etc.
 *
 * kinematics_dynamics is documented in <a href="../../../orocos-kinematics.html">
 * The Orocos Kinematics and Dynamics Online Manual</a>
 */
namespace ORO_KinDyn {}
