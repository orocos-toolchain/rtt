/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  DigitalOutInterface.hpp 

                        DigitalOutInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 

#ifndef DIGITALOUTINTERFACE_HPP
#define DIGITALOUTINTERFACE_HPP

#include "../NameServer.hpp"
#include "../NameServerRegistrator.hpp"

namespace RTT
{
    /**
     * A class representing a Digital Output device which can read or write a maximum of 32
     * bits at once. When there are N bits, the bits are numbered from Zero to N-1.
     *
     * @invariant Bit 0 denotes the first digital output on the card.
     * @invariant No more than 32 bits can be addressed 
     * @see TemplateDigitalOut
     * @ingroup DeviceInterface
     */
    class DigitalOutInterface
        : private NameServerRegistrator<DigitalOutInterface*>
    {

        public:
            /**
             * Create a DigitalOutInterface with an optional name.
             * When \a name is not "", and unique, it can be retrieved
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
             * Sets the n'th output to \a value
             */
            virtual void setBit( unsigned int bit, bool value ) = 0;
                
            /**
             * Sets a sequence of bits to pattern \a value between \a start_bit and \a stop_bit inclusive.
             * For example, setSequence(3, 3, 1) is equivalent to setBit(3, 1).
             */
            virtual void setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value) = 0;

            /**
             * Returns the status of bit \a n, starting from zero.
             * @return true if the bit is 1, false otherwise.
             */
            virtual bool checkBit(unsigned int n) const = 0;

            /**
             * Returns the sequence of bits between \a start_bit and \a stop_bit inclusive,
             * where start_bit occurs at position zero in the returned result.
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
