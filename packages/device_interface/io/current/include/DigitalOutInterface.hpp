/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  DigitalOutInterface.hpp 

                        DigitalOutInterface.hpp -  description
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
 
 

#ifndef DIGITALOUTINTERFACE_HPP
#define DIGITALOUTINTERFACE_HPP

#include <corelib/NameServer.hpp>
#include <corelib/NameServerRegistrator.hpp>

namespace ORO_DeviceInterface
{
    using namespace ORO_CoreLib;

    /**
     * @brief A class representing a Digital Output device with a maximum of 32
     * channels. When there are N bits, the bits are numbered from Zero to N-1.
     *
     * @invar Bit 0 denotes the first digital output on the card.
     * @invar No more than 32 bits can be addressed 
     */
    class DigitalOutInterface
                : private NameServerRegistrator<DigitalOutInterface*>
    {

        public:
            /**
             * Create a DigitalOutInterface with an optional name.
             * When <name> is not "", and unique, it can be retrieved
             * through DigitalOutInterface::nameserver .
             */
            DigitalOutInterface( const std::string& name )
                    : NameServerRegistrator<DigitalOutInterface*>( nameserver, name, this )
            { }

            /**
             * Create a not nameserverd DigitalOutInterface instance.
             */
            DigitalOutInterface()
            { }

            virtual ~DigitalOutInterface()
            { }

            /**
             * The NameServer of this interface.
             * @see NameServer
             */
            static NameServer<DigitalOutInterface*> nameserver;

            typedef unsigned int DWord;

            /**
             * Sets the n'th output on
             * @param n The number of the output to be switched on.
             *          The first output is number 0.
             */
            virtual void switchOn( unsigned int n ) = 0;

            /**
             * Sets the n'th output off
             * @param n The number of the output to be switched off.
             *          The first output is number 0.
             */
            virtual void switchOff( unsigned int n ) = 0;

            /**
             * Sets the n'th output to <value>.
             */
            virtual void setBit( unsigned int bit, bool value ) = 0;
                
            /**
             * Sets a sequence of bits to pattern <value>.
             */
            virtual void setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value) = 0;

            /**
             * Returns the status of bit <n>, starting from zero.
             * @return true if the bit is 1, false otherwise.
             */
            virtual bool checkBit(unsigned int n) const = 0;

            /**
             * Returns the sequence of bits between <start_bit> and <stop_bit> inclusive.
             */
            virtual unsigned int checkSequence( unsigned int start_bit, unsigned int stop_bit ) const = 0;
            
            /**
             * Query the number of outputs of this card
             * @return the number of digital output channels addressable
             */
            virtual unsigned int nbOfOutputs() const = 0;

    };

};


#endif
