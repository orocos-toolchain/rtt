/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:43 CEST 2002  SchunkCommunicationRTcom.hpp 

                        SchunkCommunicationRTcom.hpp -  description
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
 
#ifndef SCHUNKCOMMUNICATIONRTCOM_HPP
#define SCHUNKCOMMUNICATIONRTCOM_HPP

#include <rt_com.h>
#include <rt_comP.h>
#include <corelib/Event.hpp>

#define RING_BUFFER_T_SIZE      256

namespace ORO_DeviceDriver
{

    class SchunkCommunicationRTcom : public SchunkCommunicationRTcomInterface,
                public EventListenerInterface
                public EventCompleterInterface
    {

        public:

        private:
            Event* data_available;
            unsigned int maskNb = RING_BUFFER_T_SIZE - 1;
            int comport = 0;

            struct ring_buffer_t
            {
                pthread_cond_t cond;
                pthread_mutex_t mutex;
                unsigned int pRead = 0;
                unsigned int pWrite = 0;
                char buf[ RING_BUFFER_T_SIZE ];
            }

            ring_buffer;

            /**
             * Fire the Event.
             */
            void handler( unsigned int i );

            /**
             * Mask a to an element in the ringbuffer.
             * 
             * @param   a
             */
            void mask( int* a );

            /**
             * Write a number of characters to the position in the
             * ring_buffer_t pointed to by the write-pointer, move the
             * write-pointer to the next element.
             *
             * This function locks ring_buffer.
             *
             * @param   buf, the buffer that holds the charactes to write
             * @param   count, the number of characters to write
             * @return  the number of characters actually written
             */
            int ring_buffer_write( char* buf, int count );

            /**
             * Read a number of characters from the ring_buffer starting
             * at the positioin pointed to by the read-pointer, move the
             * read-pointer to the element following the last read
             * element.
             *
             * This functions doesn't lock ring_buffer because it is
             * called from within a function that does.
             *
             * @param   buf, the buffer into which the characters must be
             *          copied
             * @param   count, the number of characters read
             * @return  the number of characters actually read
             */
            int ring_buffer_read( char* buf, int count );

            /**
             * Returns the maximum number of characters that can be
             * read from ring_buffer.
             */
            int ring_buffer_getMaxReadLength();

            /**
             * Returns the maximum number of characters that can be
             * written to ring_buffer.
             */
            int ring_buffer_getMaxWriteLength();
    }
}

#endif //SCHUNKCOMMUNICATIONRTCOM_HPP
