// $Id: AnalogInInterface.hpp,v 1.10 2003/08/20 08:16:55 kgadeyne Exp $
/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  AnalogInInterface.hpp 

                        AnalogInInterface.hpp -  description
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
   
#ifndef ANALOGININTERFACE_HPP
#define ANALOGININTERFACE_HPP

#include <corelib/NameServer.hpp>
#include <corelib/NameServerRegistrator.hpp>

namespace ORO_DeviceInterface
{

    using namespace ORO_CoreLib;

    /**
     * An interface for reading analog input, like
     * for addressing a whole subdevice in comedi
     *
     *  Unit (MU) : Unit of what is actually read on the analog channel (e.g. Volt)
     * 
     */
    template < class T>
    class AnalogInInterface
                : private NameServerRegistrator<AnalogInInterface<T>*, std::string>
    {
        public:
            
            /**
             * Create a not nameserved AnalogInInterface instance.
             */
            AnalogInInterface( ) 
            {}

            /**
             * Create a nameserved AnalogInInterface. When <name> is not "" and
             * unique, it can be retrieved using the AnalogOutInterface::nameserver.
             */
            AnalogInInterface( const std::string& name ) : NameServerRegistrator<AnalogInInterface<T>*, std::string>( nameserver, name, this )
            {}

            virtual ~AnalogInInterface()
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
	      * corresponds to an unsigned int (more particular, a
	      * #define statement (like #define AREF_GROUND 0).  You
	      * should provide a mapping from that int to a particular
	      * analog reference in your driver documentation
	      */
	    virtual void arefSet(unsigned int chan, 
				 unsigned int aref) = 0;

            /**
             * Read <value> from channel <chan>
             */
            virtual void read( unsigned int chan, T& value ) const = 0;

            /**
             * Returns the binary range (e.g. 12bits AD -> 4096)
             */
            virtual T binaryRange() const = 0;

            /**
             * Returns the binary lowest value.
             */
            virtual T binaryLowest() const = 0;

            /**
             * Returns the binary highest value
             */
            virtual T binaryHighest() const = 0;

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
             * The NameServer for this interface.
             * @see NameServer
             */
            static NameServer<AnalogInInterface<T> *> nameserver;

        private:

    };

    template <class T>
    NameServer<AnalogInInterface<T> *> AnalogInInterface<T>::nameserver;
};

#endif
