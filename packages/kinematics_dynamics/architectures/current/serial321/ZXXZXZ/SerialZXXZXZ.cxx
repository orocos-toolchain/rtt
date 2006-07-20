/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  SerialZXXZXZ.cxx 

                        SerialZXXZXZ.cxx -  description
                           -------------------
    begin                : Tue April 05 2005
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
 
 

#include "kindyn/SerialZXXZXZ.hpp"

namespace ORO_KinDyn
{
    using namespace RTT;

    void SerialZXXZXZ::configurationGet(const Double6D& q, Configuration& c) const
    {
        double s23;
        double dWh;

        c.set( Serial321Configuration::Normal );

        if ( q[ 4 ] < -KINEMATICS_EPS )
        {
            c.set( Serial321Configuration::Flip );
        }

        else
        {
            c.set( Serial321Configuration::NoFlip);
        }

        s23 = sin( q[ 1 ] + q[ 2 ] );

        /* horizontal distance of wrist from shoulder: */
        dWh = sin( q[ 1 ] ) * l2 + s23 * l3;


        /* CALCULATION OF CONFIGURATION: */

        if ( dWh >= 0.0 )
            c.set( Serial321Configuration::Forward );
        else
            c.set( Serial321Configuration::Backward );

        if ( q[ 4 ] < 0. )
            c.set( Serial321Configuration::Flip );

        if ( q[ 2 ] < 0. )
            c.set( Serial321Configuration::ElbowDown );

    }

}

