/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:43 CEST 2002  SwitchDigitalInapci1032.hpp 

                        SwitchDigitalInapci1032.hpp -  description
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
 
 
/* koen.muylkens@esat.kuleuven.ac.be */

#ifndef SWITCHDIGITALINAPCI1032_HPP
#define SWITCHDIGITALINAPCI1032_HPP

#include <device_interface/DigitalInInterface.hpp>

#ifdef OROCLS_DEVICE_DRIVERS_APCI1032_EVENTS
#include <corelib/TaskPreemptible.hpp>
#include <corelib/Event.hpp>
#include <corelib/ListenerRegistration.hpp>
#endif

namespace ORO_DeviceDriver
{

    using namespace ORO_DeviceInterface;
    using namespace ORO_CoreLib;

    /**
     * A physical device class for reading our digital input card.
     * Fires an event when its state changed.
     */
    class SwitchDigitalInapci1032 :
          public DigitalInInterface
#ifdef OROCLS_DEVICE_DRIVERS_APCI1032_EVENTS
          ,public ListenerRegistration, 
          public TaskPreemptible // fast !
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
