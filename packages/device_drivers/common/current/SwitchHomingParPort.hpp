/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:24:07 CEST 2002  SwitchHomingParPort.hpp 

                       SwitchHomingParPort.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
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


#ifndef SWITCHHOMINGPARPORT_HPP
#define SWITCHHOMINGPARPORT_HPP

#include <fdi/SwitchHomingInterface.hpp>
#include <fdi/DigitalInInterface.hpp>
#include <corelib/EventInterrupt.hpp>

extern "C"
    void parport_init( void );

namespace CBDeviceDriver
{

    using namespace ORO_CoreLib;

    /**
     * A class which listens for interrupts on the parallell port and
     * fires a homing Event when it occured. The effective switch state
     * is read from two channels from a DigitalInInterface.
     */
    class SwitchHomingParPort
                : public SwitchHomingInterface
    {

        public:
        /**
         * Create a end limit switch triggerd by the parallel port interrupt.
         * @param card The digital input card from which the switch state is read.
         * @param backSwitch The bit number (channel) of the <card> which represents the back switch.
         * @param frontSwitch The bit number (channel) of the <card> which represents the back switch.
         */
            SwitchHomingParPort(DigitalInInterface* card, unsigned int backSwitch, unsigned int frontSwitch )
                : SwitchHomingInterface(), back(backSwitch), front(frontSwitch), mycard(card)
            {
                myEI.IRQSet( 7 );
                myEI.hardwareInitSet( &parport_init );
                myEI.IRQRequest();
            }

            virtual ~SwitchHomingParPort()
            {
                myEI.disable();
                myEI.IRQFree();
            }

        virtual bool isFront() 
        {
            return mycard->readBit(front);
        }

        virtual bool isBack() 
        {
            return mycard->readBit(back);
        }

        virtual HandlerRegistrationInterface* homingEvent()
        {
            return &myEI;
        }

    protected:
        EventInterrupt myEI;
        unsigned int back;
        unsigned int front;
        DigitalInInterface* mycard;
    };
};





#endif
