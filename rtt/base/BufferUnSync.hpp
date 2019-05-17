/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  BufferUnSync.hpp

                        BufferUnSync.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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



#ifndef ORO_CORELIB_BUFFER_UNSYNC_HPP
#define ORO_CORELIB_BUFFER_UNSYNC_HPP

#include "BufferInterface.hpp"
#include <deque>

namespace RTT
{ namespace base {


    /**
     * Implements a \b not threadsafe buffer. Only use when no more than one
     * thread accesses this buffer at a time.
     *
     * @see BufferLockFree, BufferUnSync
     * @ingroup PortBuffers
     */
    template<class T>
    class BufferUnSync
        :public BufferInterface<T>
    {
    public:
        typedef typename BufferBase::Options Options;
        typedef typename BufferInterface<T>::reference_t reference_t;
        typedef typename BufferInterface<T>::param_t param_t;
        typedef typename BufferInterface<T>::size_type size_type;
        typedef T value_t;

        /**
         * Create an uninitialized buffer of size \a size.
         */
        BufferUnSync( size_type size, const Options &options = Options() )
            : cap(size), buf(), mcircular(options.circular()), initialized(false), droppedSamples(0)
        {
        }

        /**
         * Create a buffer of size \a size.
         */
        BufferUnSync( size_type size, param_t initial_value, const Options &options = Options() )
            : cap(size), buf(), mcircular(options.circular()), initialized(false), droppedSamples(0)
        {
            data_sample(initial_value);
        }

        virtual bool data_sample( param_t sample, bool reset = true )
        {
            if (!initialized || reset) {
                buf.resize(cap, sample);
                buf.resize(0);
                return true;
            } else {
                return initialized;
            }
        }

        virtual value_t data_sample() const
        {
            return lastSample;
        }

        /**
         * Destructor
         */
        ~BufferUnSync() {}

        bool Push( param_t item )
        {
            if (cap == (size_type)buf.size() ) {
                //buffer is full, we either overwrite a sample, or drop the given one
                droppedSamples++;
                if (!mcircular)
                {
                    return false;
                }
                else
                {
                    buf.pop_front();
                }
            }
            buf.push_back( item );
            return true;
        }

        size_type Push(const std::vector<value_t>& items)
        {
            typename std::vector<value_t>::const_iterator itl( items.begin() );
            if (mcircular && (size_type)items.size() >= cap ) {
                // clear out current data and reset iterator to first element we're going to take.
                buf.clear();
                //note the ignored samples are added below to the dropped samples.
                droppedSamples += cap;
                itl = items.begin() + ( items.size() - cap );
            } else if ( mcircular && (size_type)(buf.size() + items.size()) > cap) {
                // drop excess elements from front
                assert( (size_type)items.size() < cap );
                while ( (size_type)(buf.size() + items.size()) > cap )
                {
                    droppedSamples++;
                    buf.pop_front();
                }
                // itl still points at first element of items.
            }
            while ( ((size_type)buf.size() != cap) && (itl != items.end()) ) {
                buf.push_back( *itl );
                ++itl;
            }
            
            size_type written = (itl - items.begin());
            
            droppedSamples += items.size() - written;
            
            return written;
        }

        FlowStatus Pop( reference_t item )
        {
            if ( buf.empty() ) {
                return NoData;
            }
            item = buf.front();
            buf.pop_front();
            return NewData;
        }

        size_type Pop(std::vector<value_t>& items )
        {
            int quant = 0;
            items.clear();
            while ( !buf.empty() ) {
                items.push_back( buf.front() );
                buf.pop_front();
                ++quant;
            }
            return quant;
        }

        value_t* PopWithoutRelease()
        {
            if(buf.empty())
                return 0;

            //note we need to copy the sample, as
            //front is not garanteed to be valid after
            //any other operation on the deque
            lastSample = buf.front();
            buf.pop_front();
            return &lastSample;
        }

        void Release(value_t *item)
        {
            //we do not need to release any memory, but we can check
            //if the other side messed up
            assert(item == &lastSample && "Wrong pointer given back to buffer");
        }
	
        size_type capacity() const {
            return cap;
        }

        size_type size() const {
            return buf.size();
        }

        void clear() {
            buf.clear();
        }

        bool empty() const {
            return buf.empty();
        }

        bool full() const {
            return (size_type)buf.size() ==  cap;
        }

        virtual size_type dropped() const
        {
            return droppedSamples;
        }
    private:
        size_type cap;
        std::deque<value_t> buf;
        value_t lastSample;
        const bool mcircular;
        bool initialized;
        size_type droppedSamples;
    };
}}

#endif // BUFFERSIMPLE_HPP
