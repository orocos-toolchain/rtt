/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:43 CEST 2002  Relaycardapci2200.hpp 

                        Relaycardapci2200.hpp -  description
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

#ifndef RELAYCARDAPCI12200_HPP
#define RELAYCARDAPCI12200_HPP

#include <device_interface/SwitchReadableInterface.hpp>
#include <device_interface/DigitalOutInterface.hpp>

#include <pkgconf/device_drivers_apci.h>

/* Forward declarations */

namespace ORO_DeviceDriver
{
    /**
     * A C++ wrapper for the APCI 2200 digital relay card.
     */
    class RelayCardapci2200
                : public ORO_DeviceInterface::DigitalOutInterface
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
