/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  EncoderAbsoluteInterface.hpp 

                        EncoderAbsoluteInterface.hpp -  description
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
 
 

#ifndef ENCODERABSOLUTE_INTERFACE_HPP
#define ENCODERABSOLUTE_INTERFACE_HPP

#include <corelib/NameServerRegistrator.hpp>

namespace ORO_DeviceInterface
{
    using namespace ORO_CoreLib;

    /**
     * This interface reads the counter of an absolute encoder card.
     */
    class EncoderAbsoluteInterface 
                : private NameServerRegistrator<EncoderAbsoluteInterface*>
    {
        public:
            /**
             * Create a nameless EncoderAbsoluteInterface.
             */
            EncoderAbsoluteInterface()
            {}
            /**
             * Create a nameserved EncoderAbsoluteInterface instance with
             * name <name>.
             */
            EncoderAbsoluteInterface( const std::string& name )
                    : NameServerRegistrator<EncoderAbsoluteInterface*, std::string>( nameserver, name, this )
            {}

            virtual ~EncoderAbsoluteInterface()
            {}

            /**
             * Reads the absolute position of an encoder.
             *
             * @param position The position of the encoder.
             */
            virtual bool readPosition(int& position)=0;

            static NameServer<EncoderAbsoluteInterface*> nameserver;
    };

}

#endif

