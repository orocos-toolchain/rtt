/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  SwitchEndLimit.hpp 

                        SwitchEndLimit.hpp -  description
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
 
#include <corelib/Event.hpp>
#include <corelib/ListenerRegistration.hpp>
#include <fdi/DigitalInInterface.hpp>
#include <utils.h>

#include "DigitalInput.hpp"

namespace ORO_DeviceDriver
{

    using namespace ORO_CoreLib;

    
    struct SwitchAttr
    {
        DigitalInInterface* board;
        unsigned int bitnumber;
    };
    
    
    /**
     * an SwitchEndLimit is a DigitalOutput which is in safeMode when turned off.
     * (active low)
     */
    class SwitchEndLimit
        : public DigitalInput
    {

        public:
            /**
             * Create an end limit switch with safe values being active low.
             *
             * @param dig_in The digital input device to read from
             * @param bit_number The bit to query.
             */
            SwitchEndLimit( DigitalInInterface* dig_in, unsigned int bit_number, bool invert )
                : DigitalInput(dig_in, bit_number, invert) {}

            /**
             * The safebit is active low.
             *
             * @return true if the switch is safe (not reached), 
             * false otherwise.
             */
            bool isSafe()
            {
                return !( isOn() );
            }

    };


    /**
     * simple KukaSwitchBoard-class, just to store the kukaswitch bitnumbers.
     * this KukaSwitchboard consist off 8 endLimitSwitches
     * (A1,A2 and A3 are not yet defined) and 3 kukaswitches.
     */
    class KukaSwitchBoard
    {

        public:
            KukaSwitchBoard( DigitalInInterface* apci1032 ) : apci1032board( apci1032 )
            {
                /*
                //power switches
                attr.bitnumber = 14;
                powerButton = new SwitchKuka( &attr );
                attr.bitnumber = 13;
                safetyButton = new SwitchKuka( &attr );
                attr.bitnumber = 12;
                kukaSafeSwitch = new SwitchKuka( &attr );

                //kuka endlimitswitches
                attr.bitnumber = 5;
                A3min = new SwitchEndLimit( &attr );
                attr.bitnumber = 6;
                A3plus = new SwitchEndLimit( &attr );
                attr.bitnumber = 7;
                A4min = new SwitchEndLimit( &attr );
                attr.bitnumber = 8;
                A4plus = new SwitchEndLimit( &attr );
                attr.bitnumber = 9;
                A5min = new SwitchEndLimit( &attr );
                attr.bitnumber = 10;
                A5plus = new SwitchEndLimit( &attr );
                attr.bitnumber = 17;
                A6min = new SwitchEndLimit( &attr );
                attr.bitnumber = 18;
                A6plus = new SwitchEndLimit( &attr );
                */
            }

            ~KukaSwitchBoard()
            {
                /*
                delete powerButton;
                delete safetyButton;
                delete kukaSafeSwitch;
                delete A3plus;
                delete A3plus;
                delete A4min;
                delete A4min;
                delete A5min;
                delete A5min;
                delete A6min;
                delete A6min;
                */
            }

            DigitalInInterface *apci1032board;

            SwitchEndLimit *A3plus;
            SwitchEndLimit *A3min;
            SwitchEndLimit *A4plus;
            SwitchEndLimit *A4min;
            SwitchEndLimit *A5plus;
            SwitchEndLimit *A5min;
            SwitchEndLimit *A6plus;
            SwitchEndLimit *A6min;

            //kuka power button
            DigitalInput *powerButton;
            //kuka safetyButton.isOn() returns true when safetybutton is in safemode
            DigitalInput *safetyButton;
            //kukaSafeSwitch.isOn() returns true when there are no SwitchEndLimites activated
            DigitalInput *kukaSafeSwitch;
    };
}
