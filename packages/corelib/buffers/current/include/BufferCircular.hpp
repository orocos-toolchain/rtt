/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  BufferCircular.hpp 

                        BufferCircular.hpp -  description
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
 
 
#ifndef BUFFERCIRCULAR_HPP
#define BUFFERCIRCULAR_HPP

#include "BufferInterface.hpp"
#include "ReadNonBlockingInterface.hpp"
#include "WriteNonBlockingInterface.hpp"
#include <os/Mutex.hpp>

namespace ORO_CoreLib
{
    /**
     * This class implements a ring buffer, it supports writing and reading
     * for temporary storage in a ring buffer of a fixed size.
     * @see ReadNonBlockingInterface
     * @see WriteNonBlockingInterface
     */

    class BufferCircular :
                public BufferInterface
    {

        public:
            /**
             * Creates a BufferCircular of size <size>.
             * @param size the size of the buffer
             */
            BufferCircular( size_t size );
            ~BufferCircular();

            /**
            * Returns the size of the buffer in bytes.
            * @return the size of the buffer.
            */
            virtual size_t capacityGet();

            /**
            * Puts 'size' number of bytes from the given address 'buffer' into
            * the circular buffer.
            *
            * @param buffer the source address
            * @param size the number of bytes to be copied from the source 
            * address
            */
            virtual int write( const char * buffer, size_t size );

            /**
             * Gets values from the circular buffer and stores them to 
             * the given address.
             *
             * @param buffer the destination address
             * @param size the number of bytes to be copied from the circular
             * buffer to the destination address
             */
            virtual int read( char * buffer, size_t size );

        private:
            char * _buffer;

            size_t _size;
            size_t _rdptr;
            size_t _wrptr;

            ORO_OS::Mutex bufLock;
    };
}

#endif
