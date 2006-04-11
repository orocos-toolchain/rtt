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
#include <boost/shared_ptr.hpp>

namespace ORO_CoreLib
{
    /**
     * A Buffer is an object which is used to store (Push) and retrieve (Pop) values from.
     * @param T The value type stored in this buffer.
     */
    template<class T>
    class BufferInterface :
        public ReadInterface<T>,
        public WriteInterface<T>
    {
    public:
        typedef T value_t;
        typedef BufferBase::size_type size_type;

        typedef boost::shared_ptr< BufferInterface<T> > shared_ptr;

        virtual ~BufferInterface()
        {}
    };
}

#endif
