/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  Singularity.hpp 

                        Singularity.hpp -  description
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
         * Check if the robot is in singularity <sing>.
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
