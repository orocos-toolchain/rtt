/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ChannelBufferElement.hpp

                        ChannelBufferElement.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Sylvain Joyeux
    email                : sylvain.joyeux@m4x.org

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
#include "../ConnPolicy.hpp"

namespace RTT { namespace internal {

    class ChannelBufferElementBase
    {
    public:
        virtual ~ChannelBufferElementBase() {};
        virtual size_t getBufferSize() const = 0;
        virtual size_t getBufferFillSize() const = 0;
        virtual size_t getNumDroppedSamples() const = 0;
    };
    
    /** A connection element that can store a fixed number of data samples.
     */
    template<typename T>
    class ChannelBufferElement : public base::ChannelElement<T>, public ChannelBufferElementBase
    {
        typename base::BufferInterface<T>::shared_ptr buffer;
        typename base::ChannelElement<T>::value_t *last_sample_p;
        const ConnPolicy policy;

    public:
        typedef typename base::ChannelElement<T>::value_t value_t;
        typedef typename base::ChannelElement<T>::param_t param_t;
        typedef typename base::ChannelElement<T>::reference_t reference_t;

        ChannelBufferElement(typename base::BufferInterface<T>::shared_ptr buffer, const ConnPolicy& policy = ConnPolicy())
            : buffer(buffer), last_sample_p(0), policy(policy) {}
            
        virtual ~ChannelBufferElement()
        {
            if(last_sample_p)
                buffer->Release(last_sample_p);
        }
 
        virtual size_t getBufferSize() const
        {
            return buffer->capacity();
        }
        
        virtual size_t getBufferFillSize() const
        {
            return buffer->size();
        }
        
        virtual size_t getNumDroppedSamples() const
        {
            return buffer->dropped();
        }
        
        /** Appends a sample at the end of the FIFO
         *
         * @return true if there was room in the FIFO for the new sample, and false otherwise.
         */
        virtual WriteStatus write(param_t sample)
        {
            if (!buffer->Push(sample)) return WriteFailure;
            return this->signal() ? WriteSuccess : NotConnected;
        }

        /** Pops and returns the first element of the FIFO
         *
         * @return false if the FIFO was empty, and true otherwise
         */
        virtual FlowStatus read(reference_t sample, bool copy_old_data)
        {
            value_t *new_sample_p;
            if ( (new_sample_p = buffer->PopWithoutRelease()) ) {
                if(last_sample_p)
                    buffer->Release(last_sample_p);

                sample = *new_sample_p;

                // In the PerOutputPort or Shared buffer policy case this buffer element may be read by multiple readers.
                // ==> We cannot store the last_sample and release immediately.
                // ==> WriteShared buffer connections will never return OldData.
                if (policy.buffer_policy != PerOutputPort && policy.buffer_policy != Shared)
                    last_sample_p = new_sample_p;
                else
                    buffer->Release(new_sample_p);

                return NewData;
            }
            if (last_sample_p) {
                if(copy_old_data)
                    sample = *(last_sample_p);
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
            if(last_sample_p)
                buffer->Release(last_sample_p);
            last_sample_p = 0;
            buffer->clear();
            base::ChannelElement<T>::clear();
        }

        virtual WriteStatus data_sample(param_t sample, bool reset = true)
        {
            if (!buffer->data_sample(sample, reset)) return WriteFailure;
            return base::ChannelElement<T>::data_sample(sample, reset);
        }

        virtual value_t data_sample()
        {
            return buffer->data_sample();
        }

        /** Returns a pointer to the ConnPolicy that has been used to construct the underlying buffer.
        */
        virtual const ConnPolicy* getConnPolicy() const
        {
            return &policy;
        }

        virtual std::string getElementName() const 
        {
            return "ChannelBufferElement";
        }
    };
}}

#endif

