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

#ifndef ORO_CORELIB_BUFFERBASE_HPP
#define ORO_CORELIB_BUFFERBASE_HPP

#include <boost/shared_ptr.hpp>
#include "../rtt-config.h"
#include "../rtt-fwd.hpp"

namespace RTT
{ namespace base {

    /**
     * Any Buffer has a capacity, size and can be empty or full.
     * This class contains the type-independent methods of a Buffer.
     * @ingroup PortBuffers
     */
    class RTT_API BufferBase
    {
    public:
        typedef int size_type;
        typedef boost::shared_ptr< BufferBase > shared_ptr;

        class Options {
        private:
            bool circular_;
            int max_threads_;
            bool multiple_writers_;
            bool multiple_readers_;
        public:
            Options();
            Options(bool circular); // implicit conversion from bool for backwards-compatibility only
            Options(const ConnPolicy &policy);
            bool circular() const { return circular_; }
            Options &circular(bool value) { circular_ = value; return *this; }
            unsigned int max_threads() const { return max_threads_; }
            Options &max_threads(unsigned int value) { max_threads_ = value; return *this; }
            bool multiple_writers() const { return multiple_writers_; }
            Options &multiple_writers(bool value) { multiple_writers_ = value; return *this; }
            bool multiple_readers() const { return multiple_readers_; }
            Options &multiple_readers(bool value) { multiple_readers_ = value; return *this; }
        };

        virtual ~BufferBase();

        /**
         * Returns the maximum number of items that can be stored in the
         * buffer.
         * @return maximum number of items.
         * @cts
         * @rt
         */
        virtual size_type capacity() const = 0;

        /**
         * Returns the actual number of items that are stored in the
         * buffer.
         * @return number of items.
         * @cts
         * @rt
         */
        virtual size_type size() const = 0;

        /**
         * Check if this buffer is empty.
         * @return true if size() == 0
         * @cts
         * @rt
         */
        virtual bool empty() const = 0;

        /**
         * Check if this buffer is full.
         * @return true if size() == capacity()
         * @cts
         * @rt
         */
        virtual bool full() const = 0;

        /**
         * Clears all contents of this buffer.
         * @cts
         * @rt
         */
        virtual void clear() = 0;
        
        /**
         * Returns the number of dropped samples, because the buffer was full
         * */
        virtual size_type dropped() const = 0;
    };
}}

#endif
