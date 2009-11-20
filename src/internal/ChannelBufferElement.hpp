/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ChannelBufferElement.hpp

                        ChannelBufferElement.hpp -  description
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


#ifndef ORO_CHANNEL_BUFFER_ELEMENT_HPP
#define ORO_CHANNEL_BUFFER_ELEMENT_HPP

#include "../base/ChannelElement.hpp"
#include "../base/BufferInterface.hpp"
#include "../internal/DataSources.hpp"

namespace RTT { namespace internal {

    /** A connection element that can store a fixed number of data samples.
     */
    template<typename T>
    class ChannelBufferElement : public base::ChannelElement<T>
    {
        typename base::BufferInterface<T>::shared_ptr buffer;
        typename internal::AssignableDataSource<T>::shared_ptr last;
        bool written;
    public:
        typedef typename base::ChannelElement<T>::param_t param_t;
        typedef typename base::ChannelElement<T>::reference_t reference_t;

        ChannelBufferElement(typename base::BufferInterface<T>::shared_ptr buffer)
            : buffer(buffer), last ( new internal::ValueDataSource<T>() ), written(false) {}
 
        /** Appends a sample at the end of the FIFO
         *
         * @return true if there was room in the FIFO for the new sample, and false otherwise.
         */
        virtual bool write(param_t sample)
        {
            written = true;
            if (buffer->Push(sample))
                return this->signal();
            return true;
        }

        /** Pops and returns the first element of the FIFO
         *
         * @return false if the FIFO was empty, and true otherwise
         */
        virtual FlowStatus read(reference_t sample)
        {
            if ( buffer->Pop(sample) ) {
                last->set(sample);
                return NewData;
            }
            if (written) {
                sample = last->get();
                return OldData;
            }
            return NoData;
        }

        /** Removes all elements in the FIFO. After a call to clear(), read()
         * will always return false (provided write() has not been called in the
         * meantime).
         */
        virtual void clear()
        { 
            written = false;
            buffer->clear();
            base::ChannelElement<T>::clear();
        }

        virtual bool data_sample(param_t sample)
        {
            buffer->data_sample(sample);
            return base::ChannelElement<T>::data_sample(sample);
        }

    };
}}

#endif

