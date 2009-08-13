#ifndef ORO_CHANNEL_ELEMENT_HPP
#define ORO_CHANNEL_ELEMENT_HPP

#include <boost/intrusive_ptr.hpp>
#include <boost/call_traits.hpp>
#include "ChannelElementBase.hpp"

namespace RTT {

    /** A typed version of ChannelElementBase. It defines generic methods that are
     * type-specific (like write and read)
     */
    template<typename T>
    class ChannelElement : public ChannelElementBase
    {
    public:
        typedef T value_t;
        typedef boost::intrusive_ptr< ChannelElement<T> > shared_ptr;
        typedef typename boost::call_traits<T>::param_type param_t;
        typedef typename boost::call_traits<T>::reference reference_t;

        /** Writes a new sample on this connection. \a sample is the sample to
         * write. 
         *
         * @returns false if an error occured that requires the channel to be invalidated. In no ways it indicates that the sample has been received by the other side of the channel.
         */
        virtual bool write(param_t sample)
        {
            typename ChannelElement<T>::shared_ptr output = boost::static_pointer_cast< ChannelElement<T> >(this->output);
            if (output)
                return output->write(sample);
            return false;
        }

        /** Reads a sample from the connection. \a sample is a reference which
         * will get updated if a sample is available. The method returns true
         * if a sample was available, and false otherwise. If false is returned,
         * then \a sample is not modified by the method
         */
        virtual bool read(reference_t sample)
        {
            typename ChannelElement<T>::shared_ptr input = static_cast< ChannelElement<T>* >(this->input);
            if (input)
                return input->read(sample);
            else return false;
        }
    };
}

#endif

