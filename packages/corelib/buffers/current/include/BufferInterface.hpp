/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  BufferInterface.hpp 

                        BufferInterface.hpp -  description
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
 
#ifndef ORO_CORELIB_BUFFERINTERFACE_HPP
#define ORO_CORELIB_BUFFERINTERFACE_HPP

#include "ReadInterface.hpp"
#include "WriteInterface.hpp"

namespace ORO_CoreLib
{
    /**
     * A buffer is a object where you can write byte data to and read byte data from.
     */
    class BufferInterface :
        public ReadInterface,
        public WriteInterface
    {
        typedef unsigned int size_t;
    public:
        virtual ~BufferInterface()
        {}

        /**
         * Returns the maximum number of bytes that can be stored in the
         * buffer.
         * @return maximum number of bytes
         */
        virtual size_t capacityGet() = 0;

    };
}

#endif
