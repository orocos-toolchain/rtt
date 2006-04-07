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
 
#ifndef ORO_CORELIB_BUFFERBASE_HPP
#define ORO_CORELIB_BUFFERBASE_HPP

#include <boost/shared_ptr.hpp>

namespace ORO_CoreLib
{
    /**
     * Any Buffer has a capacity, size and can be empty or full.
     * This class contains the type-independent methods of a Buffer.
     */
    class BufferBase
    {
    public:
        typedef unsigned int size_type;

        typedef boost::shared_ptr< BufferBase > shared_ptr;

        virtual ~BufferBase()
        {}

        /**
         * Returns the maximum number of items that can be stored in the
         * buffer.
         * @return maximum number of items.
         */
        virtual size_type capacity() const = 0;

        /**
         * Returns the actual number of items that are stored in the
         * buffer.
         * @return number of items.
         */
        virtual size_type size() const = 0;

        /**
         * Check if this buffer is empty.
         * @return true if empty
         */
        virtual bool empty() const = 0;

        /**
         * Clears all contents of this buffer.
         */
        virtual void clear() = 0;
    };
}

#endif
