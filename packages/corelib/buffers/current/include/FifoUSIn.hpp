/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  FifoUSIn.hpp 

                        FifoUSIn.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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
 
 
#ifndef FIFOUSIN_HPP
#define FIFOUSIN_HPP

#include "os/fosi.h"

#include <cstdio>
#include <fcntl.h>

#include "ReadInterface.hpp"

namespace ORO_CoreLib
{

    /**
     * A FifoUSIn is meant for reading from /dev/... fifos from userspace.
     * one can write to a fifo by using FifoUSOut from userspace or 
     * FifoRTOut in kernel space. You have to create the fifo first in
     * kernel space to be able to specify the fifos size.
     */
    class FifoUSIn 
        : public ReadInterface
    {

        public:

            /**
             * Wrap an input fifo around an already created rtosf
             * (associated with fifo-number <fifoNr>).
             * When this object is deleted, it won't destroy the fifo.
             */
            FifoUSIn( unsigned int fNr );

            /**
             * Destroy (wrapper) fifo.
             */
            ~FifoUSIn();

            /**
             * Read blocking from the fifo.
             */
            int read( char* buf, size_t length );

            /**
             * Return the fifo number.
             */
            unsigned int fifoNr() const;

        protected:
            // forbidden
            FifoUSIn();

        private:
        unsigned int okr_len;
        unsigned int fNr;
        FILE* fifo;
    };

} // namespace

#endif 
