/***************************************************************************
 tag: Peter Soetens  Mon Jun 17 11:55:32 CEST 2002  motioncontrol/src/PControl.hpp 

                       motioncontrol/src/PControl.hpp -  description
                          -------------------
   begin                : Mon June 17 2002
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





#ifndef PCONTROL_HPP
#define PCONTROL_HPP

#include "ControlModuleInterface.hpp"

namespace MotionControl
{

    struct PAttr
    {
        double K;            // proportional
        double offset;       // output = (K*input) + offset
    };

    /**
     * A very simple Proportional control module
     */

    class PControl
                : public ControlModuleInput<double>,
                public ControlModuleOutput<double>
    {

        public:
            /**
             * Creates a new Proportional controller
             * @param attr The attributes of this control module
             */

            PControl( struct PAttr* attr )
                    : input( 0 ), K( attr->K ), offset( attr->offset )
            {}

            PControl( double Kp = 0, double off = 0 )
                    : input( 0 ), K( Kp ), offset( off )
            {}

            virtual ~PControl()
            {}

            void change( double Kp, double off = 0 )
            {
                K = Kp;
                offset = off;
            }

            virtual void outputGet( double& ppl )
            {
                if ( input != 0 )
                {
                    input->outputGet( ppl );
                    ppl = K * ppl + offset;
                }
            }
        
        virtual double scaleGet()
        {
            return K;
        }
        virtual double offsetGet()
        {
            return offset;
        }

            virtual void inputAdd( ControlModuleOutput<double>* in )
            {
                input = in;
            }

            virtual void inputRemove( ControlModuleOutput<double>* in )
            {
                if ( input == in )
                    input = 0;
            }

        protected:

            ControlModuleOutput<double>* input;

            double K;
            double offset;
    };
};

#endif // P_HPP

