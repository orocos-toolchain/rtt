/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  BufferInterface.hpp

                        BufferInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef ORO_CORELIB_BUFFERINTERFACE_HPP
#define ORO_CORELIB_BUFFERINTERFACE_HPP

#include "BufferBase.hpp"
#include "../FlowStatus.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/call_traits.hpp>
#include <vector>

namespace RTT
{ namespace base {

    /**
     * A Buffer is an object which is used to store (Push) and retrieve (Pop) values from.
     * @param T The value type stored in this buffer.
     * @ingroup PortBuffers
     */
    template<class T>
    class BufferInterface :
        public BufferBase
    {
    public:
        typedef T value_t;
        typedef BufferBase::size_type size_type;

        typedef boost::shared_ptr< BufferInterface<T> > shared_ptr;
        typedef typename boost::call_traits<T>::param_type param_t;
        typedef typename boost::call_traits<T>::reference reference_t;

        virtual ~BufferInterface()
        {}

        /**
         * Read the oldest value from the buffer.
         * @param item is to be set with a value from the buffer.
         * @return true if something was read.
         * @cts
         * @rt
         */
        virtual FlowStatus Pop( reference_t item) = 0;

        /**
         * Read the whole buffer.
         * @param items is to be filled with all values in the buffer,
         * with \a items.begin() the oldest value.
         * @return the number of items read.
         * @cts
         * @rt
         */
        virtual size_type Pop( std::vector<value_t>& items ) = 0;

        /**
         * Returns a pointer to the first element in the buffer.
         * The pointer is only garanteed to stay valid until
         * the next pop operation.
         *
         * Note the pointer needs the be released by calling Release
         * on the buffer.
         *
         * @return a pointer to a sample or Zero if buffer is empty
         **/
        virtual value_t* PopWithoutRelease() = 0;

        /**
         * Releases the pointer
         * @param item pointer aquired using PopWithoutRelease()
         **/
        virtual void Release(value_t *item) = 0;
	
        /**
         * Write a single value to the buffer.
         * @param item the value to write
         * @return false if the buffer is full.
         * @cts
         * @rt
         */
        virtual bool Push( param_t item) = 0;

        /**
         * Write a sequence of values to the buffer.
         * @param items the values to write
         * @return the number of values written (may be less than items.size())
         * @cts
         * @rt
         */
        virtual size_type Push( const std::vector<value_t>& items ) = 0;

        /**
         * Initializes this buffer with a data sample, such that for
         * dynamical allocated types T, the buffer can reserve place
         * to hold these elements.
         * @param sample the data sample
         * @param reset enforce reinitialization even if this operation clears all data.
         * @return true if the buffer was successfully (re)initialized.
         * @post Calling this function causes all data in the buffer
         * to be lost and the size being reset to zero.
         * @nts
         * @nrt
         */
        virtual bool data_sample( param_t sample, bool reset = true ) = 0;

        /**
         * Reads back a data sample.
         */
        virtual value_t data_sample() const = 0;
    };
}}

#endif
