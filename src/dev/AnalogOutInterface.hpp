/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  AnalogOutInterface.hpp 

                        AnalogOutInterface.hpp -  description
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
/* Klaas Gadeyne, Mon August 11 2003
   - Added "channel" param to lowest(), highest(), resolution() calls (these
   are mostly configurable on a per channel basis.  If not, there's
   no problem
   - In comedi, this is also done for their "binary"-counterpoles.  I
   don't know if this is necessary

   Klaas Gadeyne, Wed August 20 2003
   - Added rangeSet() and arefSet() methods, that allready existed in
   the comedi implementations of these interfaces
*/ 

#ifndef ANALOGOUTINTERFACE_HPP
#define ANALOGOUTINTERFACE_HPP

#include "../NameServer.hpp"
#include "../NameServerRegistrator.hpp"

namespace RTT
{

    /**
     * An interface for writing analog output, like
     * for addressing a whole subdevice in comedi
     * 
     * Measurement Unit (MU) : Unit of what is actually read on the analog channel (e.g. Volt)
     * 
     * @ingroup DeviceInterface
     */
    class AnalogOutInterface
        : private NameServerRegistrator<AnalogOutInterface*>
    {
    public:

        /**
         * This enum can be used to configure the \a arefSet() function.
         * @see http://www.comedi.org
         */
        enum AnalogReference { Ground = 0, /** Reference to ground */
                               Common, /** Common reference */
                               Differential, /** Differential reference */
                               Other /** Undefined */
        };

        /**
         * Create a not nameserved AnalogOutInterface instance.
         */
        AnalogOutInterface()
        {}

        /**
         * Create a nameserved AnalogOutInterface. When \a name is not "" and
         * unique, it can be retrieved using the AnalogOutInterface::nameserver.
         */
        AnalogOutInterface( const std::string& name )
            : NameServerRegistrator<AnalogOutInterface*>( nameserver, name, this )
        {}

        virtual ~AnalogOutInterface()
        {}

        /** 
         * Set the range of a particular channel.  We took (for
         * now) the comedi API for this, where every range
         * (eg. -5/+5 V) corresponds to an unsigned int.  You
         * should provide a mapping from that int to a particular
         * range in your driver documentation
         */
        virtual void rangeSet(unsigned int chan, 
                              unsigned int range) = 0;

        /** 
         * Set the analog reference of a particular channel.  We took (for
         * now) the comedi API for this, where every aref
         * (eg. Analog reference set to ground (aka AREF_GROUND)
         * corresponds to an unsigned int.
         * @see AnalogReference.
         */
	    virtual void arefSet(unsigned int chan, 
                             unsigned int aref) = 0;

        /**
         * Write a raw \a value to channel \a chan
         * @return 0 on success.
         */
        virtual int rawWrite( unsigned int chan, unsigned int value ) = 0;

        /**
         * Read the current raw value from channel \a chan.
         */
        virtual int rawRead( unsigned int chan, unsigned int& value ) = 0;

        /**
         * Returns the absolute raw range (e.g. 12bits DA -> 4096)
         */
        virtual unsigned int rawRange() const = 0;

        /**
         * Write a MU \a value to channel \a chan
         * @return 0 on success.
         */
        virtual int write( unsigned int chan, double value ) = 0;

        /**
         * Read the current MU value from channel \a chan.
         */
        virtual int read( unsigned int chan, double& value ) = 0;

        /**
         * Returns the current lowest measurable input expressed
         * in MU's for a given channel
         */
        virtual double lowest(unsigned int chan) const = 0;

        /**
         * Returns the highest measurable input expressed in MU's
         * for a given channel
         */
        virtual double highest(unsigned int chan) const = 0;

        /**
         * Resolution is expressed in bits / MU
         */
        virtual double resolution(unsigned int chan) const = 0;
            
        /**
         * Returns the total number of channels.
         */
        virtual unsigned int nbOfChannels() const = 0;

        /**
         * Returns the binary range (e.g. 12bits DA -> 4096)
         * @deprecated by rawRange()
         */
        unsigned int binaryRange() const { return rawRange(); }

        /**
         * Returns the binary lowest value.
         * @deprecated Do not use. Should return zero in all implementations.
         */
        int binaryLowest() const { return 0; }

        /**
         * Returns the binary highest value
         * @deprecated Do not use. Should return rawRange() in all implementations.
         */
        int binaryHighest() const { return rawRange(); }

        /**
         * The NameServer for this interface.
         * @see NameServer
         */
        static NameServer<AnalogOutInterface*> nameserver;
    };
}

#endif
