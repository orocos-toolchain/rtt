
#ifndef _SERIAL321CONFIGURATION_HPP_
#define _SERIAL321CONFIGURATION_HPP_

#include "Configuration.hpp"

namespace ORO_KinDyn
{

    /**
     * This class keeps the configuration state of a 6DOF robot.
     * There are 8 possible configurations being a combination of one of
     * (Forward, Backward) and one of (Elbow up, Elbow down) and one of (Flip, No Flip)
     * to reach one cartesian point in the robots workspace. Choosing one configuration
     * solves this redundancy.
     */
    class Serial321Configuration : public Configuration {
    public:
        /**
         * Possible configurations:
         *  F: FORWARD  B: BACKWARD
         *  D: ELBOW DOWN   U: ELBOW UP
         *  F: FLIP     N: NO FLIP  
         */

        /**
         * The robot is in forward position.
         */
        static const int Forward = 0x01;

        /**
         * The robot is in backward position.
         */
        static const int Backward = 0x02;

        /**
         * The robots elbow is upward.
         */
        static const int ElbowUp = 0x04;

        /**
         * The robots elbow is downward.
         */
        static const int ElbowDown = 0x08;

        /**
         * The robots shoulder is not flipped.
         */
        static const int NoFlip = 0x10;

        /**
         * The robots shoulder is flipped.
         */
        static const int Flip = 0x20;

        /**
         * The robot in the normal/default configuration
         */
        static const int Normal = 0x40;
        
        /**
         * Create a configuration with state (Forward, ElbowUp, NoFlip).
         */
        Serial321Configuration()
        {
            set( Forward );
            set( ElbowUp );
            set( NoFlip );
        }
    
        /**
         * Set the configuration.
         */
        void set ( const int configurationPart )
        {
            switch ( configurationPart )
                {
                case Normal:
                    configuration = Forward | ElbowUp | NoFlip;
                    break;

                case Forward:
                    configuration |= Forward;
                    configuration &= ~Backward;
                    break;

                case Backward:
                    configuration |= Backward;
                    configuration &= ~Forward;
                    break;

                case ElbowUp:
                    configuration |= ElbowUp;
                    configuration &= ~ElbowDown;
                    break;

                case ElbowDown:
                    configuration |= ElbowDown;
                    configuration &= ~ElbowUp;
                    break;

                case NoFlip:
                    configuration |= NoFlip;
                    configuration &= ~Flip;
                    break;

                case Flip:
                    configuration |= Flip;
                    configuration &= ~NoFlip;
                    break;

                default:
                    break;
                }
        }

   
    
    private:
    };
}
#endif
