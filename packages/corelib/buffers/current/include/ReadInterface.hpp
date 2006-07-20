/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  ReadInterface.hpp 

                        ReadInterface.hpp -  description
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
 

#ifndef ORO_CORELIB_READINTERFACE_HPP
#define ORO_CORELIB_READINTERFACE_HPP

#include <boost/call_traits.hpp>
#include <vector>
#include "BufferBase.hpp"

namespace RTT
{

    /**
     * This interface describes a read interface for value types.
     * @param T The value type read from this buffer.
     */
    template<class T>
    class ReadInterface
        : public virtual BufferBase
    {
    public:
        typedef T value_t;
        typedef typename boost::call_traits<T>::reference reference_t;

        typedef BufferBase::size_type size_type;

        /**
         * Read the oldest value from the buffer.
         * @param item is to be set with a value from the buffer.
         * @return true if something was read.
         */
        virtual bool Pop( reference_t item) = 0;

        /**
         * Read the whole buffer.
         * @param items is to be filled with all values in the buffer,
         * with \a items.begin() the oldest value.
         * @return the number of items read.
         */
        virtual size_type Pop( std::vector<value_t>& items ) = 0;

        /**
         * Get the next value to be Pop()'ed, or
         * the default value if empty.
         */
        virtual value_t front() const = 0;

        virtual ~ReadInterface()
        {}

    };
}

#endif // READINTERFACE_HPP

