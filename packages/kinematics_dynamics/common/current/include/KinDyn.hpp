/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  KinDyn.hpp 

                        KinDyn.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
