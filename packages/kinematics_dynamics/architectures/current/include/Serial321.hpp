/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  Serial321.hpp 

                        Serial321.hpp -  description
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
 

#ifndef SERIAL321_HPP
#define SERIAL321_HPP

#include "KinematicsComponent.hpp"
#include "KinematicsInterface.hpp"
#include "Kinematics6DWrapper.hpp"
#include "Singularity.hpp"
#include "Serial321Configuration.hpp"

namespace ORO_KinDyn
{
    const double M_PI_T2 = 2 * M_PI;

#ifndef SQRT3d2
    const double SQRT3d2 = 0.8660254037844386; // sqrt(3)/2
#endif
#ifndef SQRT3t2
    const double SQRT3t2 = 3.46410161513775; // 2 sqrt(3)
#endif

    // from Status6R.h

    /* `Distances' around singular points in which positions are NUMERICALLY
       considered to be singular: */

    const double EPS_WRIST_ABOVE_BASE = 0.001; // units: m
    /* decision variable: XY distance between wrist and shoulder */

    const double EPS_ARM_EXTENDED = 0.001; // units: m
    /* decision variable: distance of wrist to shoulder must lie between
       sqrt(sq(L2+L3)) and sqrt(sq(L2-L3)). */

    const double EPS_PARALLEL_WRIST = 0.001; // units: dimensionless
    /* decision variable: sine of flip angle (fifth joint) goes to zero. */

    const double KINEMATICS_EPS = 1.0e-8;
    /* Considered VERY small in the context of kinematic calculations. */


    /**
     * This class implements the stateless kinematics calculations
     * for all kinds of 6DOF Robots
     *
     */
    class Serial321
        : public Kinematics6DWrapper
    {

    public:
        /**
         * Construct a model for a robot with offset = 0 and eccentricity = 0 and
         * all links of length = 1.
         */
        Serial321()
            : offset(0), eccent( 0 )
        {
            geometrySet( 1, 1, 1, 1, 1, 1);
        }

        virtual ~Serial321() {}

        /**
         * Set the armlength of all robot arms.
         */
        virtual void geometrySet( const double len1, const double len2, const double len3,
                                  const double len4, const double len5, const double len6 )
        {
            l1 = len1;
            l2 = len2;
            l3 = len3;
            l4 = len4;
            l5 = len5;
            l6 = len6;
            const1 = l2*l2 + l3*l3;
            const2 = 2. * l2 * l3;
            max_wrist_dist = const1 + const2;
        }

        /**
         * Set the shoulder offset
         * 
         * @param off
         *        The offset in meters
         */
        virtual void offsetSet( const double off )
        {
            offset = off;
        }

        /**
         * Set the arm eccentricity
         * 
         * @param ecc
         *        The eccentricity in meters
         */
        virtual void eccentricitySet( const double ecc )
        {
            eccent = ecc;
        }

        /*
          void singularityRangeSet( const double d_AE, const double d_WAB,
          const double d_PW, const double d_OOR )
          {
          Dist_AE = d_AE;
          Dist_WAB = d_WAB;
          Dist_PW = d_PW;
          Dist_OOR = d_OOR;
          }
        */


    protected:

        double l1, l2, l3; /* Link lengths */
        double l4, l5, l6; /* Link lengths */
        double offset;  /* shoulder offset */
        double eccent;  /* arm eccentricity */
        double const1;  /* l2^2 + l3^2 */
        double const2;  /* 2 * l2 * l3 */
        double max_wrist_dist; /* Maximum distance of wrist from shoulder */
#if 0
        // Probably not needed
        double Dist_AE;  /* margin around Arm Extended singularity */
        double Dist_WAB; /* margin around Wrist Above Base singularity */
        double Dist_PW;  /* margin around Parallel Wrist singularity */
        double Dist_OOR; /* margin around Out Of Reach singularity */
#endif
    };

}

#endif
