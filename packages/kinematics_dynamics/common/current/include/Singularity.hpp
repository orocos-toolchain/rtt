/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  Singularity.hpp 

                        Singularity.hpp -  description
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
 

#ifndef SINGULARITY_HPP
#define SINGULARITY_HPP

namespace ORO_KinDyn
{

    /**
     * A Singularity keeps track of the singularity a 6DOF robot is or is not in.
     */
    class Singularity
    {
    public:
        /**
         * An enumaration of all possible singularites.
         */
        enum Singularities
            {
                None = 0x00,
                WristAboveBase = 0x01,
                ArmExtended = 0x02,
                ParallelWrist = 0x04,
                OutOfReach = 0x08,
                AnySingularity = 0xFF
            };

		Singularity& operator|=(const Singularity &sing)
		{
			currentSing |= sing.currentSing;
			return (*this);
		}
		
        /**
         * Set the singularity the robot is in.
         * 
         * @param sing
         *        One of Singularities.
         */
        void singularitySet( const Singularities sing )
        {
            if ( sing == None )
                currentSing = None;
            else
                currentSing |= sing;
        }

        /**
         * Check if the robot is in singularity \a sing.
         *
         * @param sing
         *        The singularity to be checked.
         * @return true if it is in this singularity, 
         *         false otherwise.
         */
        bool isSingular( const Singularities sing ) const
        {
            return currentSing & sing;
        }

		bool isSingular() const
		{
			return currentSing != 0;	
		}
		
    private:
        int currentSing;

    };


}

#endif
