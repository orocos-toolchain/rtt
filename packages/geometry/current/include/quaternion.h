/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:21:19 CEST 2002  Quaternion.hpp 

                        Quaternion.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include "frames.h"
#include <math.h>

namespace ORO_Geometry
{
    using namespace std;

    /**
     * An implementation of Quaternions or Euler parameters, describing a
     * rotation of a point in cartesian space around a vector w with an
     * angle theta.
     */

    class Quaternion
    {

        public:
            /**
             * Create a Quaternion based on a normalised vector and an angle.
             *
             * @parm _w A normalized vector denoting the equivalent angle axis.
             * @parm _angle The angle you wish to rotate.
             */
            Quaternion( Vector _w, double _angle )
                    : w( _w ), angle( _angle ), l0( cos( _angle / 2. ) ),
                    l1( _w( 0 ) *sin( _angle / 2. ) ),
                    l2( _w( 1 ) *sin( _angle / 2. ) ),
                    l3( _w( 2 ) *sin( _angle / 2. ) )
            {}

            /**
             * Create a Quaternion from a rotation matrix.
             *
             * @parm _r The rotation matrix you wish to express with this 
             *          quaternion.
             */
            Quaternion( Rotation _r )
            {
                setMatrix( _r );
            }

            /**
             * Sets the Quaternion to represent a given Rotation. 
             * The unit Rotation matrix is represented by a unit vector w
             * along the X axis.
             * 
             * @param _r
             *        The Rotation with which the Quaternion must set
             */
            void setMatrix( Rotation _r )
            {
                l0 = ( 0.5 * sqrt( _r( 0, 0 ) + _r( 1, 1 ) + _r( 2, 2 ) + 1 ) );

                if ( _r( 2, 1 ) - _r( 1, 2 ) == 0 )
                    l1 = 0;
                else
                    l1 = ( 0.5 * sign( _r( 2, 1 ) - _r( 1, 2 ) ) * sqrt( _r( 0, 0 ) - _r( 1, 1 ) - _r( 2, 2 ) + 1 ) );

                if ( _r( 0, 2 ) - _r( 2, 0 ) == 0 )
                    l2 = 0;
                else
                    l2 = ( 0.5 * sign( _r( 0, 2 ) - _r( 2, 0 ) ) * sqrt( - _r( 0, 0 ) + _r( 1, 1 ) - _r( 2, 2 ) + 1 ) );

                if ( _r( 1, 0 ) - _r( 0, 1 ) == 0 )
                    l3 = 0;
                else
                    l3 = ( 0.5 * sign( _r( 1, 0 ) - _r( 0, 1 ) ) * sqrt( - _r( 0, 0 ) - _r( 1, 1 ) + _r( 2, 2 ) + 1 ) );

                angle = 2. * acos( l0 );

                if ( angle != 0 )
                    w = Vector( l1 / sin( angle / 2. ), l2 / sin( angle / 2. ), l3 / sin( angle / 2. ) );
                else
                    w = Vector( 1, 0, 0 );  // A default vector
            }


            /**
             * Get a Rotation matrix representing this quaternions rotation.
             */
            Rotation getMatrix() const
            {
                return Rotation( 2. * ( l0 * l0 + l1 * l1 ) - 1, 2. * ( l1 * l2 - l0 * l3 ) , 2. * ( l1 * l3 + l0 * l2 ),
                                 2. * ( l1 * l2 + l0 * l3 ) , 2. * ( l0 * l0 + l2 * l2 ) - 1, 2. * ( l2 * l3 - l0 * l1 ),
                                 2. * ( l1 * l3 - l0 * l2 ) , 2. * ( l2 * l3 + l0 * l1 ) , 2. * ( l0 * l0 + l3 * l3 ) - 1 );
            }

            /**
             * Set the angle you wish to rotate around the axis.
             */
            void setAngle( const double a )
            {
                angle = a;
                double fact = sin( a / 2. );
                l0 = cos( a / 2. );
                l1 = w( 0 ) * fact;
                l2 = w( 1 ) * fact;
                l3 = w( 2 ) * fact;
            }

            /**
             * Get the angle which is used to rotate around the axis.
             */
            inline
            double getAngle() const
            {
                return angle;
            }

            /**
             * Get a Vector denoting the axis around which the rotation is done.
             */
            inline
            Vector getVector() const
            {
                return w;
            }


        protected:
            /**
             * Forbidden constructor.
             */
            Quaternion() : angle( 0 ), l0( 0 ), l1( 0 ), l2( 0 ), l3( 0 )
            {}

            /**
             * Internal representation of the rotation axis.
             */
            Vector w;
            /**
             * Internal representation of the rotation angle.
             */
            double angle;
            /**
             * Internal representation of the lambda coefficients.
             */
            double l0, l1, l2, l3;
    };

}

#endif
