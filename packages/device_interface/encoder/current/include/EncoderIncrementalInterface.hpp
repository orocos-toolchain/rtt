/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  EncoderIncrementalInterface.hpp 

                        EncoderIncrementalInterface.hpp -  description
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
 
/*
  Adapted by Klaas Gadeyne
  - Extended API
  - Made this a template class
*/

#ifndef ENCODERINCREMENTAL_INTERFACE_HPP
#define ENCODERINCREMENTAL_INTERFACE_HPP

#include <corelib/NameServer.hpp>
#include <corelib/NameServerRegistrator.hpp>

namespace ORO_DeviceInterface
{
    using namespace ORO_CoreLib;

    /**
     * This interface is meant for reading the counter of the encoder card.
     */
    class EncoderIncrementalInterface
                : private NameServerRegistrator<EncoderIncrementalInterface*>
    {

        public:
            /**
             * Create a nameless EncoderIncrementalInterface.
             */
            EncoderIncrementalInterface()
            {}
            /**
             * Create a nameserved EncoderIncrementalInterface instance with
             * name <name>.
             */
            EncoderIncrementalInterface( const std::string& name )
                    : NameServerRegistrator<EncoderIncrementalInterface*, std::string>( nameserver, name, this )
            {}

            virtual ~EncoderIncrementalInterface()
            {}

            /**
             * Read the counter
             */
            virtual int readCounter( int& ) = 0;

            /**
             * Reset the counter
             *
             * @param value The value to set the counter to. Defaults to zero.
             */
            virtual void reset(int value = 0) = 0;

            static NameServer<EncoderIncrementalInterface*> nameserver;
    };

}

#endif

