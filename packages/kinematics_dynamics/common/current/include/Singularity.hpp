
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
