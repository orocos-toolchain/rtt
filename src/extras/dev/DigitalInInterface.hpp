/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  DigitalInInterface.hpp

                        DigitalInInterface.hpp -  description
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


#ifndef DIGITALININTERFACE_HPP
#define DIGITALININTERFACE_HPP

#include "NameServer.hpp"
#include "NameServerRegistrator.hpp"
#include "../rtt-config.h"

namespace RTT
{ namespace dev {

    /**
     * A class representing a Digital Input device from which a
     * maximum of 32 bits can be read at once. When there are N
     * bits, the bits are numbered from Zero to N-1.
     * @see TemplateDigitalIn
     * @ingroup DeviceInterface
     */
    class RTT_API DigitalInInterface
        : private NameServerRegistrator<DigitalInInterface*>
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
            : NameServerRegistrator<DigitalInInterface*>
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
             * @return a bit pattern denoting the bit status from \a start_bit to \a stop_bit
             *         where bit zero equals the value of \a start_bit.
             */
            virtual unsigned int readSequence(unsigned int start_bit, unsigned int stop_bit) const = 0;

            /**
             * Returns the number of bits that can be read for
             * digital input.
             */
            virtual unsigned int nbOfInputs() const = 0;

    };
}};



#endif
