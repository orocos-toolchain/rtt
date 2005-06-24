/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:32:59 CEST 2002  BufferSimple.hpp 

                       BufferSimple.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

   update (08-08-2002)  : Jan.Veldeman@student.kuleuven.ac.be

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/



#ifndef ORO_CORELIB_BUFFERSIMPLE_HPP
#define ORO_CORELIB_BUFFERSIMPLE_HPP

#include <os/Mutex.hpp>
#include <os/MutexLock.hpp>
#include "BufferInterface.hpp"

namespace ORO_CoreLib
{

    /**
     * Implements a very simple blocking threadsafe buffer using swinging buffers.
     * A reader will always read the last written value, so it is possible
     * the reader won't read all 'messages' sent.
     *
     * @deprecated by BufferLockFree
     */
    class BufferSimple
        :public BufferInterface
    {
        public:
            /**
             * Create a buffer of size \a length.
             */
            BufferSimple( size_t size );

            /**
             * Destructor
             */
            ~BufferSimple();

            /**
             * Write \a length bytes of buffer.
             * The next read will receive the bytes from buffer.
             * All data pending to be read will be overwritten.
             * If the length specified is larger than the capacity of
             * this buffer, only the first part will be written.
             */
            int write( const char* buffer, size_t length );

            /**
             * Read \a length bytes of buffer.
             * Only the number of bytes from the last write will be read.
             * If length is smaller than the current data stored, 
             * the remaining data is lost.
             * A next read (before a write) will return 0.
             */
            int read( char *buffer, size_t length );

            size_t capacityGet();

        private:

            /**
             * updates writer and nextreader buffer pointer
             */
            void updatePointerWrite();

            /**
             * updates read pointer.
             */
            void updatePointerRead();

            /**
            * The number of buffers containing the data.
            */
            const static int MAX_BUFFERS = 3;

            /**
             * The 3 buffers containing the data.
             */
            char* buffers[ MAX_BUFFERS ];

            /**
             * lengths of data in buffer, these values must always be smaller
             * than (or equal to) the size of the buffer
             */
            size_t lengths[ MAX_BUFFERS ];

            size_t buffersize;
            unsigned int writeBuffer;
            unsigned int readBuffer;
            unsigned int nextReadBuffer;

            ORO_OS::Mutex pointerLock;

    };
}

#endif // BUFFERSIMPLE_HPP
