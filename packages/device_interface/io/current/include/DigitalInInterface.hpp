/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  DigitalInInterface.hpp 

                        DigitalInInterface.hpp -  description
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
 
 

#ifndef DIGITALININTERFACE_HPP
#define DIGITALININTERFACE_HPP

#include <corelib/NameServer.hpp>
#include <corelib/NameServerRegistrator.hpp>

namespace ORO_DeviceInterface
{
    using namespace ORO_CoreLib;

    /**
     * @brief A class representing a Digital Input device with a maximum of 32
     * channels. When there are N bits, the bits are numbered from Zero to N-1.
     */
    class DigitalInInterface
                : private NameServerRegistrator<DigitalInInterface*, std::string>
    {
        public:
        /**
         * Construct a nameserved DigitalInInterface instance.
         * The object can then be retrieved using DigitalInInterface::nameserver .
         * 
         * @param name
         *        The name which will refer to this instance.
         */
        DigitalInInterface( const std::string& name )
            : NameServerRegistrator<DigitalInInterface*, std::string>
        ( nameserver, name, this )
        { }

        /**
         * Create a not nameserved DigitalInInterface instance.
         */
        DigitalInInterface()
        { }

            virtual ~DigitalInInterface()
            { }

            /**
             * The NameServer of this interface.
             * @see NameServer
             */
            static NameServer<DigitalInInterface*> nameserver;

            /**
             * Inspect if a certain bit is on.
             *
             * @param bit The bit to check, starting from zero.
             * @return true if the bit is high (1), false otherwise.
             */
            virtual bool isOn( unsigned int bit = 0) const = 0;

            /**
             * Inspect if a certain bit is on.
             *
             * @param bit The bit to check, starting from zero.
             * @return true if the bit is low (0), false otherwise.
             */
            virtual bool isOff( unsigned int bit = 0) const = 0;
            
            /**
             * Inspect a bit.
             *
             * @param bit The bit to check, starting from zero.
             * @return true if the bit is high (1), false otherwise.
             */
            virtual bool readBit( unsigned int bit = 0) const = 0;

            /**
             * Inspect a sequence of bits.
             *
             * @param start_bit The first bit to check.
             * @param stop_bit  The last bit (inclusive) to check.
             * @return a bit pattern denoting the bit status from <start_bit> to <stop_bit>
             *         where bit zero equals the value of <start_bit>.
             */
            virtual unsigned int readSequence(unsigned int start_bit, unsigned int stop_bit) const = 0;
            
            /**
             * Returns the number of bits that can be read for
             * digital input.
             */
            virtual unsigned int nbOfInputs() const = 0;
            
    };
};



#endif
