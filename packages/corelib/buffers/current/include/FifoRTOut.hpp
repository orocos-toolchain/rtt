/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  FifoRTOut.hpp 

                        FifoRTOut.hpp -  description
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
 
#ifndef FIFORTOUT_HPP
#define FIFORTOUT_HPP


#include "WriteNonBlockingInterface.hpp"
#include "FifoRTCommon.hpp"

namespace ORO_CoreLib
{

    /**
     * An output fifo that can be used in real time space.
     * When no real time fifo support is present, a dummy implementation
     * will be used.
     */
    class FifoRTOut :
        public WriteNonBlockingInterface
    {
        public:
            /**
             * Create an output fifo with a size of <bytes> bytes,
             * associated with fifo-number <fifoNr>.
             * When deleted, it will destroy the fifo
             *
             * @post when there already existed a fifo with nr fifoNr before
             *     this constructor was called, no new fifo is created
             */
            FifoRTOut( unsigned int fifoNr, size_t bytes );

            /**
             * Wrap an output fifo around an already created rtosf
             * (associated with fifo-number <fifoNr>).
             * When this object is deleted, it won't destroy the fifo.
             */
            FifoRTOut( unsigned int fifoNr );

            /**
             * Destroy (wrapper) fifo.
             */
            ~FifoRTOut();

            /**
             * Write to the fifo.
             */
            virtual int write( const char* msg, size_t bytes );

            virtual int put( char c );

            /**
             * Return the fifo number.
             */
            unsigned int fifoNr() const;

        protected:
            // forbidden
            FifoRTOut()
            {}

        private:
            unsigned int fifo;
            /**
             * Flag indicating whether the fifo was created in the
             * constructor or not.
             */
            bool created;
    };

} 

#endif // FIFORTOUT_HPP
