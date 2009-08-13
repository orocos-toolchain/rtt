#ifndef ORO_CHANNEL_BUFFER_ELEMENT_HPP
#define ORO_CHANNEL_BUFFER_ELEMENT_HPP

#include "../base/ChannelInterface.hpp"

namespace RTT {
    /** A connection element that can store a fixed number of data samples.
     */
    template<typename T>
    class ChannelBufferElement : public ChannelElement<T>
    {
        typename BufferInterface<T>::shared_ptr buffer;

    public:
        typedef typename ChannelElement<T>::param_t param_t;
        typedef typename ChannelElement<T>::reference_t reference_t;

        ChannelBufferElement(typename BufferInterface<T>::shared_ptr buffer)
            : buffer(buffer) {}
 
        /** Appends a sample at the end of the FIFO
         *
         * @return true if there was room in the FIFO for the new sample, and false otherwise.
         */
        virtual bool write(param_t sample)
        {
            if (buffer->Push(sample))
                return this->signal();
            return true;
        }

        /** Pops and returns the first element of the FIFO
         *
         * @return false if the FIFO was empty, and true otherwise
         */
        virtual bool read(reference_t sample)
        { return buffer->Pop(sample); }

        /** Removes all elements in the FIFO. After a call to clear(), read()
         * will always return false (provided write() has not been called in the
         * meantime).
         */
        virtual void clear()
        { 
            buffer->clear();
            ChannelElement<T>::clear();
        }
    };
}

#endif

