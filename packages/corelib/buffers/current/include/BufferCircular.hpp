/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  BufferCircular.hpp 

                        BufferCircular.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
#ifndef BUFFERCIRCULAR_HPP
#define BUFFERCIRCULAR_HPP

#include <os/Mutex.hpp>
#include "BufferInterface.hpp"

namespace ORO_CoreLib
{
    /**
     * This class implements a blocking ring buffer, it supports writing and reading
     * for temporary storage in a ring buffer of a fixed size.
     */
    class BufferCircular
        :public BufferInterface
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
            size_t capacityGet();

            /**
            * Puts 'size' number of bytes from the given address 'buffer' into
            * the circular buffer.
            *
            * @param buffer the source address
            * @param size the number of bytes to be copied from the source 
            * address
            */
            int write( const char * buffer, size_t size );

            /**
             * Gets values from the circular buffer and stores them to 
             * the given address.
             *
             * @param buffer the destination address
             * @param size the number of bytes to be copied from the circular
             * buffer to the destination address
             */
            int read( char * buffer, size_t size );

        private:
            char * _buffer;

            size_t _size;
            size_t _rdptr;
            size_t _wrptr;

            ORO_OS::Mutex bufLock;
    };
}

#endif
