/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:43 CEST 2002  Relaycardapci2200.hpp 

                        Relaycardapci2200.hpp -  description
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

#ifndef RELAYCARDAPCI12200_HPP
#define RELAYCARDAPCI12200_HPP

#include "DigitalOutInterface.hpp"

#include <pkgconf/device_drivers_apci.h>

/* Forward declarations */

namespace RTT
{
    /**
     * A C++ wrapper for the APCI 2200 digital relay card.
     */
    class RelayCardapci2200
                : public DigitalOutInterface
    {

        public:
            /**
             * Create a Relay instance which will communicate
             * the methods to the APCI2200 card device driver.
             *
             * @param name The name to be used in the DigitalOutInterface::nameserver.
             */
            RelayCardapci2200( const std::string& name );

            virtual ~RelayCardapci2200();

            virtual void switchOn( unsigned int n );

            virtual void switchOff( unsigned int n );

            virtual void setBit( unsigned int bit, bool value );
                
            virtual void setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value);

            virtual bool checkBit(unsigned int n) const;

            virtual unsigned int checkSequence( unsigned int start_bit, unsigned int stop_bit ) const;
            
            virtual unsigned int nbOfOutputs() const;

        private:
            /**
             * A handle to the device driver.
             */
#ifdef OROINT_DEVICE_DRIVERS_APCI2200
            OROIMP_DEVICE_DRIVERS_APCI2200_T *apci2200;
#endif

            /**
             * Our local cache of the output status.
             */
            unsigned int output_cache;
    };
}

#endif
