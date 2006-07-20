/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:43 CEST 2002  SwitchDigitalInapci1032.hpp 

                        SwitchDigitalInapci1032.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 
/* koen.muylkens@esat.kuleuven.ac.be */

#ifndef SWITCHDIGITALINAPCI1032_HPP
#define SWITCHDIGITALINAPCI1032_HPP

#include "DigitalInInterface.hpp"

#ifdef OROCLS_DEVICE_DRIVERS_APCI1032_EVENTS
#include <rtt/PreemptibleActivity.hpp>
#include <rtt/Event.hpp>
#include <rtt/ListenerRegistration.hpp>
#endif

namespace RTT
{

    
    

    /**
     * A physical device class for reading our digital input card.
     * Fires an event when its state changed.
     */
    class SwitchDigitalInapci1032 :
          public DigitalInInterface
#ifdef OROCLS_DEVICE_DRIVERS_APCI1032_EVENTS
          ,public ListenerRegistration, 
          public PreemptibleActivity // fast !
#endif
    {

        public:
            SwitchDigitalInapci1032( const std::string& name );
            virtual ~SwitchDigitalInapci1032();

            virtual bool isOn( unsigned int bit = 0) const;

            virtual bool isOff( unsigned int bit = 0) const;
            
            virtual bool readBit( unsigned int bit = 0) const;

            virtual unsigned int readSequence(unsigned int start_bit, unsigned int stop_bit) const;
            
            virtual unsigned int nbOfInputs() const { return 32; }

#ifdef OROCLS_DEVICE_DRIVERS_APCI1032_EVENTS
            //old and new value stored, to detect change and fire the inputEvent
            unsigned long getNewValue();
            unsigned long getOldValue();
#endif

        private:
            int checkAndGetPCISlotNumber();
            int setBoardInformation();
            int getHardwareInformation( unsigned int * pui_BaseAddress, unsigned char * pb_InterruptNbr, unsigned char * pb_SlotNumber );
            int closeBoardHandle();
#ifdef OROCLS_DEVICE_DRIVERS_APCI1032_EVENTS
            void step();

            Event inputEvent;

            unsigned long new_InputValue;
            unsigned long old_InputValue;
#endif

            int BoardHandle;
            unsigned char SlotNumber;
            unsigned char SlotNumberArray[ 10 + 1 ]; //MAX_BOARDNR + 1
            static int bh;
    };
}

#endif
